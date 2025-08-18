/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*     REDBACK - Rock mEchanics with Dissipative feedBACKs      */
/*                                                              */
/*              (c) 2014 CSIRO and UNSW Australia               */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*            Prepared by CSIRO and UNSW Australia              */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "Function.h"
#include "MooseException.h"
#include "MooseMesh.h"
#include "RedbackMechMaterial.h"
#include "libmesh/quadrature.h"

/**
RedbackMechMaterial integrates the rate dependent plasticity model of Perzyna
(Overstress model) in a
finite strain framework using return mapping algorithm. Ideally this matrial
should inherit from both
the Redback material and the tensor mechanics finite strain equivalent

Three yield criteria are included:
1. pressure-independent (J2 plasticity)
2. linear pressure-dependent (Drucker-Prager with smoothing of the apex)
3. Non-linear, capped pressure dependent (modified cam clay)

Integration is performed for associative flow rules in an incremental manner
using Newton Raphson.
Isotropic hardening/softening has been incorporated where yield stress as a
function of equivalent
plastic strain has to be specified by the user.
*/

InputParameters
RedbackMechMaterial::validParams()
{
  InputParameters params = Material::validParams();

  // Copy-paste from TensorMechanicsMaterial.C
  params.addRequiredCoupledVar("disp_x", "The x displacement");
  params.addRequiredCoupledVar("disp_y", "The y displacement");
  params.addCoupledVar("disp_z", 0.0, "The z displacement");
  params.addCoupledVar("temperature", 0.0, "temperature variable");
  params.addCoupledVar("damage", 0.0, "damage variable");

  // Copy-paste from FiniteStrainMaterial.C
  // nothing

  // Copy-paste from FiniteStrainPlasticMaterial.C
  // params.addRequiredParam< std::vector<Real> >("yield_stress", "Input data as
  // pairs of equivalent plastic strain and yield stress: Should start with
  // equivalent plastic strain 0");
  params.addParam<std::vector<Real>>("yield_stress",{},
                                     "Input data as pairs of equivalent "
                                     "plastic strain and yield stress: Should "
                                     "start with equivalent plastic strain 0");
  params.addParam<Real>("rtol", 1e-8, "Plastic strain NR tolerance");
  params.addParam<Real>("ftol", 1e-4, "Consistency condition NR tolerance");
  params.addParam<Real>("eptol", 1e-7, "Equivalent plastic strain NR tolerance");
  params.addClassDescription("Associative overstress plasticity");

  //  Copy-paste from FiniteStrainPlasticRateMaterial.C
  params.addParam<Real>("ref_pe_rate",
                        1.0,
                        "Reference plastic strain rate "
                        "parameter for rate dependent "
                        "plasticity (Overstress model)");
  params.addParam<Real>("exponent", 1.0, "Exponent for rate dependent plasticity (Perzyna)");
  params.addParam<Real>("chemo_mechanical_porosity_coeff",
                        1.0,
                        "The coefficient of volumetric plastic strain in chemical porosity");

  params.addCoupledVar("pore_pres", 0.0, "Dimensionless pore pressure");
  params.addRequiredParam<Real>("youngs_modulus", "Youngs modulus.");
  params.addRequiredParam<Real>("poisson_ratio", "Poisson ratio.");

  // For the damage mechanics functionality
  params.addParam<Real>("damage_coefficient",
                        0.0,
                        "The fraction of energies used in damage flow law (e.g. E_D/E_D0)");
  params.addParam<Real>(
      "damage_exponent", 0.0, "The damage exponent in the incremental flow law of plasticity");
  params.addParam<Real>("healing_coefficient",
                        0.0,
                        "The fraction of energies used in healing flow law (e.g. E_H/E_H0)");
  params.addParam<MooseEnum>("damage_method",
                             RedbackMechMaterial::damageMethodEnum() = "CreepDamage",
                             "The method to describe damage evolution");

  params.addCoupledVar("total_porosity", 0.0, "The total porosity (as AuxKernel)");
  params.addParam<Real>(
      "temperature_reference", 0.0, "Reference temperature used for thermal expansion");
  params.addParam<Real>("pressure_reference", 0.0, "Reference pressure used for compressibility");
  params.addParam<std::vector<FunctionName>>(
      "initial_stress",{},
      "A list of functions describing the initial stress. If provided, there "
      "must be 9 of these, corresponding to the xx, yx, zx, xy, yy, zy, xz, yz, "
      "zz components respectively.  If not provided, all components of the "
      "initial stress will be zero");

  return params;
}

