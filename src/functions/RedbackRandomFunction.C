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

#include "MooseRandom.h"
#include "RedbackRandomFunction.h"

registerMooseObject("RedbackApp", RedbackRandomFunction);

InputParameters
RedbackRandomFunction::validParams()
{
  InputParameters params = Function::validParams();
  params.addParam<Real>("min", 0.0, "Lower bound of the randomly generated values");
  params.addParam<Real>("max", 1.0, "Upper bound of the randomly generated values");
  params.addParam<unsigned int>("seed", 0, "Seed value for the random number generator");
  params.addClassDescription("This function assigns random values between min and max provided.");
  return params;
}

RedbackRandomFunction::RedbackRandomFunction(const InputParameters & parameters)
  : Function(parameters),
    _min(getParam<Real>("min")),
    _max(getParam<Real>("max")),
    _range(_max - _min)
{
  mooseAssert(_range > 0.0, "Min > Max for FunctionWithRandomIC!");
  MooseRandom::seed(getParam<unsigned int>("seed"));
}

Real
RedbackRandomFunction::value(Real /*t*/, const Point & /*p*/) const
{
  // Random number between 0 and 1
  Real rand_num = MooseRandom::rand();

  // Between 0 and range
  rand_num *= _range;

  // Between min and max
  rand_num += _min;

  return rand_num;
}
