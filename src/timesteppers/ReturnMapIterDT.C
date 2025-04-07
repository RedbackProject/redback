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

#include "ReturnMapIterDT.h"

registerMooseObject("RedbackApp", ReturnMapIterDT);

InputParameters
ReturnMapIterDT::validParams()
{
  InputParameters params = TimeStepper::validParams();
  params.addRequiredParam<PostprocessorName>(
      "postprocessor", "This has to be an ElementExtremeValue postprocessor.");
  params.addParam<Real>("dt", "Initial value of dt");
  params.addRequiredParam<Real>("max_iter", "Maximum of return map iteration that are admissible.");
  params.addRequiredParam<Real>("min_iter", "Minimum of return map iteration that are admissible.");
  params.addRequiredParam<Real>("ratio", "Ratio by which current time step is multiplied.");
  params.addRequiredParam<Real>("dt_min", "Minimum time step.");
  params.addRequiredParam<Real>("dt_max", "Maximum time step.");
  return params;
}

ReturnMapIterDT::ReturnMapIterDT(const InputParameters & parameters)
  : TimeStepper(parameters),
    PostprocessorInterface(this),
    _extreme_value(getPostprocessorValue("postprocessor")),
    _has_initial_dt(isParamValid("dt")),
    _initial_dt(_has_initial_dt ? getParam<Real>("dt") : 0.),
    _max_iter(getParam<Real>("max_iter")),
    _min_iter(getParam<Real>("min_iter")),
    _ratio(getParam<Real>("ratio")),
    _dt_min(getParam<Real>("dt_min")),
    _dt_max(getParam<Real>("dt_max"))
{
}

Real
ReturnMapIterDT::computeInitialDT()
{
  if (_has_initial_dt)
    return _initial_dt;
  else
    return computeDT();
}

Real
ReturnMapIterDT::computeDT()
{
  Real dt_new;
  if (_extreme_value > _max_iter)
  {
    dt_new = getCurrentDT() * _ratio;
    return (dt_new > _dt_min) ? dt_new : _dt_min;
  }
  else if (_extreme_value < _min_iter)
  {
    dt_new = getCurrentDT() / _ratio;
    return (dt_new < _dt_max) ? dt_new : _dt_max;
  }
  else
    return getCurrentDT();
}