RedbackMechMaterial::RedbackMechMaterial(const InputParameters & parameters)
  : Material(parameters),
    // Copy-paste from TensorMechanicsMaterial.C
    _grad_disp_x(coupledGradient("disp_x")),
    _grad_disp_y(coupledGradient("disp_y")),
    _grad_disp_z(_mesh.dimension() == 3 ? coupledGradient("disp_z") : _grad_zero),
    _grad_disp_x_old(_fe_problem.isTransient() ? coupledGradientOld("disp_x") : _grad_zero),
    _grad_disp_y_old(_fe_problem.isTransient() ? coupledGradientOld("disp_y") : _grad_zero),
    _grad_disp_z_old(_fe_problem.isTransient() && _mesh.dimension() == 3
                         ? coupledGradientOld("disp_z")
                         : _grad_zero),
    _stress(declareProperty<RankTwoTensor>("stress")),
    _total_strain(declareProperty<RankTwoTensor>("total_strain")),
    _elastic_strain(declareProperty<RankTwoTensor>("elastic_strain")),
    _elasticity_tensor(declareProperty<RankFourTensor>("elasticity_tensor")),
    _Jacobian_mult(declareProperty<RankFourTensor>("Jacobian_mult")),
    // _d_stress_dT(declareProperty<RankTwoTensor>("d_stress_dT")),
    _Cijkl(),

    // Copy-paste from FiniteStrainMaterial.C
    _strain_rate(declareProperty<RankTwoTensor>("strain_rate")),
    _strain_increment(declareProperty<RankTwoTensor>("strain_increment")),
    _total_strain_old(getMaterialPropertyOld<RankTwoTensor>("total_strain")),
    _elastic_strain_old(getMaterialPropertyOld<RankTwoTensor>("elastic_strain")),
    _stress_old(getMaterialPropertyOld<RankTwoTensor>("stress")),
    _rotation_increment(declareProperty<RankTwoTensor>("rotation_increment")),
    _dfgrd(declareProperty<RankTwoTensor>("deformation gradient")),

    // Copy-paste from FiniteStrainPlasticMaterial.C
    _yield_stress_vector(getParam<std::vector<Real>>("yield_stress")), // Read from input file
    _plastic_strain(declareProperty<RankTwoTensor>("plastic_strain")),
    _plastic_strain_old(getMaterialPropertyOld<RankTwoTensor>("plastic_strain")),
    _eqv_plastic_strain(declareProperty<Real>("eqv_plastic_strain")),
    _eqv_plastic_strain_old(getMaterialPropertyOld<Real>("eqv_plastic_strain")),

    // Copy-paste from FiniteStrainPlasticRateMaterial.C
    _ref_pe_rate(getParam<Real>("ref_pe_rate")),
    _exponent(getParam<Real>("exponent")),
    _chemo_mechanical_porosity_coeff(getParam<Real>("chemo_mechanical_porosity_coeff")),

    // Redback
    _youngs_modulus(getParam<Real>("youngs_modulus")),
    _poisson_ratio(getParam<Real>("poisson_ratio")),
    _mises_stress(declareProperty<Real>("mises_stress")),
    _mean_stress(declareProperty<Real>("mean_stress")),
    _mises_strain_rate(declareProperty<Real>("mises_strain_rate")),
    _volumetric_strain(declareProperty<Real>("volumetric_strain")),
    _volumetric_strain_rate(declareProperty<Real>("volumetric_strain_rate")),
    _total_volumetric_strain(declareProperty<Real>("total_volumetric_strain")),
    _mechanical_porosity(declareProperty<Real>("mechanical_porosity")),
    _mass_removal_rate(declareProperty<Real>("mass_removal_rate")),

    _poromech_kernel(declareProperty<Real>("poromechanics_kernel")),
    _poromech_jac(declareProperty<Real>("poromechanics_jacobian")),
    _mod_gruntfest_number(declareProperty<Real>("mod_gruntfest_number")),
    _mechanical_dissipation_mech(declareProperty<Real>("mechanical_dissipation_mech")),
    _mechanical_dissipation_jac_mech(declareProperty<Real>("mechanical_dissipation_jacobian_mech")),

    _damage_kernel(declareProperty<Real>("damage_kernel")),
    _damage_kernel_jac(declareProperty<Real>("damage_kernel_jacobian")),
    _damage_coeff(getParam<Real>("damage_coefficient")),
    _dmg_exponent(getParam<Real>("damage_exponent")),
    _healing_coeff(getParam<Real>("healing_coefficient")),

    // Get coupled variables (T & P & porosity & damage)
    _has_T(isCoupled("temperature")),
    _T(_has_T ? coupledValue("temperature") : _zero),
    _T_old(_has_T ? coupledValueOld("temperature") : _zero),
    _has_pore_pres(isCoupled("pore_pres")),
    _pore_pres(_has_pore_pres ? coupledValue("pore_pres") : _zero),
    _total_porosity(coupledValue("total_porosity")), // total_porosity MUST be
                                                     // coupled! Check that
                                                     // (TODO)
    _has_D(isCoupled("damage")),
    //_damage(_has_D ? coupledValue("damage") : _zero),
    //_damage_old(_has_D ? coupledValueOld("damage") : _zero),
    _damage(coupledValue("damage")),
    _damage_old(coupledValueOld("damage")),

    _damage_method((DamageMethod)(int)getParam<MooseEnum>("damage_method")),

    // Get some material properties from RedbackMaterial
    _gr(getMaterialProperty<Real>("gr")),
    _lewis_number(getMaterialProperty<Real>("lewis_number")),
    _ar(getMaterialProperty<Real>("ar")),
    _confining_pressure(getMaterialProperty<Real>("confining_pressure")),
    _alpha_1(getMaterialProperty<Real>("alpha_1")),
    _alpha_2(getMaterialProperty<Real>("alpha_2")),
    _alpha_3(getMaterialProperty<Real>("alpha_3")),
    _delta(getMaterialProperty<Real>("delta")),
    _solid_thermal_expansion(getMaterialProperty<Real>("solid_thermal_expansion")),
    _solid_compressibility(getMaterialProperty<Real>("solid_compressibility")),
    _mixture_compressibility(getMaterialProperty<Real>("mixture_compressibility")),
    _peclet_number(getMaterialProperty<Real>("Peclet_number")),
    _returnmap_iter(declareProperty<Real>("returnmap_iter")),
    _T0_param(getParam<Real>("temperature_reference")),
    _P0_param(getParam<Real>("pressure_reference"))
{
  Real E = _youngs_modulus;
  Real nu = _poisson_ratio;
  Real l1 = E * nu / (1 + nu) / (1 - 2 * nu); // First Lame modulus
  Real l2 = 0.5 * E / (1 + nu);               // Second Lame modulus (shear)
  Real input_array[] = {l1, l2};
  std::vector<Real> input_vector(input_array, input_array + 2);

  // Choose fill method, hardcoded.
  MooseEnum fill_method = RankFourTensor::fillMethodEnum();
  fill_method = "symmetric_isotropic"; // Creates symmetric and isotropic
                                       // elasticity tensor.
  _Cijkl.fillFromInputVector(input_vector, (RankFourTensor::FillMethod)(int)fill_method);

  // Initial stress
  const std::vector<FunctionName> & fcn_names(
      getParam<std::vector<FunctionName>>("initial_stress"));
  const unsigned num = fcn_names.size();
  if (!(num == 0 || num == 3 * 3))
    mooseError(
        "Either zero or ",
        3 * 3,
        " initial stress functions must be provided to TensorMechanicsMaterial.  You supplied ",
        num,
        "\n");
  _initial_stress.resize(num);
  for (unsigned i = 0; i < num; ++i)
    _initial_stress[i] = &getFunctionByName(fcn_names[i]);

  // initialise damage dissipation
  _damage_dissipation = 0;
}

