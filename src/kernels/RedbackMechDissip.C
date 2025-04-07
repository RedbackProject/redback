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

#include "RedbackMechDissip.h"

registerMooseObject("RedbackApp", RedbackMechDissip);

InputParameters
RedbackMechDissip::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addParam<Real>("time_factor", 1.0, "Time rescaling factor (global parameter!)");

  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

RedbackMechDissip::RedbackMechDissip(const InputParameters & parameters)
  : Kernel(parameters),
    _mechanical_dissipation(hasMaterialProperty<Real>("mechanical_dissipation_mech")
                                ? getMaterialProperty<Real>("mechanical_dissipation_mech")
                                : getMaterialProperty<Real>("mechanical_dissipation_no_mech")),
    _mechanical_dissipation_jac(
        hasMaterialProperty<Real>("mechanical_dissipation_jacobian_mech")
            ? getMaterialProperty<Real>("mechanical_dissipation_jacobian_mech")
            : getMaterialProperty<Real>("mechanical_dissipation_jacobian_no_mech")),
    _time_factor(getParam<Real>("time_factor"))
{
}

RedbackMechDissip::~RedbackMechDissip() {}

Real
RedbackMechDissip::computeQpResidual()
{
  return -_time_factor * _test[_i][_qp] * _mechanical_dissipation[_qp];
}

Real
RedbackMechDissip::computeQpJacobian()
{
  return -_time_factor * _test[_i][_qp] * _mechanical_dissipation_jac[_qp] * _phi[_j][_qp];
}