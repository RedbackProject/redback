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
#include "RedbackMaterial.h"
#include "libmesh/quadrature_trap.h"

registerMooseObject("RedbackApp", RedbackMaterial);

InputParameters
RedbackMaterial::validParams()
{
  InputParameters params = Material::validParams();

  params.addParam<std::vector<std::string>>(
      "init_from_functions__params",{}, "The names of the parameters to be initialised as functions.");
  params.addParam<std::vector<FunctionName>>(
      "init_from_functions__function_names",{},
      "The corresponding names of the functions to be used for the parameters "
      "to be initialised as functions.");
  params.addRangeCheckedParam<Real>("phi0", 0.0, "phi0>=0 & phi0<1", "initial porosity value.");
  params.addRangeCheckedParam<Real>("gr", 1.0, "gr>=0", "Gruntfest number.");
  params.addParam<Real>("ref_lewis_nb", 1.0, "Reference Lewis number.");
  params.addParam<Real>("ar", 0.0, "Arrhenius number.");
  params.addParam<Real>("delta", 1, "Kamenetskii coefficient.");
  params.addParam<Real>("alpha_1",
                        0.0,
                        "First parameter for activation "
                        "volume, alpha_1 V_{ref} / (R T_{ref}) "
                        "in the redback paper");
  params.addParam<Real>("alpha_2",
                        0.0,
                        "Second parameter for activation "
                        "volume, alpha_2 V_{ref} / (R T_{ref}) "
                        "in the redback paper");
  params.addParam<Real>(
      "alpha_3", 0.0, "Third parameter for activation volume, alpha_3 in the redback paper");
  params.addParam<Real>("confining_pressure", 1, "Normalised confining pressure");
  params.addParam<Real>("biot_coefficient", 1.0, "Biot coefficient");
  params.addParam<bool>("is_mechanics_on", false, "is mechanics on?");
  params.addParam<bool>("is_chemistry_on", false, "is chemistry on?");
  params.addParam<bool>("are_convective_terms_on", false, "are convective terms on?");
  params.addCoupledVar("temperature", 0.0, "Dimensionless temperature");
  params.addCoupledVar("pore_pres", 0.0, "Dimensionless pore pressure");
  params.addCoupledVar("disp_x", 0.0, "The x displacement");
  params.addCoupledVar("disp_y", 0.0, "The y displacement");
  params.addCoupledVar("disp_z", 0.0, "The z displacement");
  params.addCoupledVar("total_porosity", 0.0, "The total porosity (as AuxKernel)");
  params.addCoupledVar(
      "inverse_lewis_number_tilde",
      0.0,
      "Varying component of (inverse of) Lewis number, coming from mutli-app for example");
  params.addCoupledVar("lewis_transversal",
                       "Lewis in transversal direction "
                       "of the inferface (only when used on lower dimensional subdomain)");
  params.addCoupledVar("continuation_parameter", 1.0, "The continuation parameter");
  params.addParam<MooseEnum>(
      "continuation_variable",
      RedbackMaterial::continuationMethodEnum() = "Gruntfest", // default value
      "The name of the variable multiplied by the continuation_parameter value");
  params.addCoupledVar("concentration", 0.0, "The concentration of species");

  // params.addCoupledVar("solid_velocity_aux", "Solid velocity (AuxKernel) from RedbackMechMaterial
  // (if used)");
  params.addParam<MooseEnum>(
      "density_method",
      RedbackMaterial::densityMethodEnum() = "linear",
      "The method to describe density evolution with temperature and pore pressure"); // TODO:
                                                                                      // fluid,
                                                                                      // solid,
                                                                                      // mixture?...
  params.addParam<MooseEnum>("permeability_method",
                             RedbackMaterial::permeabilityMethodEnum() = "KozenyCarman",
                             "The method to describe permeability evolution");

  params.addParam<Real>("ar_F", 0.0, "Arrhenius number for the forward reaction.");
  params.addParam<Real>("ar_R", 0.0, "Arrhenius number for the reverse reaction.");
  params.addParam<Real>(
      "chemical_ar_F_factor",
      0.0,
      "Mutliplying factor of concentration for Arrhenius of the forward reaction.");
  params.addParam<Real>("da_endo", 0, "Damkoehler number for the endothermic reaction.");
  params.addParam<Real>("da_exo", 0, "Damkoehler number for the exothermic reaction.");
  params.addParam<Real>("mu", 0, "Chemical pressurization coefficient.");
  params.addParam<Real>("Kc", 0, "Equilibrium constant.");
  params.addParam<Real>("eta1", 1, "ratio of concentrations (see documentation).");
  params.addParam<Real>("eta2", 1, "ratio of concentrations (see documentation).");
  params.addRangeCheckedParam<Real>("Aphi",
                                    0.0,
                                    "Aphi>=0 & Aphi<=1",
                                    "percentage of volume change from "
                                    "chemistry contributing to porosity (see "
                                    "documentation)");
  params.addParam<Real>("pressurization_coefficient", 0, "Pressurization coefficient (Lambda).");
  params.addParam<Real>("thermal_diffusivity", 1, "thermal diffusivity (cth) for the case of dimensional equations");
  params.addParam<Real>("Peclet_number", 1, "Peclet number");

  params.addParam<Real>("solid_compressibility",
                        1,
                        "solid compressibility (beta^{(s)} in "
                        "1/Pa)"); // _solid_compressibility_param
  params.addParam<Real>("fluid_compressibility",
                        0,
                        "fluid compressibility (beta^{(f)} in "
                        "1/Pa)"); // _fluid_compressibility_param
  params.addParam<Real>("solid_thermal_expansion",
                        0,
                        "solid expansion (lambda^{(s)} in 1/K)"); // _solid_thermal_expansion_param
  params.addParam<Real>("fluid_thermal_expansion",
                        0,
                        "fluid expansion (lambda^{(f)} in 1/K)"); // _fluid_thermal_expansion_param
  params.addParam<Real>(
      "thermal_diffusivity", 1, "normalised thermal diffusivity (-)"); // _thermal_diffusivity_param
  params.addParam<Real>(
      "solid_density", 2.5, "normalised solid density (-)");                 // solid_density_param
  params.addParam<Real>("fluid_density", 1, "normalised fluid density (-)"); // fluid_density_param

  params.addParam<RealVectorValue>("gravity",
                                   RealVectorValue(),
                                   "Gravitational acceleration (m/s^2) as a "
                                   "vector pointing downwards.  Eg "
                                   "(0,0,-9.81)");

  params.addParam<Real>(
      "temperature_reference", 0.0, "Reference temperature used for thermal expansion");
  params.addParam<Real>("pressure_reference", 0.0, "Reference pressure used for compressibility");

  return params;
}

