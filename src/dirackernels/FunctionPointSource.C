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
#include "FunctionPointSource.h"

registerMooseObject("RedbackApp", FunctionPointSource);

InputParameters
FunctionPointSource::validParams()
{
  InputParameters params = DiracKernel::validParams();
  params.addRequiredParam<FunctionName>("function", "The input function (in time).");
  params.addRequiredParam<std::vector<Real>>("point", "The x,y,z coordinates of the point");
  return params;
}

FunctionPointSource::FunctionPointSource(const InputParameters & parameters)
  : DiracKernel(parameters),
    _func(getFunction("function")),
    _point_param(getParam<std::vector<Real>>("point"))
{
  _p(0) = _point_param[0];

  if (_point_param.size() > 1)
  {
    _p(1) = _point_param[1];

    if (_point_param.size() > 2)
    {
      _p(2) = _point_param[2];
    }
  }
}

void
FunctionPointSource::addPoints()
{
  addPoint(_p);
}

Real
FunctionPointSource::f()
{
  return _func.value(_t, _p);
}

Real
FunctionPointSource::computeQpResidual()
{
  return -_test[_i][_qp] * f(); //*_value;
}