MooseEnum
RedbackMechMaterial::damageMethodEnum()
{
  return MooseEnum("BrittleDamage CreepDamage BreakageMechanics DamageHealing FromMultiApp");
}

void
RedbackMechMaterial::initQpStatefulProperties()
{
  // called only once at the very beginning of the simulation
  _total_strain[_qp].zero();
  _elastic_strain[_qp].zero();
  _stress[_qp].zero();
  if (_initial_stress.size() == 3 * 3)
    for (unsigned i = 0; i < 3; ++i)
      for (unsigned j = 0; j < 3; ++j)
        _stress[_qp](i, j) = _initial_stress[i * 3 + j]->value(_t, _q_point[_qp]);
  _plastic_strain[_qp].zero();
  _eqv_plastic_strain[_qp] = 0.0;
  _elasticity_tensor[_qp].zero();
  _Jacobian_mult[_qp].zero();
  _strain_rate[_qp].zero();
  _strain_increment[_qp].zero();
  _rotation_increment[_qp].zero();
  _dfgrd[_qp].zero();

  // Redback properties
  _mises_stress[_qp] = getSigEqv(_stress[_qp]);
  _mean_stress[_qp] = _stress[_qp].trace() / 3.0;
  _mises_strain_rate[_qp] = 0;
  _volumetric_strain[_qp] = 0;
  _volumetric_strain_rate[_qp] = 0;
  _total_volumetric_strain[_qp] = 0;
  _mechanical_porosity[_qp] = 0;
  _poromech_kernel[_qp] = 0;
  _poromech_jac[_qp] = 0;
  _mod_gruntfest_number[_qp] = 0;
  _mechanical_dissipation_mech[_qp] = 0;
  _mechanical_dissipation_jac_mech[_qp] = 0;
  _damage_kernel[_qp] = 0;
  _damage_kernel_jac[_qp] = 0;
  _mass_removal_rate[_qp] = 0;
}

void
RedbackMechMaterial::computeProperties()
{
  computeStrain();
  for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
  {
    stepInitQpProperties();
    computeQpElasticityTensor();
    computeQpStress();
  }
}

void
RedbackMechMaterial::stepInitQpProperties()
{
}

void
RedbackMechMaterial::computeQpElasticityTensor()
{
  // Fill in the matrix stiffness material property
  _elasticity_tensor[_qp] = _Cijkl * (1 - _damage[_qp]);
  _Jacobian_mult[_qp] = _Cijkl * (1 - _damage[_qp]);
}

