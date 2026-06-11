/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*     REDBACK - Rock mEchanics with Dissipative feedBACKs      */
/*                                                              */
/*              (c) 2014 CSIRO and UNSW Australia               */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*            Prepared by CSIRO and UNSW Australia              */
/*            But now with the big boys of TU Delft             */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "RedbackMechMaterialContact.h"
#include "libmesh/elem.h"
#include "libmesh/remote_elem.h"
#include <cmath>
#include <algorithm>
#include <set>
#include <vector>

registerMooseObject("RedbackApp", RedbackMechMaterialContact);

InputParameters
RedbackMechMaterialContact::validParams()
{
  InputParameters params = RedbackMechMaterial::validParams();

  MooseEnum h_method("h_rec hmin h_circle", "h_rec");
  params.addParam<MooseEnum>("h_method", h_method, "Characteristic element height.");

  MooseEnum friction_model("none coulomb_static coulomb_dynamic", "none");
  params.addParam<MooseEnum>("friction_model", friction_model, "Friction model.");

  MooseEnum reference_height_method("self neighbors_avg neighbors_min neighbors_harmonic", "neighbors_avg");
  params.addParam<MooseEnum>("reference_height_method", reference_height_method,
                             "Reference height method.");

  params.addRequiredParam<Real>("youngs_modulus_contact", "Contact stiffness scale.");

  params.addParam<Real>("alpha_n", 1.0, "");
  params.addParam<Real>("alpha_t", 1.0, "");
  params.addParam<Real>("gap_exponent", 1.0, "");
  params.addParam<Real>("isotropic_stiffness_ratio", 1e-8, "");
  params.addParam<Real>("min_gap", 1e-12, "");
  params.addParam<Real>("friction_coefficient_static", "");
  params.addParam<Real>("friction_coefficient_dynamic", "");
  params.addParam<Real>("friction_velocity_reference", 1e-6, "");
  params.addParam<Real>("stabilization_poisson_ratio", 0.0, "");

  return params;
}

RedbackMechMaterialContact::RedbackMechMaterialContact(const InputParameters & parameters)
  : RedbackMechMaterial(parameters),

    _h_method(getParam<MooseEnum>("h_method")),
    _friction_model(getParam<MooseEnum>("friction_model")),
    _reference_height_method(getParam<MooseEnum>("reference_height_method")),

    _youngs_modulus_contact(getParam<Real>("youngs_modulus_contact")),
    _alpha_n(getParam<Real>("alpha_n")),
    _alpha_t(getParam<Real>("alpha_t")),
    _gap_exponent(getParam<Real>("gap_exponent")),
    _isotropic_stiffness_ratio(getParam<Real>("isotropic_stiffness_ratio")),
    _stabilization_poisson_ratio(getParam<Real>("stabilization_poisson_ratio")),
    _min_gap(getParam<Real>("min_gap")),

    _friction_coefficient_static(isParamValid("friction_coefficient_static")
                                  ? getParam<Real>("friction_coefficient_static")
                                  : 0.0),
    _friction_coefficient_dynamic(isParamValid("friction_coefficient_dynamic")
                                   ? getParam<Real>("friction_coefficient_dynamic")
                                   : 0.0),
    _friction_velocity_reference(getParam<Real>("friction_velocity_reference")),

    _contact_h0(declareProperty<Real>("contact_h0")),
    _contact_gap(declareProperty<Real>("contact_gap")),
    _contact_reference_height(declareProperty<Real>("contact_reference_height")),
    _contact_current_height(declareProperty<Real>("contact_current_height")),
    _contact_active(declareProperty<Real>("contact_active")),
    _contact_normal_strain(declareProperty<Real>("contact_normal_strain")),
    _contact_normal_stress(declareProperty<Real>("contact_normal_stress")),
    _contact_tangential_strain(declareProperty<Real>("contact_tangential_strain")),
    _contact_tangential_strain_old(getMaterialPropertyOld<Real>("contact_tangential_strain")),
    _contact_tangential_stress(declareProperty<Real>("contact_tangential_stress")),
    _contact_sliding(declareProperty<Real>("contact_sliding")),
    _contact_tangential_slip_rate(declareProperty<Real>("contact_tangential_slip_rate")),
    _contact_friction_coefficient(declareProperty<Real>("contact_friction_coefficient")),

    _contact_normal(declareProperty<RealVectorValue>("contact_normal")),
    _contact_tangent(declareProperty<RealVectorValue>("contact_tangent")),

    _contact_kn(declareProperty<Real>("contact_kn")),
    _contact_kt(declareProperty<Real>("contact_kt")),
    _contact_height_ratio(declareProperty<Real>("contact_height_ratio"))
{
}

