#include "ContactMaterial.h"

#include "libmesh/elem.h"

#include <cmath>
#include <limits>

registerMooseObject("RedbackApp", ContactMaterial);

InputParameters
ContactMaterial::validParams()
{
  InputParameters params = Material::validParams();

  params.addRequiredCoupledVar("disp_x", "The x displacement");
  params.addRequiredCoupledVar("disp_y", "The y displacement");
  params.addCoupledVar("disp_z", 0.0, "The z displacement");

  params.addParam<Real>(
      "youngs_modulus_iso", 1e-9, "Small isotropic Young's modulus for regularization");
  params.addParam<Real>(
      "poisson_ratio_iso", 0.0, "Small isotropic Poisson ratio for regularization");

  params.addRequiredParam<Real>("k_n", "Contact stiffness scale");
  params.addParam<Real>("contact_exponent", 2.0, "Contact stiffening exponent");

  MooseEnum h0_method("normal_extent y_extent hmin equivalent_radius", "normal_extent");
  params.addParam<MooseEnum>("h0_method", h0_method, "Method to compute reference element height");

  MooseEnum contact_measure("J4 lambda", "J4");
  params.addParam<MooseEnum>(
      "contact_measure", contact_measure, "Compression measure used in anisotropic contact law");

  MooseEnum friction_model("none rate-independent rate-dependent rate-and-state", "none");
  params.addParam<MooseEnum>("friction_model", friction_model, "Tangential/friction model");

  params.addParam<Real>(
      "tangent_perturbation", 1e-8, "Perturbation size for numerical tangent calculation");

  return params;
}

ContactMaterial::ContactMaterial(const InputParameters & parameters)
  : Material(parameters),
    _grad_disp_x(coupledGradient("disp_x")),
    _grad_disp_y(coupledGradient("disp_y")),
    _grad_disp_z(coupledGradient("disp_z")),
    _dim(_mesh.dimension()),
    _youngs_modulus_iso(getParam<Real>("youngs_modulus_iso")),
    _poisson_ratio_iso(getParam<Real>("poisson_ratio_iso")),
    _k_n(getParam<Real>("k_n")),
    _contact_exponent(getParam<Real>("contact_exponent")),
    _h0_method(getParam<MooseEnum>("h0_method")),
    _contact_measure(getParam<MooseEnum>("contact_measure")),
    _friction_model(getParam<MooseEnum>("friction_model")),
    _tangent_perturbation(getParam<Real>("tangent_perturbation")),
    _stress(declareProperty<RankTwoTensor>("stress")),
    _Jacobian_mult(declareProperty<RankFourTensor>("Jacobian_mult")),
    _strain(declareProperty<RankTwoTensor>("strain")),
    _biot_coefficient(declareProperty<Real>("biot_coefficient")),
    _mixture_gravity_term(declareProperty<RealVectorValue>("mixture_gravity_term")),
    _lambda_n(declareProperty<Real>("lambda_n")),
    _J4(declareProperty<Real>("J4")),
    _closure(declareProperty<Real>("closure")),
    _closure_gap(declareProperty<Real>("closure_gap")),
    _h_current(declareProperty<Real>("h_current")),
    _h0(declareProperty<Real>("h0")),
    _h0_current(declareProperty<Real>("h0_current"))
{
}

void
ContactMaterial::initQpStatefulProperties()
{
}

RankTwoTensor
ContactMaterial::computeDeformationGradient() const
{
  RankTwoTensor grad_u;
  grad_u.zero();

  for (unsigned int j = 0; j < _dim; ++j)
  {
    grad_u(0, j) = _grad_disp_x[_qp](j);
    grad_u(1, j) = _grad_disp_y[_qp](j);

    if (_dim == 3)
      grad_u(2, j) = _grad_disp_z[_qp](j);
  }

  RankTwoTensor F = grad_u;
  F.addIa(1.0);
  return F;
}

Point
ContactMaterial::computeNormal() const
{
  const Elem * elem = _current_elem;

  Point N_eff;
  N_eff.zero();

  for (unsigned int s = 0; s < elem->n_sides(); ++s)
  {
    const Elem * neighbor = elem->neighbor_ptr(s);

    if (!neighbor)
      continue;

    if (neighbor->subdomain_id() == elem->subdomain_id())
      continue;

    std::unique_ptr<const Elem> side(elem->build_side_ptr(s));

    if (_dim == 2)
    {
      const Point & p0 = side->point(0);
      const Point & p1 = side->point(1);

      Point t = p1 - p0;
      const Real L = t.norm();

      if (L < 1e-14)
        continue;

      t /= L;

      Point n;
      n(0) = -t(1);
      n(1) =  t(0);
      n(2) =  0.0;

      const Point xc = elem->vertex_average();
      const Point xn = neighbor->vertex_average();
      const Point to_neighbor = xn - xc;

      if (n * to_neighbor < 0.0)
        n *= -1.0;

      N_eff += L * n;
    }
    else if (_dim == 3)
    {
      const Point & p0 = side->point(0);
      const Point & p1 = side->point(1);
      const Point & p2 = side->point(2);

      Point w1 = p1 - p0;
      Point w2 = p2 - p0;

      Point n = w1.cross(w2);
      const Real norm_n = n.norm();

      if (norm_n < 1e-14)
        continue;

      const Real A = 0.5 * norm_n;
      n /= norm_n;

      const Point xc = elem->vertex_average();
      const Point xn = neighbor->vertex_average();
      const Point to_neighbor = xn - xc;

      if (n * to_neighbor < 0.0)
        n *= -1.0;

      N_eff += A * n;
    }
  }

  const Real norm = N_eff.norm();

  if (norm < 1e-12)
  {
    mooseError(
        "ContactMaterial::computeNormal(): "
        "Failed to construct interface normal for element ",
        elem->id(),
        ". No valid interface sides were detected.");
  }
  
  return N_eff / norm;
}

