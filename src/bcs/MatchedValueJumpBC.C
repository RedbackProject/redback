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

#include "MatchedValueJumpBC.h"

registerMooseObject("RedbackApp", MatchedValueJumpBC);

InputParameters
MatchedValueJumpBC::validParams()
{
  InputParameters params = NodalBC::validParams();
  params.addRequiredCoupledVar("v", "The variable whose value we are to match.");
  params.addClassDescription("Implements a NodalBC which equates two different Variables' values "
                             "on a specified boundary.");
  params.addParam<PostprocessorName>("tangent_jump", 0, "jump value on the interface");
  params.addRequiredParam<Real>("fault_angle", "The angle of the fault in degrees");
  params.addParam<bool>("convert_to_radians",
                        false,
                        "If true, the value you entered will be "
                        "multiplied by Pi/180");
  params.addRequiredRangeCheckedParam<unsigned int>(
      "component",
      "component >= 0 & component <= 2",
      "An integer corresponding to the direction the variable "
      "this kernel acts in. (0 for x, 1 for y, 2 for z)");
  return params;
}

MatchedValueJumpBC::MatchedValueJumpBC(const InputParameters & parameters)
  : NodalBC(parameters),
    _v(coupledValue("v")),
    _v_num(coupled("v")),
    _jump(getPostprocessorValue("tangent_jump")),
    _angle(getParam<bool>("convert_to_radians") ? getParam<Real>("fault_angle") * M_PI / 180.0
                                                : getParam<Real>("fault_angle")),
    _component(getParam<unsigned int>("component"))
{
}

Real
MatchedValueJumpBC::computeQpResidual()
{
  RealVectorValue fault_tangent(cos(_angle), sin(_angle)); // only works for linear fault
  return _u[_qp] - _v[_qp] + _jump * fault_tangent(_component);
}

Real
MatchedValueJumpBC::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _v_num)
    return -1.0;
  else
    return 0.;
}
