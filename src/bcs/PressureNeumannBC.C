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

#include "PressureNeumannBC.h"

registerMooseObject("RedbackApp", PressureNeumannBC);

InputParameters
PressureNeumannBC::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  params.addRequiredCoupledVar("coupled_var", "The variable whose value we are to match.");
  params.addRequiredParam<unsigned int>("component",
                                        "An integer corresponding to the direction the variable "
                                        "this BC acts in. (0 for x, 1 for y, 2 for z)");
  params.addParam<Real>("scale_factor", 1.0, "rescaling factor");
  return params;
}

PressureNeumannBC::PressureNeumannBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
    _var(coupledValue("coupled_var")),
    _var_num(coupled("coupled_var")),
    _component(getParam<unsigned int>("component")),
    _scale_factor(getParam<Real>("scale_factor"))
{
}

Real
PressureNeumannBC::computeQpResidual()
{
  return -_var[_qp] * _normals[_qp](_component) * _test[_i][_qp] * _scale_factor;
}

Real
PressureNeumannBC::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _var_num)
    return -_phi[_j][_qp] * _normals[_qp](_component) * _test[_i][_qp] * _scale_factor;

  else
    return 0.;
}
