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

#include "RedbackDamage.h"

registerMooseObject("RedbackApp", RedbackDamage);

InputParameters
RedbackDamage::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addParam<Real>("time_factor", 1.0, "Time rescaling factor (global parameter!)");

  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

RedbackDamage::RedbackDamage(const InputParameters & parameters)
  : Kernel(parameters),
    _damage_kernel(getMaterialProperty<Real>("damage_kernel")),
    _damage_kernel_jac(getMaterialProperty<Real>("damage_kernel_jacobian")),
    _time_factor(getParam<Real>("time_factor"))
{
}

RedbackDamage::~RedbackDamage() {}

Real
RedbackDamage::computeQpResidual()
{
  return -_time_factor * _test[_i][_qp] * _damage_kernel[_qp];
}

Real
RedbackDamage::computeQpJacobian()
{
  return -_time_factor * _test[_i][_qp] * _damage_kernel_jac[_qp] * _phi[_j][_qp];
}
