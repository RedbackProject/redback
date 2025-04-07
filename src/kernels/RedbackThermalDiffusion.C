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

#include "RedbackThermalDiffusion.h"

registerMooseObject("RedbackApp", RedbackThermalDiffusion);

InputParameters
RedbackThermalDiffusion::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addParam<Real>("time_factor", 1.0, "Time rescaling factor (global parameter!)");

  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

RedbackThermalDiffusion::RedbackThermalDiffusion(const InputParameters & parameters)
  : Kernel(parameters),
    _diffusivity(getMaterialProperty<Real>("thermal_diffusivity")),
    _time_factor(getParam<Real>("time_factor"))
{
}

RedbackThermalDiffusion::~RedbackThermalDiffusion() {}

Real
RedbackThermalDiffusion::computeQpResidual()
{
  return _time_factor * _diffusivity[_qp] * _grad_u[_qp] * _grad_test[_i][_qp];
}

Real
RedbackThermalDiffusion::computeQpJacobian()
{
  return _time_factor * _diffusivity[_qp] * _grad_phi[_j][_qp] * _grad_test[_i][_qp];
}