void
RedbackMechMaterialContact::initQpStatefulProperties()
{
  RedbackMechMaterial::initQpStatefulProperties();
  _contact_kn[_qp] = 0.0;
  _contact_kt[_qp] = 0.0;
  _contact_height_ratio[_qp] = 1.0;
}

//
// Helper functions
//

Real
RedbackMechMaterialContact::computeCharacteristicHeight(const Elem & elem) const
// Compute a characteristic element size from geometry.
{
  if (_h_method == "hmin")
    return elem.hmin();

  const Real meas = elem.volume();
  const unsigned int dim = elem.dim();

  if (_h_method == "h_rec")
  {
    if (dim == 1)
      return meas;
    else if (dim == 2)
      return std::sqrt(meas);
    else
      return std::cbrt(meas);
  }

  if (_h_method == "h_circle")
  {
    if (dim == 2)
      return std::sqrt(meas / M_PI);
    else
      mooseError("h_circle only valid for 2D elements.");
  }
  mooseError("Unsupported h_method");
}

Real
RedbackMechMaterialContact::computeReferenceHeight(const Elem & elem) const
// Compute the reference height h_ref used in the stiffness ratio:
// r = h_ref / h_cur
{
  if (_reference_height_method == "self")
    return computeCharacteristicHeight(elem);

  std::vector<Real> neighbor_heights;
  std::set<dof_id_type> visited_ids;

  for (unsigned int s = 0; s < elem.n_sides(); ++s)
  {
    const Elem * neighbor = elem.neighbor_ptr(s);

    if (!neighbor)
      continue;

    if (neighbor == libMesh::remote_elem)
      continue;

    if (neighbor->subdomain_id() == elem.subdomain_id())
      continue;

    if (!visited_ids.insert(neighbor->id()).second)
      continue;

    const Real h_nb = computeCharacteristicHeight(*neighbor);
    if (h_nb > 0.0)
      neighbor_heights.push_back(h_nb);
  }

  if (neighbor_heights.empty())
    return computeCharacteristicHeight(elem);

  if (_reference_height_method == "neighbors_min")
    return *std::min_element(neighbor_heights.begin(), neighbor_heights.end());

  if (_reference_height_method == "neighbors_harmonic")
  {
    Real inv_sum = 0.0;
    for (const Real h : neighbor_heights)
      inv_sum += 1.0 / h;

    return (inv_sum > 0.0) ? (static_cast<Real>(neighbor_heights.size()) / inv_sum)
                           : computeCharacteristicHeight(elem);
  }

  // default: arithmetic average of neighboring bulk heights
  Real sum = 0.0;
  for (const Real h : neighbor_heights)
    sum += h;

  return sum / static_cast<Real>(neighbor_heights.size());
}

Real
RedbackMechMaterialContact::computeContactReferenceHeight(const Elem & elem) const
// Compute the undeformed contact element height h_contact,0.
{
  return computeCharacteristicHeight(elem);
}

Real
RedbackMechMaterialContact::computeCurrentHeight(const Real h_contact_0,
                                                 const Real eps_n) const
// Compute the current contact element height:
// h_cur = max(h_min, h_contact,0 * (1 + eps_n))
{
  return std::max(_min_gap, h_contact_0 * (1.0 + eps_n));
}

void
RedbackMechMaterialContact::computeElementBasis(const Elem & elem,
                                                RealVectorValue & normal,
                                                RealVectorValue & tangent) const