void
RedbackMechMaterial::computeStrain()
{
  // Method from Rashid, 1993
  std::vector<RankTwoTensor> Fhat(_qrule->n_points());
  RankTwoTensor ave_Fhat;
  Real volume = 0;
  Real ave_dfgrd_det = 0.0;

  for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
  {
    // Deformation gradient
    auto A = RankTwoTensor ::initializeFromRows(
        _grad_disp_x[_qp], _grad_disp_y[_qp], _grad_disp_z[_qp]); // Deformation gradient
    auto Fbar = RankTwoTensor ::initializeFromRows(
                       _grad_disp_x_old[_qp],
                       _grad_disp_y_old[_qp],
                       _grad_disp_z_old[_qp]); // Old Deformation gradient

    _dfgrd[_qp] = A;
    _dfgrd[_qp].addIa(1.0); // Gauss point deformation gradient

    A -= Fbar; // A = gradU - gradUold

    Fbar.addIa(1.0); // Fbar = ( I + gradUold)

    // Incremental deformation gradient Fhat = I + A Fbar^-1
    Fhat[_qp] = A * Fbar.inverse();
    Fhat[_qp].addIa(1.0);

    // Calculate average Fhat for volumetric locking correction
    ave_Fhat += Fhat[_qp] * _JxW[_qp];
    volume += _JxW[_qp];

    ave_dfgrd_det += _dfgrd[_qp].det() * _JxW[_qp]; // Average deformation gradient
  }

  ave_Fhat /= volume;      // This is needed for volumetric locking correction
  ave_dfgrd_det /= volume; // Average deformation gradient

  for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
  {
    Real factor(std::pow(ave_Fhat.det() / Fhat[_qp].det(), 1.0 / 3.0));
    Fhat[_qp] *= factor; // Finalize volumetric locking correction

    computeQpStrain(Fhat[_qp]);

    factor =
        std::pow(ave_dfgrd_det / _dfgrd[_qp].det(), 1.0 / 3.0); // Volumetric locking correction
    _dfgrd[_qp] *= factor;                                      // Volumetric locking correction
  }
}

void
RedbackMechMaterial::computeQpStrain()
{
  mooseError("Wrong computeQpStrain called in FiniteStrainMaterial");
}

void
RedbackMechMaterial::computeQpStress()
{
  // Obtain previous plastic rate of deformation tensor
  RankTwoTensor dp = _plastic_strain_old[_qp];

  // Solve J2 plastic constitutive equations based on current strain increment
  // Returns current  stress and plastic rate of deformation tensor
  _returnmap_iter[_qp] = 0;
  RankTwoTensor sig;
  Real p_y = 0, q_y = 0; // volumetric (p) and deviatoric (q) projections of yield stress
  returnMap(_stress_old[_qp], _strain_increment[_qp], _elasticity_tensor[_qp], dp, sig, p_y, q_y);
  _stress[_qp] = sig;

  // Rotate the stress to the current configuration
  _stress[_qp] = _rotation_increment[_qp] * _stress[_qp] * _rotation_increment[_qp].transpose();

  // Updates current plastic rate of deformation tensor
  _plastic_strain[_qp] = dp;

  // Evaluate and update current equivalent and volumetric plastic strain
  _eqv_plastic_strain[_qp] = std::pow(2.0 / 3.0, 0.5) * dp.L2norm();
  _volumetric_strain[_qp] = dp.trace();

  // Calculate elastic strain increment
  RankTwoTensor total_strain_increment;
  total_strain_increment = _strain_increment[_qp];
  total_strain_increment.addIa(_solid_thermal_expansion[_qp] * (_T[_qp] - _T_old[_qp]));
  RankTwoTensor delta_ee =
      _strain_increment[_qp] - (_plastic_strain[_qp] - _plastic_strain_old[_qp]);

  // Update elastic strain tensor in intermediate configuration
  _elastic_strain[_qp] = _elastic_strain_old[_qp] + delta_ee;

  // Rotate elastic strain tensor to the current configuration
  _elastic_strain[_qp] =
      _rotation_increment[_qp] * _elastic_strain[_qp] * _rotation_increment[_qp].transpose();

  // Rotate to plastic rate of deformation tensor the current configuration
  _plastic_strain[_qp] =
      _rotation_increment[_qp] * _plastic_strain[_qp] * _rotation_increment[_qp].transpose();

  // Update strain in intermediate configuration
  _total_strain[_qp] = _total_strain_old[_qp] + total_strain_increment;
  /*RankTwoTensor grad_tensor(_grad_disp_x[_qp], _grad_disp_y[_qp],
  _grad_disp_z[_qp]);
  RankTwoTensor total_strain_small_deformation = ( grad_tensor +
  grad_tensor.transpose() )/2.0;*/

  // Rotate strain to current configuration
  _total_strain[_qp] =
      _rotation_increment[_qp] * _total_strain[_qp] * _rotation_increment[_qp].transpose();
  _total_volumetric_strain[_qp] = _total_strain[_qp].trace();

  // Compute the energy dissipation and the properties declared
  computeRedbackTerms(sig, q_y, p_y);
}

// Delta Function
Real
RedbackMechMaterial::deltaFunc(const unsigned int i, const unsigned int j)
{
  if (i == j)
    return 1.0;
  else
    return 0.0;
}

// Obtain yield stress for a given equivalent plastic strain (input)
Real
RedbackMechMaterial::getYieldStress(const Real eqpe)
{
  unsigned int nsize = _yield_stress_vector.size();

  if (_yield_stress_vector[0] > 0.0 || nsize % 2 > 0) // Error check for input inconsitency
    mooseError("Error in yield stress input: Should be a vector with eqv "
               "plastic strain and yield stress pair values.\n");

  unsigned int ind = 0;
  Real tol = 1e-8;

  while (ind < nsize)
  {
    if (std::abs(eqpe - _yield_stress_vector[ind]) < tol)
      return _yield_stress_vector[ind + 1];

    if (ind + 2 < nsize)
    {
      if (eqpe > _yield_stress_vector[ind] && eqpe < _yield_stress_vector[ind + 2])
        return _yield_stress_vector[ind + 1] +
               (eqpe - _yield_stress_vector[ind]) /
                   (_yield_stress_vector[ind + 2] - _yield_stress_vector[ind]) *
                   (_yield_stress_vector[ind + 3] - _yield_stress_vector[ind + 1]);
    }
    else
      return _yield_stress_vector[nsize - 1];

    ind += 2;
  }

  return 0.0;
}