RedbackMaterial::RedbackMaterial(const InputParameters & parameters)
  : Material(parameters),
    _has_T(isCoupled("temperature")),
    _T(_has_T ? coupledValue("temperature") : _zero),

    _has_pore_pres(isCoupled("pore_pres")),
    _pore_pres(_has_pore_pres ? coupledValue("pore_pres") : _zero),
    //_pore_pres_old(_has_pore_pres ? coupledValueOld("pore_pres") : _zero),

    _total_porosity(coupledValue("total_porosity")), // total_porosity MUST be
                                                     // coupled! Check that
                                                     // (TODO)
    _inverse_lewis_number_tilde(coupledValue("inverse_lewis_number_tilde")),
    _has_lewis_trans(isCoupled("lewis_transversal")),
    _lewis_t(_has_lewis_trans ? coupledValue("lewis_transversal") : _zero),
    _concentration(coupledValue("concentration")),
    _continuation_parameter(coupledScalarValue("continuation_parameter")),

    //_disp_x(isCoupled("disp_x") ? coupledValue("disp_x") : _zero),

    _init_from_functions__params(getParam<std::vector<std::string>>("init_from_functions__params")),
    _init_from_functions__function_names(
        getParam<std::vector<FunctionName>>("init_from_functions__function_names")),
    _phi0_param(getParam<Real>("phi0")),
    _gr_param(getParam<Real>("gr")),
    _ref_lewis_nb_param(getParam<Real>("ref_lewis_nb")),
    _ar_param(getParam<Real>("ar")),
    _delta_param(getParam<Real>("delta")),
    _confining_pressure_param(getParam<Real>("confining_pressure")),
    _biot_coeff_param(getParam<Real>("biot_coefficient")),
    _alpha_1_param(getParam<Real>("alpha_1")),
    _alpha_2_param(getParam<Real>("alpha_2")),
    _alpha_3_param(getParam<Real>("alpha_3")),
    _peclet_number_param(getParam<Real>("Peclet_number")),
    _ar_F_param(getParam<Real>("ar_F")),
    _ar_R_param(getParam<Real>("ar_R")),
    _chemical_ar_F_factor(getParam<Real>("chemical_ar_F_factor")),
    _da_endo_param(getParam<Real>("da_endo")),
    _da_exo_param(getParam<Real>("da_exo")),
    _mu_param(getParam<Real>("mu")),
    _Kc_param(getParam<Real>("Kc")),
    _eta1_param(getParam<Real>("eta1")),
    _eta2_param(getParam<Real>("eta2")),
    _Aphi_param(getParam<Real>("Aphi")),
    _pressurization_coefficient_param(getParam<Real>("pressurization_coefficient")),

    _solid_compressibility_param(getParam<Real>("solid_compressibility")),
    _fluid_compressibility_param(getParam<Real>("fluid_compressibility")),
    _solid_thermal_expansion_param(getParam<Real>("solid_thermal_expansion")),
    _fluid_thermal_expansion_param(getParam<Real>("fluid_thermal_expansion")),
    _thermal_diffusivity_param(getParam<Real>("thermal_diffusivity")),
    _solid_density_param(getParam<Real>("solid_density")),
    _fluid_density_param(getParam<Real>("fluid_density")),

    _is_mechanics_on(getParam<bool>("is_mechanics_on")),
    _is_chemistry_on(getParam<bool>("is_chemistry_on")),
    _are_convective_terms_on(getParam<bool>("are_convective_terms_on")),

    _gravity_param(getParam<RealVectorValue>("gravity")),
    _mixture_gravity_term(
        declareProperty<RealVectorValue>("mixture_gravity_term")),               // rho_mixture * g
    _fluid_gravity_term(declareProperty<RealVectorValue>("fluid_gravity_term")), // rho_fluid * g

    _gr(declareProperty<Real>("gr")),
    _ref_lewis_nb(declareProperty<Real>("ref_lewis_nb")),
    _ar(declareProperty<Real>("ar")),
    _confining_pressure(declareProperty<Real>("confining_pressure")),
    _biot_coeff(declareProperty<Real>("biot_coefficient")),
    _alpha_1(declareProperty<Real>("alpha_1")),
    _alpha_2(declareProperty<Real>("alpha_2")),
    _alpha_3(declareProperty<Real>("alpha_3")),
    _peclet_number(declareProperty<Real>("Peclet_number")),
    _delta(declareProperty<Real>("delta")),

    _initial_porosity(declareProperty<Real>("initial_porosity")),
    //_porosity(declareProperty<Real>("porosity")),
    _lewis_number(declareProperty<Real>("lewis_number")),
    _mixture_compressibility(declareProperty<Real>("mixture_compressibility")),

    _mechanical_dissipation_no_mech(declareProperty<Real>("mechanical_dissipation_no_mech")),
    _mechanical_dissipation_jac_no_mech(
        declareProperty<Real>("mechanical_dissipation_jacobian_no_mech")),

    _ar_F(declareProperty<Real>("ar_F")),
    _ar_R(declareProperty<Real>("ar_R")),
    _mu(declareProperty<Real>("mu")),

    _chemical_porosity(declareProperty<Real>("chemical_porosity")),
    _solid_ratio(declareProperty<Real>("solid_ratio")),
    _chemical_endothermic_energy(declareProperty<Real>("chemical_endothermic_energy")),
    _chemical_endothermic_energy_jac(declareProperty<Real>("chemical_endothermic_energy_jacobian")),
    _chemical_exothermic_energy(declareProperty<Real>("chemical_exothermic_energy")),
    _chemical_exothermic_energy_jac(declareProperty<Real>("chemical_exothermic_energy_jacobian")),
    _chemical_source_mass(declareProperty<Real>("chemical_source_mass")),
    _chemical_source_mass_jac(declareProperty<Real>("chemical_source_mass_jacobian")),

    _thermal_convective_mass(declareProperty<RealVectorValue>("thermal_convective_mass")),
    _pressure_convective_mass(declareProperty<RealVectorValue>("pressure_convective_mass")),
    //_convective_mass_jac_vec(declareProperty<RealVectorValue>("convective_mass_jacobian_vector")),
    //_convective_mass_jac_real(declareProperty<Real>("convective_mass_jacobian_real")),
    //_convective_mass_off_diag_vec(declareProperty<RealVectorValue>("convective_mass_off_diagonal_vector")),
    //_convective_mass_off_diag_real(declareProperty<Real>("convective_mass_off_diagonal_real")),

    _mixture_convective_energy(declareProperty<RealVectorValue>("mixture_convective_energy")),
    //_mixture_convective_energy_jac(declareProperty<Real>("mixture_convective_energy_jacobian")),
    //_mixture_convective_energy_off_jac(declareProperty<Real>("mixture_convective_energy_off_jacobian")),

    //_solid_velocity(declareProperty<RealVectorValue>("solid_velocity")),
    _fluid_velocity(declareProperty<RealVectorValue>("fluid_velocity")),
    _solid_compressibility(declareProperty<Real>("solid_compressibility")),
    _fluid_compressibility(declareProperty<Real>("fluid_compressibility")),
    _solid_thermal_expansion(declareProperty<Real>("solid_thermal_expansion")),
    _fluid_thermal_expansion(declareProperty<Real>("fluid_thermal_expansion")),
    _thermal_diffusivity(declareProperty<Real>("thermal_diffusivity")),

    _mixture_density(declareProperty<Real>("mixture_density")),

    _continuation_method((ContinuationMethod)(int)getParam<MooseEnum>("continuation_variable")),
    _density_method((DensityMethod)(int)getParam<MooseEnum>("density_method")),
    _permeability_method((PermeabilityMethod)(int)getParam<MooseEnum>("permeability_method")),

    _mises_strain(declareProperty<Real>("mises_strain")),
    _mises_strain_rate_nomech(declareProperty<Real>("mises_strain_rate_nomech")),

    _pressurization_coefficient(declareProperty<Real>("pressurization_coefficient")),

    _grad_temp(coupledGradient("temperature")),
    _grad_pore_pressure(coupledGradient("pore_pres")),
    //_grad_pore_pressure(_has_pore_pres ? coupledGradient("pore_pres") :
    //_zero) // TODO: what if H is not activated?
    //_grad_grad_pore_pressure(coupledSecond("pore_pres")),

    _dispx_dot(isCoupled("disp_x") ? coupledDot("disp_x") : _zero),
    _dispy_dot(isCoupled("disp_y") ? coupledDot("disp_y") : _zero),
    _dispz_dot(isCoupled("disp_z") ? coupledDot("disp_z") : _zero),
    _solid_velocity(declareProperty<RealVectorValue>("solid_velocity")),

    _T0_param(getParam<Real>("temperature_reference")),
    _P0_param(getParam<Real>("pressure_reference"))
{
  // Find functions to initialise parameters from
  unsigned int num_param_names = _init_from_functions__params.size();
  unsigned int num_fct_names = _init_from_functions__function_names.size();

  if (num_param_names != num_fct_names)
    mooseError("Number of init_from_functions__params much match the number of "
               "_init_from_functions__function_names!");

  _num_init_functions = num_param_names;
  _init_functions.resize(_num_init_functions);

  std::vector<std::string> valid_params;
  valid_params.push_back("gr");
  valid_params.push_back("ref_lewis_nb");
  valid_params.push_back("ar");
  valid_params.push_back("confining_pressure");
  valid_params.push_back("gravity");
  valid_params.push_back("thermal_diffusivity");
  unsigned int pos;
  for (unsigned int i = 0; i < _num_init_functions; i++)
  {
    pos = find(valid_params.begin(), valid_params.end(), _init_from_functions__params[i]) -
          valid_params.begin();
    if (pos >= valid_params.size())
    {
      std::string error_msg = std::string("Invalid parameter name '") +
                              _init_from_functions__params[i] +
                              std::string("' to initialise from function. The "
                                          "list of supported names is [");
      for (unsigned int j = 0; j < valid_params.size(); j++)
      {
        error_msg = error_msg + valid_params[j];
        if (j < valid_params.size() - 1)
          error_msg = error_msg + std::string(", ");
      }
      error_msg = error_msg + std::string("]");
      mooseError(error_msg);
    }
    _init_functions[i] = &getFunctionByName(_init_from_functions__function_names[i]);
  }
}

