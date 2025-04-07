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

#include "RedbackMassDiffusion.h"

registerMooseObject("RedbackApp", RedbackMassDiffusion);

InputParameters
RedbackMassDiffusion::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addParam<Real>("time_factor", 1.0, "Time rescaling factor (global parameter!)");

  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

RedbackMassDiffusion::RedbackMassDiffusion(const InputParameters & parameters)
  : Kernel(parameters),
    _Le(getMaterialProperty<Real>("lewis_number")),
    _gravity_term(getMaterialProperty<RealVectorValue>("fluid_gravity_term")),
    _time_factor(getParam<Real>("time_factor"))
{
}

RedbackMassDiffusion::~RedbackMassDiffusion() {}

Real
RedbackMassDiffusion::computeQpResidual()
{
  return (_time_factor / _Le[_qp]) * (_grad_u[_qp] - _gravity_term[_qp]) * _grad_test[_i][_qp];
}

Real
RedbackMassDiffusion::computeQpJacobian()
{
  return (_time_factor / _Le[_qp]) * _grad_phi[_j][_qp] * _grad_test[_i][_qp];
}