// Compute tangent and normal directions from element geometry.
{
  const unsigned int n_nodes = elem.n_nodes();
  if (n_nodes < 2)
    mooseError("Element must have at least 2 nodes.");

  const Point & p0 = elem.point(0);
  const Point & p1 = elem.point(1);

  RealVectorValue e1(p1(0) - p0(0), p1(1) - p0(1), p1(2) - p0(2));
  const Real e1_norm = e1.norm();
  if (e1_norm < 1e-16)
    mooseError("Degenerate element: first edge has near-zero length.");

  tangent = e1 / e1_norm;

  // In 2D:
  //   t = (x1 - x0) / ||x1 - x0||
  //   n = [-t_y, t_x, 0]
  if (elem.dim() == 2)
  {
    normal = RealVectorValue(-tangent(1), tangent(0), 0.0);
    const Real n_norm = normal.norm();
    if (n_norm < 1e-16)
      mooseError("Failed to compute 2D contact normal.");
    normal /= n_norm;
  }

  // In 3D:
  //   t = (x1 - x0) / ||x1 - x0||
  //   n = ((x1 - x0) x (x2 - x0)) / ||(x1 - x0) x (x2 - x0)||
  else if (elem.dim() == 3)
  {
    if (n_nodes < 3)
      mooseError("3D element must have at least 3 nodes.");

    const Point & p2 = elem.point(2);
    RealVectorValue e2(p2(0) - p0(0), p2(1) - p0(1), p2(2) - p0(2));

    normal = e1.cross(e2);
    const Real n_norm = normal.norm();
    if (n_norm < 1e-16)
      mooseError("Failed to compute 3D contact normal.");

    normal /= n_norm;
  }
  else
    mooseError("Only dim=2 and dim=3 supported.");
}

Real
RedbackMechMaterialContact::computeNormalStrain(const RankTwoTensor & eps,
                                                const RealVectorValue & normal) const
// Compute projected normal strain:
//   eps_n = n · eps · n
{
  return normal * (eps * normal);
}

Real
RedbackMechMaterialContact::computeTangentialStrain(const RankTwoTensor & eps,
                                                    const RealVectorValue & normal,
                                                    const RealVectorValue & tangent) const
// Compute projected tangential engineering shear strain:
//   gamma_t = 2 * (t · eps · n)
{
  return 2.0 * (tangent * (eps * normal));
}

Real
RedbackMechMaterialContact::computeTangentialSlipRate(const Real h0,
                                                      const Real gamma_t,
                                                      const Real gamma_t_old) const
// Compute tangential slip-rate proxy:
//   v_t = h0 * (gamma_t - gamma_t_old) / dt
{
  if (_dt <= 0.0)
    return 0.0;

  return h0 * (gamma_t - gamma_t_old) / _dt;
}

Real
RedbackMechMaterialContact::computeFrictionCoefficient(const Real vt) const
/* Compute effective dynamic friction coefficient, with smooth transition from static to dynamic:
   mu(v_t) = mu_d + (mu_s - mu_d) * exp(-|v_t| / v_ref)
*/
{
  const Real vabs = std::abs(vt);
  const Real vref = std::max(_friction_velocity_reference, 1e-16);

  return _friction_coefficient_dynamic +
         (_friction_coefficient_static - _friction_coefficient_dynamic) *
             std::exp(-vabs / vref);
}


//
// Material implementations
//