MooseEnum
RedbackMaterial::continuationMethodEnum()
{
  return MooseEnum("Gruntfest Lewis");
}

MooseEnum
RedbackMaterial::densityMethodEnum()
{
  return MooseEnum("linear");
}

MooseEnum
RedbackMaterial::permeabilityMethodEnum()
{
  return MooseEnum("KozenyCarman");
}

void
RedbackMaterial::stepInitQpProperties()
{
  // TODO: Variable initialisation we'd like done only once (one off)
  // but can't figure out how so doing it at every step...
  _initial_porosity[_qp] = _phi0_param;
  //_porosity[_qp] = _phi0_param; // _total_porosity now coming from AuxKernel
  //(coupled to this material in .i)
  _chemical_porosity[_qp] = 0;
  _solid_ratio[_qp] = 0;
  _mises_strain[_qp] = 0;
  _solid_velocity[_qp] = RealVectorValue(_dispx_dot[_qp], _dispy_dot[_qp], _dispz_dot[_qp]);
  _fluid_velocity[_qp] = RealVectorValue();

  // Variable initialisation (called at each step)
  unsigned int pos; // position index (to find a string in vector of strings)
  pos = find(_init_from_functions__params.begin(), _init_from_functions__params.end(), "gr") -
        _init_from_functions__params.begin();
  if (pos < _num_init_functions)
  {
    _gr[_qp] = _init_functions[pos]->value(_t, _q_point[_qp]);
  }
  else
  {
    _gr[_qp] = _gr_param;
  }
  pos = find(_init_from_functions__params.begin(),
             _init_from_functions__params.end(),
             "ref_lewis_nb") -
        _init_from_functions__params.begin();
  if (pos < _num_init_functions)
  {
    _ref_lewis_nb[_qp] = _init_functions[pos]->value(_t, _q_point[_qp]);
  }
  else
  {
    _ref_lewis_nb[_qp] = _ref_lewis_nb_param;
  }
  pos = find(_init_from_functions__params.begin(), _init_from_functions__params.end(), "ar") -
        _init_from_functions__params.begin();
  if (pos < _num_init_functions)
  {
    _ar[_qp] = _init_functions[pos]->value(_t, _q_point[_qp]);
  }
  else
  {
    _ar[_qp] = _ar_param;
  }
  pos = find(_init_from_functions__params.begin(),
             _init_from_functions__params.end(),
             "confining_pressure") -
        _init_from_functions__params.begin();
  if (pos < _num_init_functions)
  {
    _confining_pressure[_qp] = _init_functions[pos]->value(_t, _q_point[_qp]);
  }
  else
  {
    _confining_pressure[_qp] = _confining_pressure_param;
  }
  pos = find(_init_from_functions__params.begin(), _init_from_functions__params.end(), "gravity") -
        _init_from_functions__params.begin();
  if (pos < _num_init_functions)
  {
    _gravity_param = _init_functions[pos]->vectorValue(_t, _q_point[_qp]);
    // TODO: does not need to be (re)set for  each _qp...
  }
  pos = find(_init_from_functions__params.begin(),
             _init_from_functions__params.end(),
             "thermal_diffusivity") -
        _init_from_functions__params.begin();
  if (pos < _num_init_functions)
  {
    _thermal_diffusivity[_qp] = _init_functions[pos]->value(_t, _q_point[_qp]);
  }
  else
  {
    _thermal_diffusivity[_qp] = _thermal_diffusivity_param;
  }

  switch (_continuation_method)
  {
    case Gruntfest:
      _gr[_qp] *= _continuation_parameter[0];
      break;
    case Lewis:
      _ref_lewis_nb[_qp] *= _continuation_parameter[0];
      break;
    default:
      mooseError("Continuation method not implemented yet");
  }

  _biot_coeff[_qp] = _biot_coeff_param;
  _alpha_1[_qp] = _alpha_1_param;
  _alpha_2[_qp] = _alpha_2_param;
  _alpha_3[_qp] = _alpha_3_param;
  _peclet_number[_qp] = _peclet_number_param;
  _delta[_qp] = _delta_param;
  _lewis_number[_qp] = _ref_lewis_nb[_qp];
  _ar_F[_qp] = _ar_F_param;
  _ar_R[_qp] = _ar_R_param;
  _mu[_qp] = _mu_param;
  _pressurization_coefficient[_qp] = _pressurization_coefficient_param;
  _solid_compressibility[_qp] = _solid_compressibility_param;
  _fluid_compressibility[_qp] = _fluid_compressibility_param;
  _solid_thermal_expansion[_qp] = _solid_thermal_expansion_param;
  _fluid_thermal_expansion[_qp] = _fluid_thermal_expansion_param;
  _thermal_diffusivity[_qp] = _thermal_diffusivity_param;
  _mixture_density[_qp] =
      (1 - _phi0_param) * _solid_density_param + _phi0_param * _fluid_density_param;
  _mixture_gravity_term[_qp] = _mixture_density[_qp] * _gravity_param;
  _fluid_gravity_term[_qp] = _fluid_density_param * _gravity_param;
  _dplastic_heat_dstrain_no_mech[_qp].zero();
  _dplastic_heat_dcurvature_no_mech[_qp].zero();
}

