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

#ifndef REDBACKMATERIAL_H
#define REDBACKMATERIAL_H

//#include "FiniteStrainPlasticMaterial.h"
#include "Material.h"
#include "RankTwoTensor.h"

class RedbackMaterial : public Material
{
public:
  RedbackMaterial(const InputParameters & parameters);

  static InputParameters validParams();

  /// Static method for use in validParams for getting the continuation method
  static MooseEnum continuationMethodEnum();

  // various choices to decide which variable to use for continuation
  enum ContinuationMethod
  {
    Gruntfest,
    Lewis
  };

  /// Static method for use in validParams for getting the density method
  static MooseEnum densityMethodEnum();

  // various methods to handle density (variation with T and P)
  enum DensityMethod
  {
    linear
  };

  /// Static method for use in validParams for getting the permeability method
  static MooseEnum permeabilityMethodEnum();

  // various methods to handle permeability evolution
  enum PermeabilityMethod
  {
    KozenyCarman
  };

protected:
  virtual void stepInitQpProperties();
  virtual void computeQpProperties() override;
  virtual void computeRedbackTerms();

  bool _has_T;
  const VariableValue & _T;
  bool _has_pore_pres;

  const VariableValue & _pore_pres; //, & _pore_pres_old;
  const VariableValue & _total_porosity;
  const VariableValue & _inverse_lewis_number_tilde;
  bool _has_lewis_trans;
  const VariableValue & _lewis_t;

  const VariableValue & _concentration;
  const VariableValue & _continuation_parameter; // Coupled scalar variable value

  // functionality to initialise some parameters from function (overwrites
  // initialisation as float)
  std::vector<std::string> _init_from_functions__params;
  std::vector<FunctionName> _init_from_functions__function_names;
  std::vector<const Function *> _init_functions;
  unsigned int _num_init_functions;

  Real _phi0_param, _gr_param, _ref_lewis_nb_param, _ar_param, _delta_param;
  Real _confining_pressure_param, _biot_coeff_param, _alpha_1_param, _alpha_2_param, _alpha_3_param,
      _peclet_number_param; //_ar_c_param, _da_param, _mu_param,
  Real _ar_F_param, _ar_R_param, _chemical_ar_F_factor, _da_endo_param, _da_exo_param, _mu_param,
      _Kc_param, _eta1_param, _eta2_param, _Aphi_param, _pressurization_coefficient_param;
  Real _solid_compressibility_param, _fluid_compressibility_param, _solid_thermal_expansion_param,
      _fluid_thermal_expansion_param, _thermal_diffusivity_param, _solid_density_param,
      _fluid_density_param;
  bool _is_mechanics_on, _is_chemistry_on, _are_convective_terms_on;

  RealVectorValue _gravity_param;

  MaterialProperty<RealVectorValue> & _mixture_gravity_term;
  MaterialProperty<RealVectorValue> & _fluid_gravity_term;

  MaterialProperty<Real> & _gr;
  MaterialProperty<Real> & _ref_lewis_nb;
  MaterialProperty<Real> & _ar;
  MaterialProperty<Real> & _confining_pressure;
  MaterialProperty<Real> & _biot_coeff;
  MaterialProperty<Real> & _alpha_1;
  MaterialProperty<Real> & _alpha_2;
  MaterialProperty<Real> & _alpha_3;
  MaterialProperty<Real> & _peclet_number;
  MaterialProperty<Real> & _delta;
  MaterialProperty<Real> & _initial_porosity;
  MaterialProperty<Real> & _lewis_number;
  MaterialProperty<Real> & _mixture_compressibility;

  MaterialProperty<Real> & _mechanical_dissipation_no_mech;
  MaterialProperty<Real> & _mechanical_dissipation_jac_no_mech;

  MaterialProperty<Real> & _ar_F;
  MaterialProperty<Real> & _ar_R;
  MaterialProperty<Real> & _mu;

  MaterialProperty<Real> & _chemical_porosity;
  MaterialProperty<Real> & _solid_ratio;
  MaterialProperty<Real> & _chemical_endothermic_energy;
  MaterialProperty<Real> & _chemical_endothermic_energy_jac;
  MaterialProperty<Real> & _chemical_exothermic_energy;
  MaterialProperty<Real> & _chemical_exothermic_energy_jac;
  MaterialProperty<Real> & _chemical_source_mass;
  MaterialProperty<Real> & _chemical_source_mass_jac;

  MaterialProperty<RealVectorValue> & _thermal_convective_mass;
  MaterialProperty<RealVectorValue> & _pressure_convective_mass;
  // MaterialProperty<RealVectorValue> & _convective_mass_jac_vec;
  // MaterialProperty<Real> & _convective_mass_jac_real;
  // MaterialProperty<RealVectorValue> & _convective_mass_off_diag_vec;
  // MaterialProperty<Real> & _convective_mass_off_diag_real;
  MaterialProperty<RealVectorValue> & _mixture_convective_energy;
  // MaterialProperty<Real> & _mixture_convective_energy_jac;
  // MaterialProperty<Real> & _mixture_convective_energy_off_jac;

  MaterialProperty<RealVectorValue> & _fluid_velocity;
  MaterialProperty<Real> & _solid_compressibility;   // \bar(\beta_s)/\sigma_{ref}
  MaterialProperty<Real> & _fluid_compressibility;   // \bar(\beta_f)/\sigma_{ref}
  MaterialProperty<Real> & _solid_thermal_expansion; // \bar(\lambda_s)/ \delta T_ref
  MaterialProperty<Real> & _fluid_thermal_expansion; // \bar(\lambda_f)/delta T_ref
  MaterialProperty<Real> & _thermal_diffusivity;

  MaterialProperty<Real> & _mixture_density;

  ContinuationMethod _continuation_method;
  DensityMethod _density_method;
  PermeabilityMethod _permeability_method;

  Real _exponential;

  MaterialProperty<Real> & _mises_strain;
  MaterialProperty<Real> & _mises_strain_rate_nomech;

  MaterialProperty<Real> & _pressurization_coefficient;

  const VariableGradient & _grad_temp;
  const VariableGradient & _grad_pore_pressure;
  // VariableSecond& _grad_grad_pore_pressure;

  const VariableValue & _dispx_dot;
  const VariableValue & _dispy_dot;
  const VariableValue & _dispz_dot;
  MaterialProperty<RealVectorValue> & _solid_velocity;

  Real _T0_param, _P0_param;

  MaterialProperty<RankTwoTensor> & _dplastic_heat_dstrain_no_mech;
  MaterialProperty<RankTwoTensor> & _dplastic_heat_dcurvature_no_mech;
};

#endif // REDBACKMATERIAL_H