Real
RedbackMechMaterial::macaulayBracket(Real val)
{
  if (val > 0.0)
    return val;
  else
    return 0.0;
}

void
RedbackMechMaterial::computeRedbackTerms(RankTwoTensor & sig, Real q_y, Real p_y)
{
  // update velocities
  //_solid_velocity[_qp] = RealVectorValue(_dispx_dot[_qp], _dispy_dot[_qp],
  //_dispz_dot[_qp]);// TODO

  // Material::computeRedbackTerms();

  // Compute stresses
  _mises_stress[_qp] = getSigEqv(sig);
  _mean_stress[_qp] = sig.trace() / 3.0;

  // Compute plastic strains
  RankTwoTensor instantaneous_strain_rate, total_volumetric_strain_rate;

  if (_dt == 0)
  {
    instantaneous_strain_rate.zero();
    total_volumetric_strain_rate.zero();
  }
  else
  {
    instantaneous_strain_rate = (_plastic_strain[_qp] - _plastic_strain_old[_qp]) / _dt;
    total_volumetric_strain_rate = (_total_strain[_qp] - _total_strain_old[_qp]) / _dt;
  }
  _mises_strain_rate[_qp] = std::pow(2.0 / 3.0, 0.5) * instantaneous_strain_rate.L2norm();
  _volumetric_strain_rate[_qp] = total_volumetric_strain_rate.trace();
  Real def_grad = _grad_disp_x[_qp](0) + _grad_disp_y[_qp](1) + _grad_disp_z[_qp](2);
  Real def_grad_old =
      _grad_disp_x_old[_qp](0) + _grad_disp_y_old[_qp](1) + _grad_disp_z_old[_qp](2);
  Real def_grad_rate = 0.;
  if (_dt > 0)
    def_grad_rate = (def_grad - def_grad_old) / _dt;

  // Update mechanical porosity (elastic and plastic components)
  // TODO: set T0 properly (once only, at the very beginning). Until then, T = T
  // - T0, P = P - P0
  Real delta_phi_mech_el =
      (1.0 - _total_porosity[_qp]) * (_solid_compressibility[_qp] * (_pore_pres[_qp] - _P0_param) -
                                      _solid_thermal_expansion[_qp] * (_T[_qp] - _T0_param) +
                                      (_elastic_strain[_qp] - _elastic_strain_old[_qp]).trace());
  Real delta_phi_mech_pl =
      (1.0 - _total_porosity[_qp]) * (_plastic_strain[_qp] - _plastic_strain_old[_qp]).trace();

  _mechanical_porosity[_qp] = delta_phi_mech_el + delta_phi_mech_pl;

  if (_has_D)
  {
    _mechanical_porosity[_qp] += std::pow(_damage[_qp], 3 / 2);

    switch (_damage_method)
    {
      case BrittleDamage:
        formDamageDissipation(sig);
        break;
      case CreepDamage:
        formDamageDissipation(sig);
        break;
      case BreakageMechanics:
        formDamageDissipation(sig);
        break;
      case DamageHealing:
        formDamageDissipation(sig);
        break;
      default:
        mooseError("damage method not implemented yet, use other options");
    }

    form_damage_kernels(q_y);
  }

  Real gruntfest_number = _gr[_qp] * std::exp(_ar[_qp]);

  // Compute Mechanical Dissipation.
  _mechanical_dissipation_mech[_qp] =
      gruntfest_number * sig.doubleContraction(instantaneous_strain_rate) +
      _damage_dissipation; // The negative sign in damage dissipation is
                           // according
                           // to thermodynamics with internal state variables (see
                           // Rosakis et al, 2000)
  /* The following loop can ensure positive mechanical dissipation.
   * if (_mechanical_dissipation_mech[_qp] < 0)
  {
          mooseWarning("Dissipation is negative. Check the reason!");
  }*/

  // Compute Mechanical Dissipation Jacobian
  _mechanical_dissipation_jac_mech[_qp] =
      _mechanical_dissipation_mech[_qp] / (1 + _delta[_qp] * _T[_qp]) / (1 + _delta[_qp] * _T[_qp]);

  _poromech_kernel[_qp] = def_grad_rate * _peclet_number[_qp] / _mixture_compressibility[_qp];
  if (_has_T)
  {
    _poromech_jac[_qp] = (1 / (1 + _delta[_qp] * _T[_qp]) / (1 + _delta[_qp] * _T[_qp]));
  }
  else
  {
    _poromech_jac[_qp] = 0;
  }

  // Compute the equivalent Gruntfest number for comparison with SuCCoMBE TODO:
  // Remove this number from the tests!!!
  if (q_y != 0)
  {
    _mod_gruntfest_number[_qp] =
        gruntfest_number * std::exp(-_ar[_qp]) *
        (std::fabs(getSigEqv(sig) *
                   std::pow(macaulayBracket(getSigEqv(sig) / q_y - 1.0), _exponent)) +
         std::fabs(_mean_stress[_qp] *
                   std::pow(macaulayBracket(_mean_stress[_qp] - p_y), _exponent)));
  }
  // Begin of the chemical degradation method of Hu and Hueckel 2013 (doi:10.1680/geot.SIP13.P.020)
  // _mass_removal_rate[_qp] = 0;
  Real total_energy_input = sig.doubleContraction(instantaneous_strain_rate);

  _mass_removal_rate[_qp] = _chemo_mechanical_porosity_coeff * (1 + total_energy_input);
  if (_volumetric_strain[_qp] > 0)
  {
    _mass_removal_rate[_qp] = _chemo_mechanical_porosity_coeff * _volumetric_strain[_qp];
  }
  // End of the chemical degradation method of Hu and Hueckel 2013

  // formulate the Taylor-Quinney coefficient and Gruntfest numbers for the case
  // of damage

  return;
}

