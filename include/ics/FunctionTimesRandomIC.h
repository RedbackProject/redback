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

#ifndef FUNCTIONTIMESRANDOMIC_H
#define FUNCTIONTIMESRANDOMIC_H

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
 * FunctionTimesRandomIC just returns a Random value.
 */
class FunctionTimesRandomIC : public InitialCondition
{
public:
  /**
   * Constructor
   *
   * @param name The name given to the initial condition in the input file.
   * @param parameters The parameters object holding data for the class to use.
   */
  FunctionTimesRandomIC(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  /**
   * Evaluate the function at the current quadrature point and timestep.
   */
  Real f();
  /**
   * The value of the variable at a point.
   *
   * This must be overridden by derived classes.
   */
  virtual Real value(const Point & p);

  Real _min;
  Real _max;
  Real _range;
  const Function & _func;
};

#endif // FUNCTIONTIMESRANDOMIC_H
