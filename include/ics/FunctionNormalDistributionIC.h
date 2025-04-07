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

#ifndef FUNCTIONNORMALDISTRIBUTIONIC_H
#define FUNCTIONNORMALDISTRIBUTIONIC_H

#include "InitialCondition.h"
#include "InputParameters.h"

// System includes
#include <string>

class Function;
namespace libMesh
{
class Point;
}

/**
 * FunctionNormalDistributionIC just returns a Random value.
 */
class FunctionNormalDistributionIC : public InitialCondition
{
public:
  /**
   * Constructor
   *
   * @param name The name given to the initial condition in the input file.
   * @param parameters The parameters object holding data for the class to use.
   */
  FunctionNormalDistributionIC(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  /**
   * The value of the variable at a point.
   *
   * This must be overridden by derived classes.
   */
  virtual Real value(const Point & p);

  Real _mean;
  Real _stddev;
  Real _min;
  Real _max;
};

#endif // FUNCTIONNORMALDISTRIBUTIONIC_H