void
RedbackMechMaterialContact::computeQpElasticityTensor()
{

  _elasticity_tensor[_qp].zero();
  _Jacobian_mult[_qp].zero();

  // eps = eps_old + delta_eps
  RankTwoTensor eps = _total_strain_old[_qp] + _strain_increment[_qp];

  // tangent + normal
  RealVectorValue normal, tangent;
  computeElementBasis(*_current_elem, normal, tangent);

  // h_cur = current height = h0 * (1 + eps_n)
  const Real h_ref = computeReferenceHeight(*_current_elem);
  const Real h0    = computeCharacteristicHeight(*_current_elem);
  const Real eps_n = computeNormalStrain(eps, normal);
  const Real h_cur = computeCurrentHeight(h0, eps_n);

  // ratio = h_ref / h_cur
  const Real ratio = h_ref / h_cur;
  _contact_height_ratio[_qp] = ratio;

  // STRAINS and slip rate
  const Real gamma_t = computeTangentialStrain(eps, normal, tangent);
  const Real gamma_t_old = _contact_tangential_strain_old[_qp];
  const Real vt = computeTangentialSlipRate(h0, gamma_t, gamma_t_old);

  _contact_kn[_qp] = 0.0;
  _contact_kt[_qp] = 0.0;

  // eps_n < 0
  if (eps_n < 0.0)
  {
    // k_n = alpha_n * E_c * (h_ref / h_cur)^p
    // sigma_n = k_n * eps_n
    const Real kn = _alpha_n * _youngs_modulus_contact * std::pow(ratio, _gap_exponent);
    _contact_kn[_qp] = kn;

    // C_ijkl += k_n * n_i n_j n_k n_l
    for (unsigned int i=0;i<3;i++)
      for (unsigned int j=0;j<3;j++)
        for (unsigned int k=0;k<3;k++)
          for (unsigned int l=0;l<3;l++)
          {
            const Real val = kn * normal(i)*normal(j)*normal(k)*normal(l);
            _elasticity_tensor[_qp](i,j,k,l) += val;
            _Jacobian_mult[_qp](i,j,k,l) += val;
          }

    // FRICTION 
    if (_friction_model != "none")
    {
      // k_t = alpha_t * E_c * (h_ref / h_cur)^p
      const Real kt = _alpha_t * _youngs_modulus_contact * std::pow(ratio, _gap_exponent);
      _contact_kt[_qp] = kt;
      
      // tau_trial = k_t * gamma_t
      // sigma_n = k_n * eps_n
      const Real tau_trial = kt * gamma_t;
      const Real sigma_n = kn * eps_n;

      // Friction coefficient
      Real mu = 0.0;

      if (_friction_model == "coulomb_static")
        mu = _friction_coefficient_static;

      else if (_friction_model == "coulomb_dynamic")
        mu = computeFrictionCoefficient(vt);

      // tau_max = mu * |sigma_n|
      // |tau_trial| <= tau_limit
      const Real tau_limit = mu * std::abs(sigma_n);
      if (std::abs(tau_trial) <= tau_limit)
      {
        // symmetric (t ⊗ n)
        RankTwoTensor sym_tn;
        sym_tn.zero();

        for (unsigned int i=0;i<3;i++)
          for (unsigned int j=0;j<3;j++)
            sym_tn(i,j) = 0.5 * (tangent(i)*normal(j) + normal(i)*tangent(j));

        // C_ijkl += k_t * (sym_tn_ij * sym_tn_kl)
        for (unsigned int i=0;i<3;i++)
          for (unsigned int j=0;j<3;j++)
            for (unsigned int k=0;k<3;k++)
              for (unsigned int l=0;l<3;l++)
              {
                const Real val = kt * sym_tn(i,j)*sym_tn(k,l);
                _elasticity_tensor[_qp](i,j,k,l) += val;
                _Jacobian_mult[_qp](i,j,k,l) += val;
              }
      }
    }
  }
}


