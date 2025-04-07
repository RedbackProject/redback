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
#include "FunctionNormalDistributionIC.h"
#include "MooseRandom.h"

#include "libmesh/point.h"

registerMooseObject("RedbackApp", FunctionNormalDistributionIC);

InputParameters
FunctionNormalDistributionIC::validParams()
{
  InputParameters params = InitialCondition::validParams();
  params.addRequiredParam<Real>("mean", "Mean value of normal distribution function");
  params.addRequiredParam<Real>("standard_deviation",
                                "Standard deviation of normal distribution function");
  params.addParam<unsigned int>("seed", 0, "Seed value for the random number generator");
  params.addParam<Real>(
      "minimum", -1e99, "minimum value (if you want to cap the range of available values)");
  params.addParam<Real>(
      "maximum", 1e99, "minimum value (if you want to cap the range of available values)");
  return params;
}

FunctionNormalDistributionIC::FunctionNormalDistributionIC(const InputParameters & parameters)
  : InitialCondition(parameters),
    _mean(getParam<Real>("mean")),
    _stddev(getParam<Real>("standard_deviation")),
    _min(getParam<Real>("minimum")),
    _max(getParam<Real>("maximum"))
{
  mooseAssert(_stddev > 0.0,
              "standard_deviation must be positive for FunctionNormalDistributionIC!");
  // Real range = _max - _min;
  // mooseAssert(range >= 0.0, "The maximum value must be greater than the minimum in
  // FunctionNormalDistributionIC");
  MooseRandom::seed(getParam<unsigned int>("seed"));
}

Real
FunctionNormalDistributionIC::value(const Point & /*p*/)
{
  Real result = MooseRandom::randNormal(_mean, _stddev);
  return fmax(_min, fmin(_max, result));
}
