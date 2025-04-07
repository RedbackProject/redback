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

#include "RedbackPoromechanics.h"

registerMooseObject("RedbackApp", RedbackPoromechanics);

InputParameters
RedbackPoromechanics::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addCoupledVar("temperature", 0.0, "Temperature variable.");
  params.addParam<Real>("time_factor", 1.0, "Time rescaling factor (global parameter!)");

  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

RedbackPoromechanics::RedbackPoromechanics(const InputParameters & parameters)
  : Kernel(parameters),
    _poromech_kernel(getMaterialProperty<Real>("poromechanics_kernel")),
    _poromech_jac(getMaterialProperty<Real>("poromechanics_jacobian")),
    _temp_var(coupled("temperature")),
    _time_factor(getParam<Real>("time_factor"))
{
}

RedbackPoromechanics::~RedbackPoromechanics() {}

Real
RedbackPoromechanics::computeQpResidual()
{
  return _time_factor * _poromech_kernel[_qp] * _test[_i][_qp];
  // TODO: add note in doco to tell users not to turn on term if compress=0
  // TODO: add check compress > 0
}

Real
RedbackPoromechanics::computeQpJacobian()
{
  return 0;
}

Real
RedbackPoromechanics::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _temp_var)
  {
    return _time_factor * _poromech_kernel[_qp] * _poromech_jac[_qp] * _phi[_j][_qp] *
           _test[_i][_qp];
  }
  return 0;
}