void
RedbackMechMaterialContact::returnMap(const RankTwoTensor & /*sig_old*/,
                                      const RankTwoTensor & delta_d,
                                      const RankFourTensor & /*E_ijkl*/,
                                      RankTwoTensor & dp,
                                      RankTwoTensor & sig,
                                      Real & p_y,
                                      Real & q_y)
{
  dp = _plastic_strain_old[_qp];

  RankTwoTensor eps = _total_strain_old[_qp] + delta_d;

  RealVectorValue normal, tangent;
  computeElementBasis(*_current_elem, normal, tangent);

  const Real h_ref = computeReferenceHeight(*_current_elem);
  const Real h_contact_0 = computeContactReferenceHeight(*_current_elem);
  const Real eps_n = computeNormalStrain(eps, normal);
  const Real h_cur = computeCurrentHeight(h_contact_0, eps_n);
  const Real gamma_t = computeTangentialStrain(eps, normal, tangent);
  const Real gamma_t_old = _contact_tangential_strain_old[_qp];
  const Real vt = computeTangentialSlipRate(h_contact_0, gamma_t, gamma_t_old);

  _contact_h0[_qp] = h_contact_0;
  _contact_gap[_qp] = h_cur;
  _contact_reference_height[_qp] = h_ref;
  _contact_current_height[_qp] = h_cur;
  _contact_normal[_qp] = normal;
  _contact_tangent[_qp] = tangent;
  _contact_normal_strain[_qp] = eps_n;
  _contact_tangential_strain[_qp] = gamma_t;
  _contact_tangential_slip_rate[_qp] = vt;

  RankTwoTensor n_outer_n;
  n_outer_n.zero();
  for (unsigned int i = 0; i < 3; ++i)
    for (unsigned int j = 0; j < 3; ++j)
      n_outer_n(i, j) = normal(i) * normal(j);

  RankTwoTensor sym_tn;
  sym_tn.zero();
  for (unsigned int i = 0; i < 3; ++i)
    for (unsigned int j = 0; j < 3; ++j)
      sym_tn(i, j) = 0.5 * (tangent(i) * normal(j) + normal(i) * tangent(j));

  sig.zero();

  const Real E_small = _isotropic_stiffness_ratio * _youngs_modulus_contact;
  const Real nu_small = _stabilization_poisson_ratio;
  const Real lambda_s = (nu_small * E_small) / ((1.0 + nu_small) * (1.0 - 2.0 * nu_small));
  const Real mu_s = E_small / (2.0 * (1.0 + nu_small));

  sig.addIa(lambda_s * eps.trace());
  sig += 2.0 * mu_s * eps;

  _contact_normal_stress[_qp] = 0.0;
  _contact_tangential_stress[_qp] = 0.0;
  _contact_active[_qp] = 0.0;
  _contact_sliding[_qp] = 0.0;
  _contact_friction_coefficient[_qp] = 0.0;

  if (eps_n < 0.0)
  {
    const Real kn_factor =
        _alpha_n * _youngs_modulus_contact * std::pow(h_ref / h_cur, _gap_exponent);
    const Real sigma_n = kn_factor * eps_n;

    sig += sigma_n * n_outer_n;

    _contact_normal_stress[_qp] = sigma_n;
    _contact_active[_qp] = 1.0;

    if (_friction_model != "none")
    {
      const Real kt_factor =
          _alpha_t * _youngs_modulus_contact * std::pow(h_ref / h_cur, _gap_exponent);

      const Real tau_trial = kt_factor * gamma_t;

      Real mu_eff = 0.0;
      if (_friction_model == "coulomb_static")
        mu_eff = _friction_coefficient_static;
      else if (_friction_model == "coulomb_dynamic")
        mu_eff = computeFrictionCoefficient(vt);

      _contact_friction_coefficient[_qp] = mu_eff;

      const Real tau_limit = mu_eff * std::abs(sigma_n);

      Real tau_t = tau_trial;
      if (std::abs(tau_trial) > tau_limit)
      {
        tau_t = (tau_trial >= 0.0 ? 1.0 : -1.0) * tau_limit;
        _contact_sliding[_qp] = 1.0;
      }

      sig += tau_t * sym_tn;
      _contact_tangential_stress[_qp] = tau_t;
    }
  }

  const Real ratio = h_ref / h_cur;
  _contact_height_ratio[_qp] = ratio;

  p_y = 0.0;
  q_y = 0.0;
}

void
RedbackMechMaterialContact::getFlowTensor(const RankTwoTensor & /*sig*/,
                                          Real /*q*/,
                                          Real /*p*/,
                                          Real /*q_y*/,
                                          Real /*p_y*/,
                                          Real /*yield_stress*/,
                                          RankTwoTensor & flow_tensor_dev)
{
  flow_tensor_dev.zero();
}

Real
RedbackMechMaterialContact::getFlowIncrement(Real /*sig_eqv*/,
                                             Real /*p*/,
                                             Real /*q_y*/,
                                             Real /*p_y*/,
                                             Real /*yield_stress*/)
{
  return 0.0;
}

void
RedbackMechMaterialContact::getJac(const RankTwoTensor & /*sig*/,
                                   const RankFourTensor & /*E_ijkl*/,
                                   Real /*flow_incr_dev*/,
                                   Real /*q*/,
                                   Real /*p*/,
                                   Real /*p_y*/,
                                   Real /*q_y*/,
                                   Real /*yield_stress*/,
                                   RankFourTensor & /*dresid_dsig*/)
{
}

void
RedbackMechMaterialContact::get_py_qy(Real p,
                                      Real q,
                                      Real & p_y,
                                      Real & q_y,
                                      Real /*yield_stress*/,
                                      bool & is_plastic)
{
  is_plastic = false;
  p_y = p;
  q_y = q;
}