Point
ContactMaterial::computeTangent(const Point & N0) const
{
  Point S0;

  if (_dim == 2)
  {
    S0(0) = -N0(1);
    S0(1) =  N0(0);
    S0(2) =  0.0;
  }
  else
  {
    Point a(1.0, 0.0, 0.0);

    if (std::abs(N0 * a) > 0.9)
      a = Point(0.0, 1.0, 0.0);

    S0 = a - (a * N0) * N0;

    const Real norm = S0.norm();

    if (norm < 1e-12)
      mooseError("computeTangent: failed to construct tangent.");

    S0 /= norm;
  }

  return S0;
}

Point
ContactMaterial::computeCurrentNormal(const RankTwoTensor & F,
                                      const Point & N0) const
{
  RankTwoTensor FinvT = F.inverse().transpose();

  Point n;
  n.zero();

  for (unsigned int i = 0; i < _dim; ++i)
    for (unsigned int j = 0; j < _dim; ++j)
      n(i) += FinvT(i, j) * N0(j);

  const Real norm = n.norm();

  if (norm < 1e-12)
    mooseError("computeCurrentNormal: zero norm.");

  return n / norm;
}

Point
ContactMaterial::computeCurrentTangent(const RankTwoTensor & F,
                                       const Point & S0) const
{
  Point s;
  s.zero();

  for (unsigned int i = 0; i < _dim; ++i)
    for (unsigned int j = 0; j < _dim; ++j)
      s(i) += F(i, j) * S0(j);

  const Real norm = s.norm();

  if (norm < 1e-12)
    mooseError("computeCurrentTangent: zero norm.");

  return s / norm;
}

Real
ContactMaterial::computeElementHeight(const Point & N0) const
{
  const Elem * elem = _current_elem;

  if (_h0_method == "hmin")
    return elem->hmin();

  if (_h0_method == "equivalent_radius")
  {
    const Real meas = elem->volume();

    if (_dim == 2)
      return std::sqrt(meas / libMesh::pi);
    else if (_dim == 3)
      return std::cbrt(3.0 * meas / (4.0 * libMesh::pi));
    else
      return meas;
  }

  if (_h0_method == "y_extent")
  {
    Real ymin = std::numeric_limits<Real>::max();
    Real ymax = -std::numeric_limits<Real>::max();

    for (unsigned int i = 0; i < elem->n_nodes(); ++i)
    {
      const Point & p = elem->point(i);
      ymin = std::min(ymin, p(1));
      ymax = std::max(ymax, p(1));
    }

    return std::max(ymax - ymin, 1e-14);
  }

  // Default and recommended: element extent in the reference normal direction
  Real smin = std::numeric_limits<Real>::max();
  Real smax = -std::numeric_limits<Real>::max();

  for (unsigned int i = 0; i < elem->n_nodes(); ++i)
  {
    const Point & p = elem->point(i);
    const Real s = p * N0;

    smin = std::min(smin, s);
    smax = std::max(smax, s);
  }

  return std::max(smax - smin, 1e-14);
}

