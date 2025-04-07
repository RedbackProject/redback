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
#include "FunctionWithRandomIC.h"
#include "MooseRandom.h"

#include "libmesh/point.h"

registerMooseObject("RedbackApp", FunctionWithRandomIC);

InputParameters
FunctionWithRandomIC::validParams()
{
  InputParameters params = InitialCondition::validParams();
  params.addParam<Real>("min", 0.0, "Lower bound of the randomly generated values");
  params.addParam<Real>("max", 1.0, "Upper bound of the randomly generated values");
  params.addParam<unsigned int>("seed", 0, "Seed value for the random number generator");
  params.addRequiredParam<FunctionName>("function",
                                        "The initial condition function (without randomness).");
  return params;
}

FunctionWithRandomIC::FunctionWithRandomIC(const InputParameters & parameters)
  : InitialCondition(parameters),
    _min(getParam<Real>("min")),
    _max(getParam<Real>("max")),
    _range(_max - _min),
    _func(getFunction("function"))
{
  mooseAssert(_range > 0.0, "Min > Max for FunctionWithRandomIC!");
  MooseRandom::seed(getParam<unsigned int>("seed"));
}

Real
FunctionWithRandomIC::value(const Point & p)
{
  // Random number between 0 and 1
  Real rand_num = MooseRandom::rand();

  // Between 0 and range
  rand_num *= _range;

  // Between min and max
  rand_num += _min;

  return rand_num + _func.value(_t, p);
}
