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

#include "RedbackThermalPressurization.h"

registerMooseObject("RedbackApp", RedbackThermalPressurization);

InputParameters
RedbackThermalPressurization::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addCoupledVar(
      "temperature", 0.0, "Temperature variable."); // TODO: check "required" fields across redback
  params.addParam<Real>("time_factor", 1.0, "Time rescaling factor (global parameter!)");

  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

RedbackThermalPressurization::RedbackThermalPressurization(const InputParameters & parameters)
  : Kernel(parameters),
    _temp_dot(coupledDot("temperature")),
    _dtemp_dot_dtemp(coupledDotDu("temperature")),
    _pressurization_coefficient(getMaterialProperty<Real>("pressurization_coefficient")),
    _temp_var(coupled("temperature")),
    _time_factor(getParam<Real>("time_factor"))
{
}

RedbackThermalPressurization::~RedbackThermalPressurization() {}

Real
RedbackThermalPressurization::computeQpResidual()
{
  return -_time_factor * _pressurization_coefficient[_qp] * _temp_dot[_qp] * _test[_i][_qp];
}

Real
RedbackThermalPressurization::computeQpJacobian()
{
  return 0;
}

Real
RedbackThermalPressurization::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _temp_var)
  {
    return -_time_factor * _pressurization_coefficient[_qp] * _dtemp_dot_dtemp[_qp] *
           _phi[_j][_qp] * _test[_i][_qp];
  }
  return 0;
}
