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

#include "Function.h"
#include "RedbackVariableEqualsFunction.h"

registerMooseObject("RedbackApp", RedbackVariableEqualsFunction);

InputParameters
RedbackVariableEqualsFunction::validParams()
{
  InputParameters params = TimeKernel::validParams();
  params.addRequiredParam<FunctionName>("function",
                                        "The function to assign values to the variable.");
  return params;
}

RedbackVariableEqualsFunction::RedbackVariableEqualsFunction(const InputParameters & parameters)
  : TimeKernel(parameters), _func(getFunction("function"))
{
}

Real
RedbackVariableEqualsFunction::computeQpResidual()
{
  return _test[_i][_qp] * (_u[_qp] - _func.value(_t, _q_point[_qp]));
}

Real
RedbackVariableEqualsFunction::computeQpJacobian()
{
  return _test[_i][_qp] * 1 * _phi[_j][_qp];
}
