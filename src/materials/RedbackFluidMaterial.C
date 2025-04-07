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

#include "RedbackFluidMaterial.h"

registerMooseObject("RedbackApp", RedbackFluidMaterial);

InputParameters
RedbackFluidMaterial::validParams()
{
  InputParameters params = Material::validParams();

  params.addCoupledVar("temperature", 0.0, "Dimensionless temperature");
  params.addCoupledVar("pore_pres", 0.0, "Dimensionless pore pressure");
  params.addCoupledVar("fluid_vel_x", 0.0, "The x component of fluid velocity");
  params.addCoupledVar("fluid_vel_y", 0.0, "The y component of fluid velocity");
  params.addCoupledVar("fluid_vel_z", 0.0, "The z component of fluid velocity");
  params.addParam<Real>(
      "viscosity_ratio",
      0,
      "Ratio of Fluid second viscosity to Fluid dynamic viscosity (lambda/μ)"); // μb=lambda-2/3
  params.addParam<Real>("fluid_density", 1, "Reference fluid density (\rho)");
  params.addParam<Real>("fluid_compressibility",
                        0,
                        "Fluid compressibility (beta^{(f)} in 1/Pa)"); // default is incompressible
  params.addParam<Real>("fluid_thermal_expansion",
                        0,
                        "Fluid expansion (lambda^{(f)} in 1/K)"); // _fluid_thermal_expansion_param
  params.addParam<Real>(
      "temperature_reference", 0.0, "Reference temperature used for thermal expansion");
  params.addParam<Real>("pressure_reference", 0.0, "Reference pressure used for compressibility");
  params.addParam<Real>("Peclet_number", 1, "Peclet number");
  params.addParam<Real>("Reynolds_number", 1, "Reynolds number");
  params.addParam<Real>("Froude_number", 1, "Froude number");

  params.addParam<RealVectorValue>(
      "gravity",
      RealVectorValue(),
      "Gravitational acceleration (m/s^2) as a vector pointing downwards.  Eg '0 0 -9.81'");
  return params;
}

RedbackFluidMaterial::RedbackFluidMaterial(const InputParameters & parameters)
  : Material(parameters),
    _has_T(isCoupled("temperature")),
    _T(_has_T ? coupledValue("temperature") : _zero),

    _has_pore_pres(isCoupled("pore_pres")),
    _pore_pres(_has_pore_pres ? coupledValue("pore_pres") : _zero),

    _fluid_vel_x(coupledValue("fluid_vel_x")),
    _fluid_vel_y(coupledValue("fluid_vel_y")),
    _fluid_vel_z(coupledValue("fluid_vel_z")),

    _grad_fluid_vel_x(coupledGradient("fluid_vel_x")),
    _grad_fluid_vel_y(coupledGradient("fluid_vel_y")),
    //  _grad_fluid_vel_z(_mesh.dimension() == 3 ? coupledGradient("fluid_vel_z") : _grad_zero),
    _grad_fluid_vel_z(coupledGradient("fluid_vel_z")),

    _gravity_param(getParam<RealVectorValue>("gravity")),

    _peclet_number_param(getParam<Real>("Peclet_number")),
    _reynolds_number_param(getParam<Real>("Reynolds_number")),
    _froude_number_param(getParam<Real>("Froude_number")),

    _gravity_term(declareProperty<RealVectorValue>(
        "gravity_term")), // actually fluid gravity (but need to be called mixture for the kernel)

    _fluid_density(declareProperty<Real>(
        "NS_fluid_density")), // this fluid density is used in the stressdivergence kernel
    _div_fluid_vel(declareProperty<Real>("divergence_of_fluid_velocity")),
    _div_fluid_kernel(declareProperty<Real>("divergence_fluid_velocity_kernel")),
    _pressurization_coefficient(declareProperty<Real>("pressurization_coefficient")),
    _peclet_number(declareProperty<Real>("Peclet_number")),
    _reynolds_number(declareProperty<Real>("Reynolds_number")),
    _froude_number(declareProperty<Real>("Froude_number")),
    _viscosity_ratio(declareProperty<Real>("viscosity_ratio")),
    _thermal_convective_mass(declareProperty<RealVectorValue>("thermal_convective_mass")),
    _pressure_convective_mass(declareProperty<RealVectorValue>("pressure_convective_mass")),
    _fluid_stress(declareProperty<RankTwoTensor>("fluid_stress")),

    //_Jacobian_fluid_mult(declareProperty<RankFourTensor>("Jacobian_fluid_mult")),

    _viscosity_ratio_param(getParam<Real>("viscosity_ratio")),
    //_bulk_viscosity_param(getParam<Real>("bulk_viscosity")),
    //_dynamic_viscosity_param(getParam<Real>("dynamic_viscosity")),
    _fluid_density_param(getParam<Real>("fluid_density")),
    _fluid_compressibility_param(getParam<Real>("fluid_compressibility")),
    _fluid_thermal_expansion_param(getParam<Real>("fluid_thermal_expansion")),
    _grad_temp(coupledGradient("temperature")),
    _grad_pore_pressure(coupledGradient("pore_pres")),
    _T0_param(getParam<Real>("temperature_reference")),
    _P0_param(getParam<Real>("pressure_reference"))