void
RedbackMechMaterial::computeQpStrain(const RankTwoTensor & Fhat)
{
  // Cinv - I = A A^T - A - A^T;
  RankTwoTensor A; // A = I - Fhatinv
  A.addIa(1.0);
  A -= Fhat.inverse();
  RankTwoTensor Cinv_I = A * A.transpose() - A - A.transpose();

  // strain rate D from Taylor expansion, Chat = (-1/2(Chat^-1 - I) +
  // 1/4*(Chat^-1 - I)^2 + ...
  _strain_increment[_qp] = -Cinv_I * 0.5 + Cinv_I * Cinv_I * 0.25;

  /* This line calculates the thermo-elastic strain in incremental form, as
   * follows :
   * thermal_strain = _solid_thermal_expansion * (T - T0)
   * thermal_strain_increment = thermal_strain - thermal_strain_old
   *                          = _solid_thermal_expansion * (T - T0) -
   * _solid_thermal_expansion * (T_old - T0)
   *                          = _solid_thermal_expansion * (T - T_old)
   * The negative sign is to satisfy the sign convention Redback has adopted
   * (positive fields in extension)
   */
  _strain_increment[_qp].addIa(-_solid_thermal_expansion[_qp] * (_T[_qp] - _T_old[_qp]));

  /*RankTwoTensor Chat = Fhat.transpose()*Fhat;
  RankTwoTensor A = Chat;
  A.addIa(-1.0);

  RankTwoTensor B = Chat*0.25;
  B.addIa(-0.75);
  _strain_increment[_qp] = -B*A;*/

  RankTwoTensor D;
  if (_dt > 0)
    D = _strain_increment[_qp] / _dt;
  else
    D.zero();
  _strain_rate[_qp] = D;

  // Calculate rotation R_incr
  RankTwoTensor invFhat(Fhat.inverse());

  std::vector<Real> a(3);
  a[0] = invFhat(1, 2) - invFhat(2, 1);
  a[1] = invFhat(2, 0) - invFhat(0, 2);
  a[2] = invFhat(0, 1) - invFhat(1, 0);
  Real q = (a[0] * a[0] + a[1] * a[1] + a[2] * a[2]) / 4.0;
  Real trFhatinv_1 = invFhat.trace() - 1.0;
  Real p = trFhatinv_1 * trFhatinv_1 / 4.0;
  // Real y = 1.0/((q + p)*(q + p)*(q + p));

  /*Real C1 = std::sqrt(p * (1 + (p*(q+q+(q+p))) * (1-(q+p)) * y));
  Real C2 = 0.125 + q * 0.03125 * (p*p - 12*(p-1)) / (p*p);
  Real C3 = 0.5 * std::sqrt( (p*q*(3-q) + p*p*p + q*q)*y );
  */

  Real C1 =
      std::sqrt(p + 3.0 * p * p * (1.0 - (p + q)) / ((p + q) * (p + q)) -
                2.0 * p * p * p * (1 - (p + q)) / ((p + q) * (p + q) * (p + q))); // cos theta_a
  Real C2 = 0.0;
  if (q > 0.01)
    C2 = (1.0 - C1) / (4.0 * q); // (1-cos theta_a)/4q
  else                           // alternate form for small q
    C2 = 0.125 + q * 0.03125 * (p * p - 12 * (p - 1)) / (p * p) +
         q * q * (p - 2.0) * (p * p - 10.0 * p + 32.0) / (p * p * p) +
         q * q * q * (1104.0 - 992.0 * p + 376.0 * p * p - 72 * p * p * p + 5.0 * p * p * p * p) /
             (512.0 * p * p * p * p);

  Real C3 = 0.5 * std::sqrt((p * q * (3.0 - q) + p * p * p + q * q) /
                            ((p + q) * (p + q) * (p + q))); // sin theta_a/(2 sqrt(q))

  // Calculate incremental rotation. Note that this value is the transpose of
  // that from Rashid, 93, so we transpose it before storing
  RankTwoTensor R_incr;
  R_incr.addIa(C1);
  for (unsigned int i = 0; i < 3; ++i)
    for (unsigned int j = 0; j < 3; ++j)
      R_incr(i, j) += C2 * a[i] * a[j];

  R_incr(0, 1) += C3 * a[2];
  R_incr(0, 2) -= C3 * a[1];
  R_incr(1, 0) -= C3 * a[2];
  R_incr(1, 2) += C3 * a[0];
  R_incr(2, 0) += C3 * a[1];
  R_incr(2, 1) -= C3 * a[0];
  _rotation_increment[_qp] = R_incr.transpose();
}

