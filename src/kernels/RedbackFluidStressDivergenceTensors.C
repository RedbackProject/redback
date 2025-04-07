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

#include "RedbackFluidStressDivergenceTensors.h"

#include "ElasticityTensorTools.h"
#include "Material.h"

registerMooseObject("RedbackApp", RedbackFluidStressDivergenceTensors);

InputParameters
RedbackFluidStressDivergenceTensors::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addRequiredParam<unsigned int>("component",
                                        "An integer corresponding to the direction the variable "
                                        "this kernel acts in. (0 for x, 1 for y, 2 for z)");
  params.addRequiredCoupledVar("fluid_vel_x", "The x-component of fluid velocity");
  params.addRequiredCoupledVar("fluid_vel_y", "The y-component of fluid velocity");
  params.addCoupledVar("fluid_vel_z", 0.0, "The z-component of fluid velocity");
  // params.addRequiredCoupledVar("temp", 0.0, "The temperature");
  params.addRequiredCoupledVar("pore_pres", "The pore fluid pressure");
  params.addParam<bool>("stokes", false, "simplifies system to stokes");

  // Using the displaced mesh will be set in the solid mechanics action input now.
  params.set<bool>("use_displaced_mesh") = true;

  return params;
}

RedbackFluidStressDivergenceTensors::RedbackFluidStressDivergenceTensors(
    const InputParameters & parameters)
  : Kernel(parameters),
    //_pore_pres(coupledValue("pore_pres")),
    _grad_pore_pressure(coupledGradient("pore_pres")),
    _fluid_stress(getMaterialProperty<RankTwoTensor>("fluid_stress")),
    //_Jacobian_fluid_mult(getMaterialProperty<RankFourTensor>("Jacobian_fluid_mult")),
    _component(getParam<unsigned int>("component")),
    _fluid_density(getMaterialProperty<Real>("NS_fluid_density")),

    _reynolds_number(getMaterialProperty<Real>("Reynolds_number")),
    _viscosity_ratio(getMaterialProperty<Real>("viscosity_ratio")),

    //_vel_fluid_x_coupled(isCoupled("fluid_vel_x")),
    //_vel_fluid_y_coupled(isCoupled("fluid_vel_y")),
    //_vel_fluid_coupled(isCoupled("fluid_vel_z")),
    //_temp_coupled(isCoupled("temp")),

    _vel_fluid_x_var(coupled("fluid_vel_x")),
    _vel_fluid_y_var(coupled("fluid_vel_y")),
    _vel_fluid_z_var(coupled("fluid_vel_z")),
    //_temp_var(coupled("temp")),
    _porepressure_var(coupled("pore_pres")),

    _gravity_term(getMaterialProperty<RealVectorValue>("gravity_term")),

    _stokes(getParam<bool>("stokes"))
{
}

Real
RedbackFluidStressDivergenceTensors::computeQpResidual()
{
  if (_stokes)
    return _grad_u[_qp] * _grad_test[_i][_qp] / _reynolds_number[_qp] +
           _grad_pore_pressure[_qp](_component) * _test[_i][_qp];
  else
    // density does not disappear because it depends of pressure and temperature
    return (_fluid_stress[_qp].row(_component)) * _grad_test[_i][_qp] /
               (_reynolds_number[_qp] * _fluid_density[_qp]) +
           _test[_i][_qp] *
               // gradient of pressure * test, no integration by parts to be able to have
               // DirichletBC of P
               (_grad_pore_pressure[_qp](_component) / _fluid_density[_qp] -
                _gravity_term[_qp](_component));
}

Real
RedbackFluidStressDivergenceTensors::computeQpJacobian()
{
  if (_stokes)
    return 1. / _reynolds_number[_qp] * _grad_phi[_j][_qp] * _grad_test[_i][_qp];
  else
    return 1. / (_reynolds_number[_qp] * _fluid_density[_qp]) *
           (_grad_phi[_j][_qp] * _grad_test[_i][_qp] +
            // 1+(lambda-2/3) both are influencing
            // comes from the gradu_transpose + volumetric expansion term in the stress
            (1. / 3. + _viscosity_ratio[_qp]) * _grad_phi[_j][_qp](_component) *
                _grad_test[_i][_qp](_component));
}

Real
RedbackFluidStressDivergenceTensors::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (_stokes)
    if (jvar == _porepressure_var)
      return _grad_phi[_j][_qp](_component) * _test[_i][_qp];
    else
      return 0;
  else if (jvar == _vel_fluid_x_var)
    // this comes from the volumetric expansion term in the stress
    return (-2. / 3. + _viscosity_ratio[_qp]) / (_reynolds_number[_qp] * _fluid_density[_qp]) *
               _grad_phi[_j][_qp](0) * _grad_test[_i][_qp](_component) +
           // this comes from the gradu_transpose in the stress
           1. / (_reynolds_number[_qp] * _fluid_density[_qp]) * _grad_phi[_j][_qp](_component) *
               _grad_test[_i][_qp](0);
  else if (jvar == _vel_fluid_y_var)
    return (-2. / 3. + _viscosity_ratio[_qp]) / (_reynolds_number[_qp] * _fluid_density[_qp]) *
               _grad_phi[_j][_qp](1) * _grad_test[_i][_qp](_component) +
           1. / (_reynolds_number[_qp] * _fluid_density[_qp]) * _grad_phi[_j][_qp](_component) *
               _grad_test[_i][_qp](1);
  else if (jvar == _vel_fluid_z_var)
    return (-2. / 3. + _viscosity_ratio[_qp]) / (_reynolds_number[_qp] * _fluid_density[_qp]) *
               _grad_phi[_j][_qp](_component) * _grad_test[_i][_qp](2) +
           1. / (_reynolds_number[_qp] * _fluid_density[_qp]) * _grad_phi[_j][_qp](_component) *
               _grad_test[_i][_qp](2);
  // Porepressure term
  else if (jvar == _porepressure_var)
    return _grad_phi[_j][_qp](_component) * _test[_i][_qp] / _fluid_density[_qp];

  else
    return 0;
}