{
}

//=================================================================

void
RedbackFluidMaterial::stepInitQpProperties()
{
  _fluid_density[_qp] = _fluid_density_param;
}

void
RedbackFluidMaterial::computeQpProperties()
{
  // Set our variables
  stepInitQpProperties();

  // Default Material method
  Material::computeQpProperties();

  // Compute the terms used in Redback Kernels
  computeRedbackTerms();
}

void
RedbackFluidMaterial::computeRedbackTerms()
{
  _peclet_number[_qp] = _peclet_number_param;
  _reynolds_number[_qp] = _reynolds_number_param;
  _froude_number[_qp] = _froude_number_param;
  _viscosity_ratio[_qp] = _viscosity_ratio_param;

  // Real fluid_density;
  // fluid_density = (1 + _fluid_compressibility_param * _pore_pres[_qp] -
  //                  _fluid_thermal_expansion_param * _T[_qp]);

  // Gravity term in the momentum kernel
  _gravity_term[_qp] = _gravity_param / pow(_froude_number[_qp], 2);

  // Constitutive law
  RankTwoTensor grad_v(_grad_fluid_vel_x[_qp], _grad_fluid_vel_y[_qp], _grad_fluid_vel_z[_qp]);
  // Real second_viscosity = _bulk_viscosity_param + 2*_dynamic_viscosity_param/3.0; //zeta
  _div_fluid_vel[_qp] =
      _grad_fluid_vel_x[_qp](0) + _grad_fluid_vel_y[_qp](1) + _grad_fluid_vel_z[_qp](2);
  // Fluid stress for Newtonian compressible fluid, in small deformation
  _fluid_stress[_qp].zero();
  // Influence of bulk viscosity
  _fluid_stress[_qp].addIa((_viscosity_ratio[_qp] - 2. / 3.) *
                           _div_fluid_vel[_qp]); // - _pore_pres[_qp]);
  _fluid_stress[_qp] += (grad_v + grad_v.transpose());

  //_Jacobian_fluid_mult[_qp].zero();

  // Fluid divergence for mass Kernel component
  if (_fluid_compressibility_param == 0)
  {
    _div_fluid_kernel[_qp] = _div_fluid_vel[_qp] * _peclet_number[_qp];
    _pressurization_coefficient[_qp] = _fluid_thermal_expansion_param;
  }
  else
  {
    _div_fluid_kernel[_qp] =
        _div_fluid_vel[_qp] * _peclet_number[_qp] / _fluid_compressibility_param;
    _pressurization_coefficient[_qp] =
        _fluid_thermal_expansion_param / _fluid_compressibility_param;
  }

  // Assembling mass kernels components
  RealVectorValue fluid_vel_vector =
      RealVectorValue(_fluid_vel_x[_qp], _fluid_vel_y[_qp], _fluid_vel_z[_qp]);
  //_pressure_convective_mass[_qp] = _grad_pore_pressure[_qp]*fluid_vel_vector -
  //_pressurization_coefficient[_qp]*_grad_temp[_qp]*fluid_vel_vector;
  //_thermal_convective_mass[_qp] = _grad_temp[_qp]*fluid_vel_vector;
  //^the sum of these two terms is done in massConvection Kernel
  _pressure_convective_mass[_qp] = fluid_vel_vector * _peclet_number[_qp];
  _thermal_convective_mass[_qp] =
      _pressurization_coefficient[_qp] * fluid_vel_vector * _peclet_number[_qp];

  return;
}