Real
RedbackMechMaterial::getSigEqv(const RankTwoTensor & stress)
{
  return std::pow(3 * stress.secondInvariant(), 0.5);
}

void
RedbackMechMaterial::returnMap(const RankTwoTensor & sig_old,
                               const RankTwoTensor & delta_d,
                               const RankFourTensor & E_ijkl,
                               RankTwoTensor & dp,
                               RankTwoTensor & sig,
                               Real & p_y,
                               Real & q_y)
{
  const Real tol1 = 1e-10; // TODO: expose to user interface and/or make the tolerance relative
  const Real tol3 = 1e-6;  // TODO: expose to user interface and/or make the tolerance relative
  Real err3 = 1.1 * tol3;
  bool is_plastic; // is this point in plastic regime or not?
  bool is_first_plastic_determined = false;
  bool is_first_plastic; // is_plastic the first time it's called

  Real eqvpstrain = std::pow(2.0 / 3.0, 0.5) * dp.L2norm();
  Real yield_stress = getYieldStress(eqvpstrain);

  // calculate the term _exponential = -Q_{mech}/(RT) with Q_{mech} = E_0 + p'c
  // V_{ref} + p_f V_{act}
  _exponential = 1;
  if (_has_D)
  {
    _exponential *= std::pow(1 - _damage[_qp], -_dmg_exponent);
  }
  if (_has_T)
  {
    // E_0/(RT) = Ar/(1+delta T*)
    _exponential *= std::exp(-_ar[_qp]) *
                    std::exp(_ar[_qp] * _delta[_qp] * _T[_qp] / (1 + _delta[_qp] * _T[_qp]));
  }

  // The following expression should be further pursued for a forward
  // physics-based model
  _exponential =
      _exponential * std::exp(-_alpha_1[_qp] * _confining_pressure[_qp] -
                              _pore_pres[_qp] * _alpha_2[_qp] *
                                  (1 + _alpha_3[_qp] * std::log(_confining_pressure[_qp])));

  unsigned int iterisohard = 0;
  const unsigned int maxiterisohard = 20, maxiter = 50;
  RankTwoTensor sig_new;
  RankTwoTensor dpn;
  while (err3 > tol3 && iterisohard < maxiterisohard) // Hardness update iteration
  {
    iterisohard++;
    unsigned int iter = 0;
    RankTwoTensor delta_dp;

    // Elastic guess
    sig_new = sig_old + E_ijkl * delta_d;
    // Compute distance to current yield surface (line), only valid for
    // associative potential
    Real p = sig_new.trace() / 3.0;
    Real q = getSigEqv(sig_new);
    get_py_qy_damaged(p, q, p_y, q_y, yield_stress, is_plastic);
    if (!is_first_plastic_determined)
    {
      is_first_plastic = is_plastic;
      is_first_plastic_determined = true;
    }
    if (is_first_plastic)
    {
      Real flow_incr = getFlowIncrement(q, p, q_y, p_y, yield_stress);

      RankTwoTensor flow_tensor;
      getFlowTensor(sig_new, q, p, q_y, p_y, yield_stress, flow_tensor);
      flow_tensor *= flow_incr;

      RankTwoTensor resid = flow_tensor - delta_dp;
      Real err1 = resid.L2norm();

      while (err1 > tol1 && iter < maxiter) // Stress update iteration (hardness fixed)
      {
        iter++;

        // Jacobian = d(residual)/d(sigma)
        RankFourTensor dr_dsig;
        getJac(sig_new, E_ijkl, flow_incr, q, p, p_y, q_y, yield_stress, dr_dsig);
        RankFourTensor dr_dsig_inv = dr_dsig.invSymm();
        RankTwoTensor ddsig = -dr_dsig_inv * resid; // Newton Raphson
        delta_dp -=
            E_ijkl.invSymm() * ddsig; // Update increment of plastic rate of deformation tensor
        sig_new += ddsig;             // Update stress

        // Update residual
        p = sig_new.trace() / 3.0;
        q = getSigEqv(sig_new);
        get_py_qy_damaged(p, q, p_y, q_y, yield_stress, is_plastic);

        flow_incr = getFlowIncrement(q, p, q_y, p_y, yield_stress);
        if (flow_incr < 0.0) // negative flow increment not allowed
          throw MooseException("Constitutive Error-Negative flow increment: Reduce time "
                               "increment.");
        getFlowTensor(sig_new, q, p, q_y, p_y, yield_stress, flow_tensor);
        flow_tensor *= flow_incr;
        resid = flow_tensor - delta_dp; // Residual
        err1 = resid.L2norm();
      }
      if (iter >= maxiter) // Convergence failure
        throw MooseException("Constitutive Error-Too many iterations: Reduce time "
                             "increment.\n"); // Convergence failure //TODO: check the
                                              // adaptive time stepping
    }
    _returnmap_iter[_qp] = iter;
    dpn = dp + delta_dp;
    eqvpstrain = std::pow(2.0 / 3.0, 0.5) * dpn.L2norm();
    Real yield_stress_prev = yield_stress;
    yield_stress = getYieldStress(eqvpstrain);
    err3 = std::abs(yield_stress - yield_stress_prev);
  }

  if (iterisohard >= maxiterisohard)
    throw MooseException("Constitutive Error-Too many iterations in Hardness "
                         "Update:Reduce time increment.\n"); // Convergence failure

  dp = dpn; // Plastic rate of deformation tensor in unrotated configuration
  sig = sig_new;
}