void
RedbackMaterial::computeQpProperties()
{
  // Set our variables
  stepInitQpProperties();

  // Default Material method
  Material::computeQpProperties();

  // Compute the terms used in Redback Kernels
  computeRedbackTerms();
}

void
RedbackMaterial::computeRedbackTerms()
{
  Real omega_rel, temporary, phi_prime, s_prime;
  Real beta_star_m, one_minus_phi_beta_star_s, phi_beta_star_f;

  // TODO: put flags for all properties depending on activated variables.

  // TODO: do not compute these when mechanics is on (5 fields overwritten)
  _exponential = std::exp(-_ar[_qp]) *
                 std::exp(_ar[_qp] * _delta[_qp] * _T[_qp] / (1 + _delta[_qp] * _T[_qp]));
  // Compute Mises strain
  _mises_strain[_qp] = _exponential * _dt;
  // Compute Mises strain rate
  _mises_strain_rate_nomech[_qp] = _exponential;

  if (!_is_mechanics_on)
  {
    // Compute Mechanical Dissipation
    _mechanical_dissipation_no_mech[_qp] =
        _gr[_qp] * std::exp(_ar[_qp]) *
        std::exp(-_alpha_1[_qp] * _confining_pressure[_qp] -
                 _pore_pres[_qp] * _alpha_2[_qp] *
                     (1 + _alpha_3[_qp] * std::log(_confining_pressure[_qp]))) *
        std::exp(_ar[_qp] * _delta[_qp] * _T[_qp] / (1 + _delta[_qp] * _T[_qp]));

    // Compute Mechanical Dissipation Jacobian
    _mechanical_dissipation_jac_no_mech[_qp] = _mechanical_dissipation_no_mech[_qp] * _ar[_qp] *
                                               _delta[_qp] / (1 + _delta[_qp] * _T[_qp]) /
                                               (1 + _delta[_qp] * _T[_qp]);
  }

  if (_is_chemistry_on)
  {
    /*
     * The following calculates the volume ratios of a generic reversible
     * reaction of the form:
     * AB_(solid) <-> A_(solid)  + B_(fluid)
     * The chemical porosity is the volume ration of B over the total volume
     * V=V_A + V_B + V_AB
     * and the solid ratio is the volume of product A over the solid volume
     * V_A+V_AB
     */

    // Update chemical Arrhenius term
    _ar_F[_qp] += _chemical_ar_F_factor * _concentration[_qp];

    // Step 1: calculate the relative rate of reactions
    omega_rel = _eta2_param * _Kc_param *
                std::exp(-(_ar_F[_qp] - _ar_R[_qp]) / (1 + _delta[_qp] * _T[_qp]));

    // Step 2: calculate the solid ratio
    _solid_ratio[_qp] = omega_rel / (1 + omega_rel);

    // Step 3: calculate the chemical porosity and update the total porosity
    _chemical_porosity[_qp] =
        _Aphi_param * (1 - _phi0_param) * _solid_ratio[_qp] / (_solid_ratio[_qp] + _eta1_param);
    //_porosity[_qp] =  _phi0_param + _chemical_porosity[_qp];
    // _total_porosity will be updated through the AuxKernel (at next iteration)

    // Step 4: calculate the partial derivatives for the jacobian
    temporary = _eta2_param * _Kc_param * (_ar_F[_qp] - _ar_R[_qp]) * _delta[_qp] *
                std::exp((_ar_F[_qp] - _ar_R[_qp]) / (1 + _delta[_qp] * _T[_qp])) /
                std::pow(1 + _delta[_qp] * _T[_qp], 2);

    phi_prime =
        -temporary * _Aphi_param * _eta1_param * (1 - _phi0_param) /
        std::pow(_eta1_param * std::exp(_ar_R[_qp] / (1 + _delta[_qp] * _T[_qp])) +
                     (1 + _eta1_param) * std::exp(_ar_F[_qp] / (1 + _delta[_qp] * _T[_qp])) *
                         _eta2_param * _Kc_param,
                 2);

    s_prime = -temporary / std::pow(std::exp(_ar_R[_qp] / (1 + _delta[_qp] * _T[_qp])) +
                                        std::exp(_ar_F[_qp] / (1 + _delta[_qp] * _T[_qp])) *
                                            _eta2_param * _Kc_param,
                                    2);

    // Compute Endothermic Chemical Energy
    _chemical_endothermic_energy[_qp] =
        _da_endo_param * (1 - _total_porosity[_qp]) * (1 - _solid_ratio[_qp]) *
        std::exp((_ar_F[_qp] * _delta[_qp] * _T[_qp]) / (1 + _delta[_qp] * _T[_qp]));

    // Compute Endothermic Chemical Energy Jacobian
    _chemical_endothermic_energy_jac[_qp] =
        _da_endo_param * std::exp((_ar_F[_qp]) / (1 + _delta[_qp] * _T[_qp])) *
        (_ar_F[_qp] * _delta[_qp] * (1 - _total_porosity[_qp]) * (1 - _solid_ratio[_qp]) /
             std::pow(1 + _delta[_qp] * _T[_qp], 2) -
         (1 - _solid_ratio[_qp]) * phi_prime - (1 - _total_porosity[_qp]) * s_prime);

    // Compute Exothermic Chemical Energy
    _chemical_exothermic_energy[_qp] =
        _da_exo_param * (1 - _total_porosity[_qp]) * _solid_ratio[_qp] * _chemical_porosity[_qp] *
        std::exp((_ar_R[_qp] * _delta[_qp] * _T[_qp]) / (1 + _delta[_qp] * _T[_qp]));

    // Compute Exothermic Chemical Energy Jacobian
    _chemical_exothermic_energy_jac[_qp] =
        _da_exo_param * std::exp(_ar_R[_qp] / (1 + _delta[_qp] * _T[_qp])) *
        (_solid_ratio[_qp] *
             (_ar_R[_qp] * _delta[_qp] * _chemical_porosity[_qp] * (1 - _total_porosity[_qp]) /
                  std::pow(1 + _delta[_qp] * _T[_qp], 2) +
              (1 - _total_porosity[_qp] - _chemical_porosity[_qp]) * phi_prime) +
         _chemical_porosity[_qp] * (1 - _total_porosity[_qp]) * s_prime);

    // Compute Chemical Source/Sink Term for the mass (pore pressure) equation
    _chemical_source_mass[_qp] =
        _mu[_qp] * (1 - _total_porosity[_qp]) * (1 - _solid_ratio[_qp]) *
        std::exp((_ar_F[_qp] * _delta[_qp] * _T[_qp]) / (1 + _delta[_qp] * _T[_qp]));

    // Compute Jacobian of Chemical Source/Sink Term for the mass (pore
    // pressure) equation. The corresponding variable
    // is pore pressure
    _chemical_source_mass_jac[_qp] =
        0; /*_mu[_qp]* std::exp( (_ar_F[_qp]) / (1 + _delta[_qp]*_T[_qp]) ) *
(
_ar_F[_qp] * _delta[_qp] * (1 - _total_porosity[_qp]) * (1 - _solid_ratio[_qp])
/ std::pow(1+_delta[_qp]*_T[_qp], 2)
- (1 - _solid_ratio[_qp]) * phi_prime
- (1 - _total_porosity[_qp]) * s_prime
);*/

    // Update Lewis number
    if (_phi0_param != _total_porosity[_qp])
      _lewis_number[_qp] = _ref_lewis_nb[_qp] *
                           std::pow((1 - _total_porosity[_qp]) / (1 - _phi0_param), 2) *
                           std::pow(_phi0_param / _total_porosity[_qp], 3);
  }
  if (_inverse_lewis_number_tilde[_qp] != 0)
  {
    // to include modifications from multi-app for example
    Real inverse_lewis_number = 1 / _lewis_number[_qp] + _inverse_lewis_number_tilde[_qp];
    _lewis_number[_qp] = 1 / inverse_lewis_number;
  }

  // Forming the compressibilities of the phases
  one_minus_phi_beta_star_s =
      (1 - _total_porosity[_qp]) * _solid_compressibility[_qp]; // normalized
  // compressibility of
  // the solid phase
  phi_beta_star_f =
      _total_porosity[_qp] * _fluid_compressibility[_qp]; // normalized compressibility of the fluid
                                                          // phase
  beta_star_m =
      one_minus_phi_beta_star_s + phi_beta_star_f; // normalized compressibility of the mixture
  _mixture_compressibility[_qp] = beta_star_m;
  if (_mixture_compressibility[_qp] == 0)
    mooseError("The mixture compressiblity cannot be zero!");

  // convective terms
  if (_are_convective_terms_on)
  {
    Real solid_density, fluid_density;
    Real /*lambda_m_star,*/ one_minus_phi_lambda_s, phi_lambda_f;
    RealVectorValue mixture_velocity, normalized_gravity;

    // Forming the partial densities and gravity terms
    switch (_density_method)
    {
      case linear:
        // Linear approximation of the EOS (Equation Of State)
        solid_density = _solid_density_param *
                        (1 + _solid_compressibility[_qp] * (_pore_pres[_qp] - _P0_param) -
                         _solid_thermal_expansion[_qp] * (_T[_qp] - _T0_param));
        fluid_density = _fluid_density_param *
                        (1 + _fluid_compressibility[_qp] * (_pore_pres[_qp] - _P0_param) -
                         _fluid_thermal_expansion[_qp] * (_T[_qp] - _T0_param));
        break;
      default:
        mooseError("density method not implemented yet, use linear");
    }
    _mixture_density[_qp] =
        (1 - _total_porosity[_qp]) * solid_density + _total_porosity[_qp] * fluid_density;

    // Terms feeding the stress equilibrium and Darcy flux
    normalized_gravity = _gravity_param;

    _mixture_gravity_term[_qp] =
        _mixture_density[_qp] * normalized_gravity; // for the stress equilibrium equation
    _fluid_gravity_term[_qp] = fluid_density * normalized_gravity; // for Darcy's flux

    // Forming the thermal expansions of the phases
    one_minus_phi_lambda_s = (1 - _total_porosity[_qp]) *
                             _solid_thermal_expansion[_qp]; // normalized thermal expansion
                                                            // coefficient of the solid phase
    phi_lambda_f = _total_porosity[_qp] * _fluid_thermal_expansion[_qp]; // normalized thermal
                                                                         // expansion coefficient of
                                                                         // the fluid phase
    // lambda_m_star =
    //     one_minus_phi_lambda_s + phi_lambda_f; // normalized compressibility of the mixture

    // Forming the velocities through mechanics and Darcy's flow law
    if (_total_porosity[_qp] != 0)
    {
      RealVectorValue grad_pp_grav = _grad_pore_pressure[_qp] - fluid_density * normalized_gravity;
      Real factor = beta_star_m / (_peclet_number[_qp] * _total_porosity[_qp]);
      // solving Darcy's flux for the fluid velocity
      _fluid_velocity[_qp] = _solid_velocity[_qp] - factor * grad_pp_grav / _lewis_number[_qp];

      if (_has_lewis_trans == true)
      {
        const auto dim = _current_elem->dim() + 1; // should be mesh->mesh_dimension();
        FEType fe_type(_current_elem->default_order(), LAGRANGE);
        std::unique_ptr<FEBase> fe_face(FEBase::build(dim, fe_type));
        QTrap qface(dim - 1);
        fe_face->attach_quadrature_rule(&qface);
        const std::vector<Point> & normals = fe_face->get_normals();
        const Elem * interior_parent = _current_elem->interior_parent();
        mooseAssert(interior_parent,
                    "No interior parent exists for element "
                        << _current_elem->id()
                        << ". There may be a problem with your sideset set-up.");
        auto s = interior_parent->which_side_am_i(_current_elem);
        fe_face->reinit(interior_parent, s);

        _fluid_velocity[_qp] += factor * (1. / _lewis_number[_qp] - 1. / _lewis_t[_qp]) *
                                (grad_pp_grav * normals[0]) * normals[0];
      }
    }
    else
      _fluid_velocity[_qp] = _solid_velocity[_qp];

    mixture_velocity = (solid_density / _mixture_density[_qp]) * _solid_velocity[_qp] +
                       (fluid_density / _mixture_density[_qp]) *
                           _fluid_velocity[_qp]; // barycentric velocity for the mixture

    // Forming the kernels and their jacobians
    _pressure_convective_mass[_qp] =
        _peclet_number[_qp] *
        ((one_minus_phi_beta_star_s / beta_star_m) * _solid_velocity[_qp] +
         (phi_beta_star_f / beta_star_m) * _fluid_velocity[_qp]); // convective term multiplying
                                                                  // the pressure flux in the mass
                                                                  // equation. TODO: disable for
                                                                  // incompressible case
    _thermal_convective_mass[_qp] =
        _peclet_number[_qp] *
        ((one_minus_phi_lambda_s / beta_star_m) * _solid_velocity[_qp] +
         (phi_lambda_f / beta_star_m) * _fluid_velocity[_qp]); // convective term multiplying the
                                                               // thermal flux in the mass equation

    //_convective_mass_jac_vec[_qp] = _pressure_convective_mass[_qp] -
    //(_fluid_compressibility[_qp]*_grad_pore_pressure[_qp] -
    //_fluid_thermal_expansion[_qp]*_grad_temp[_qp])/_lewis_number[_qp];
    //_convective_mass_jac_real[_qp] =
    //(_fluid_compressibility[_qp]*fluid_density*normalized_gravity/_lewis_number[_qp])*(_fluid_compressibility[_qp]*_grad_pore_pressure[_qp]
    //- _fluid_thermal_expansion[_qp]*_grad_temp[_qp]);

    //_convective_mass_off_diag_vec[_qp] = -_thermal_convective_mass[_qp];
    //_convective_mass_off_diag_real[_qp] =
    //-(_fluid_thermal_expansion[_qp]*fluid_density*normalized_gravity/_lewis_number[_qp])*(_fluid_compressibility[_qp]*_grad_pore_pressure[_qp]
    //- _fluid_thermal_expansion[_qp]*_grad_temp[_qp]);

    _mixture_convective_energy[_qp] =
        _peclet_number[_qp] * mixture_velocity; // convective term multiplying
                                                // the thermal flux in the
                                                // energy equation
    //_mixture_convective_energy_jac[_qp] =
    //-(_peclet_number[_qp]/_lewis_number[_qp])*lambda_fluid*_fluid_gravity_term[_qp]*_grad_temp[_qp];
    ////RealVectorValue(); //derivative with respect to temperature
    //_mixture_convective_energy_off_jac[_qp] =
    //(_peclet_number[_qp]/_lewis_number[_qp])*(phi_beta_star_f*_fluid_gravity_term[_qp]*_grad_temp[_qp]
    //- 0); // 2nd
    // term is for del_square_P; //derivative with respect to temperature
  }
  return;
}