RankTwoTensor
ContactMaterial::computeCauchyStress(const RankTwoTensor & F) const
{
  const RankTwoTensor C = F.transpose() * F;
  const Real J = F.det();

  if (J <= 0.0)
    mooseError("ContactMaterial: det(F) <= 0.");

  RankTwoTensor I;
  I.zero();
  I.addIa(1.0);

  const Point N0 = computeNormal();

  Point CN;
  CN.zero();

  for (unsigned int i = 0; i < _dim; ++i)
    for (unsigned int j = 0; j < _dim; ++j)
      CN(i) += C(i, j) * N0(j);

  const Real J4_raw = N0 * CN;
  const Real J4 = std::max(J4_raw, 1e-16);
  const Real lambda_n = std::sqrt(J4);

  const Real h0 = computeElementHeight(N0);

  RankTwoTensor M;
  M.zero();

  for (unsigned int i = 0; i < _dim; ++i)
    for (unsigned int j = 0; j < _dim; ++j)
      M(i, j) = N0(i) * N0(j);

  // ------------------------------------------------------------------
  // Isotropic stabilisation:
  //
  // W_stab = mu/2 * (tr(C) - 3)
  // S_stab = 2 dW/dC = mu I
  // ------------------------------------------------------------------
  const Real mu = _youngs_modulus_iso / (2.0 * (1.0 + _poisson_ratio_iso));
  const RankTwoTensor S_iso = mu * I;

  // ------------------------------------------------------------------
  // Compression-only anisotropic normal contact contribution
  // ------------------------------------------------------------------
  RankTwoTensor S_aniso;
  S_aniso.zero();

  const Real m = _contact_exponent;

  if (_contact_measure == "J4")
  {
    const Real g = std::max(1.0 - J4, 0.0);

    if (g > 0.0)
    {
      const Real term1 = std::pow(g, m) / J4;
      const Real term2 = std::pow(g, m + 1.0) / ((m + 1.0) * J4 * J4);

      const Real factor = -2.0 * _k_n / h0 * (term1 + term2);

      S_aniso = factor * M;
    }
  }
  else if (_contact_measure == "lambda")
  {
    const Real lambda_reg = std::max(lambda_n, 1e-12);
    const Real g = std::max(1.0 - lambda_reg, 0.0);

    if (g > 0.0)
    {
      const Real term1 = std::pow(g, m) / (lambda_reg * lambda_reg);
      const Real term2 =
          std::pow(g, m + 1.0) / ((m + 1.0) * std::pow(lambda_reg, 3.0));

      const Real factor = -_k_n / h0 * (term1 + term2);

      S_aniso = factor * M;
    }
  }
  else
    mooseError("Unknown contact_measure.");

  // ------------------------------------------------------------------
  // Tangential/frictional contribution.
  // Empty for now. This is where Paper 2 goes.
  // ------------------------------------------------------------------
  RankTwoTensor S_shear;
  S_shear.zero();

  if (_friction_model != "none")
  {
    if (_friction_model == "rate-independent")
    {
      mooseError("Friction model not implemented yet.");
    }
    else if (_friction_model == "rate-dependent")
      mooseError("Friction model not implemented yet.");
    else if (_friction_model == "rate-and-state")
      mooseError("Friction model not implemented yet.");
  }

  const RankTwoTensor S = S_iso + S_aniso + S_shear;

  return (F * S * F.transpose()) / J;
}

RankFourTensor
ContactMaterial::computeNumericalTangent(const RankTwoTensor & F) const
{
  RankFourTensor Cnum;
  Cnum.zero();

  const Real eps = _tangent_perturbation;

  for (unsigned int k = 0; k < _dim; ++k)
    for (unsigned int l = 0; l < _dim; ++l)
    {
      RankTwoTensor d;
      d.zero();

      d(k, l) += 0.5 * eps;
      d(l, k) += 0.5 * eps;

      RankTwoTensor I;
      I.zero();
      I.addIa(1.0);

      const RankTwoTensor Fp = (I + d) * F;
      const RankTwoTensor Fm = (I - d) * F;

      const RankTwoTensor sig_p = computeCauchyStress(Fp);
      const RankTwoTensor sig_m = computeCauchyStress(Fm);

      const RankTwoTensor dsig = (sig_p - sig_m) / (2.0 * eps);

      for (unsigned int i = 0; i < _dim; ++i)
        for (unsigned int j = 0; j < _dim; ++j)
          Cnum(i, j, k, l) = dsig(i, j);
    }

  return Cnum;
}

void
ContactMaterial::computeQpProperties()
{
  const RankTwoTensor F = computeDeformationGradient();
  const RankTwoTensor C = F.transpose() * F;

  RankTwoTensor I;
  I.zero();
  I.addIa(1.0);

  _strain[_qp] = 0.5 * (C - I);

  const Point N0 = computeNormal();

  Point CN;
  CN.zero();

  for (unsigned int i = 0; i < _dim; ++i)
    for (unsigned int j = 0; j < _dim; ++j)
      CN(i) += C(i, j) * N0(j);

  const Real J4_raw = N0 * CN;
  const Real J4 = std::max(J4_raw, 1e-16);
  const Real lambda_n = std::sqrt(J4);

  const Real h0 = computeElementHeight(N0);
  const Real h = h0 * lambda_n;
  const Real closure = h0 - h;

  _J4[_qp] = J4;
  _lambda_n[_qp] = lambda_n;
  _h_current[_qp] = h;
  _closure[_qp] = closure;
  _closure_gap[_qp] = closure;
  _h0[_qp] = h0;
  _h0_current[_qp] = h0;

  _stress[_qp] = computeCauchyStress(F);
  _Jacobian_mult[_qp] = computeNumericalTangent(F);

  _biot_coefficient[_qp] = 0.0;
  _mixture_gravity_term[_qp] = RealVectorValue(0.0, 0.0, 0.0);
}