void
RedbackMechMaterial::get_py_qy_damaged(
    Real p, Real q, Real & p_y, Real & q_y, Real yield_stress, bool & is_plastic)
{
  get_py_qy(p, q, p_y, q_y, yield_stress, is_plastic);
  p_y *= (1 - _damage[_qp]);
  q_y *= (1 - _damage[_qp]);
}

/*void
RedbackMechMaterial::form_damage_kernels(Real cohesion)
{
  mooseError("form_damage_kernels must be overwritten in children class");
}*/

void
RedbackMechMaterial::formDamageDissipation(RankTwoTensor & /*sig*/)
{
  /* The damage potential is being formed in this function. We start by
   * postulating a helmholtz free energy of the form:
   *  Psi = Psi_damage * Psi0
   *  where Psi_damage = 1/2 *(1-D)^2
   *  and Psi0 is the purely mechanical expression of the Helmholtz free energy,
   * which in turn is split into a volumetric and a deviatoric part:
   *  Psi0 = Psi0_vol + Psi0_dev
   *  The volumetric part is: Psi0_vol = (2/3)*K*(epsilon^e_v)^2
   *  The deviatoric part is: Psi0_dev = (3/2)*G*(epsilon^e_d)^2
   *  Then, the damage potential is defined as damage_potential = - d Psi / d D
   * =  (1-D) * Psi0
   */
  Real bulk_modulus = _youngs_modulus * _poisson_ratio / (1 + _poisson_ratio) /
                      (1 - 2 * _poisson_ratio);                      // First Lame modulus
  Real shear_modulus = 0.5 * _youngs_modulus / (1 + _poisson_ratio); // Second Lame modulus (shear)

  Real vol_elastic_strain = _elastic_strain[_qp].trace();
  Real dev_elastic_strain = std::pow(2.0 / 3.0, 0.5) * _elastic_strain[_qp].L2norm();

  Real Psi0_vol = (2 / 3) * bulk_modulus * std::pow(vol_elastic_strain, 2);
  Real Psi0_dev = (3 / 2) * shear_modulus * std::pow(dev_elastic_strain, 2);
  Real Psi0 = Psi0_vol + Psi0_dev;

  Real damage_potential = (1 - _damage[_qp]) * Psi0;
  Real damage_rate = 0.;
  if (_dt > 0)
    damage_rate = (_damage[_qp] - _damage_old[_qp]) / _dt;

  // _damage_dissipation is equal to (- d Psi / d D * D_dot) which in this code
  // is (damage_potential * damage_rate)
  _damage_dissipation = damage_potential * damage_rate;
}

void
RedbackMechMaterial::form_damage_kernels(Real cohesion)
{
  // update damage evolution law from selected method
  switch (_damage_method)
  {
    case BrittleDamage:
      formBrittleDamage();
      break;
    case CreepDamage:
      formCreepDamage(cohesion);
      break;
    case BreakageMechanics:
      mooseError("damage method not implemented yet, use other options");
      break;
    case DamageHealing:
      mooseError("damage method not implemented yet, use other options");
      break;
    default:
      mooseError("damage method not implemented yet, use other options");
  }
}

void
RedbackMechMaterial::formBrittleDamage()
{
  // Kachanov's original law of Brittle Damage
  Real exponent_kachanov = 1;
  Real kachanov = _mises_stress[_qp] / (1 - _damage[_qp]);

  Real plastic_damage = _damage_coeff * std::pow(kachanov, exponent_kachanov);
  Real healing_damage = 0;

  Real mises_stress_old = getSigEqv(_stress_old[_qp]);
  if (mises_stress_old > _mises_stress[_qp])
  {
    _damage_kernel[_qp] = healing_damage;
    _damage_kernel_jac[_qp] = 0;
  }
  else
    _damage_kernel[_qp] = plastic_damage + healing_damage;
  _damage_kernel_jac[_qp] = 0;
}

void
RedbackMechMaterial::formCreepDamage(Real cohesion)
{
  // The derivative of the yield surface with respect to the
  // deviatoric stress q.
  // Damage evolution law for creep damage
  // J2 plastic potential with evolving cohesion for the damage evolution law
  // (remember that cohesion is q_y which is updated as q_y * (1-D) in the
  // get_py_qy_damaged function)
  Real d_yield_dq = 1 / std::pow(cohesion, 2);

  Real lambda_dot = 0.;
  if (d_yield_dq > 0) // ensuring positiveness of the plastic multiplier
  {
    /* the plastic multiplier could be having this form:
     * lambda_dot = _mises_stress[_qp] * _mises_strain_rate[_qp] / d_yield_dq;
     * but cohesion in J2 plasticity is the mises stress at yield, so we are
     * going with a much simpler form: */
    lambda_dot = _mises_strain_rate[_qp] / d_yield_dq;
  }

  Real plastic_damage = _damage_coeff * lambda_dot;
  Real healing_damage = 0;
  _damage_kernel[_qp] = plastic_damage + healing_damage;
  _damage_kernel_jac[_qp] = 0;
}
