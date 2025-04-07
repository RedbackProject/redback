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

#ifndef REDBACKRANDOMFUNCTION
#define REDBACKRANDOMFUNCTION

#include "Function.h"

/**
 * Defines random function
 */
class RedbackRandomFunction : public Function
{
public:
  RedbackRandomFunction(const InputParameters & parameters);

  static InputParameters validParams();

  virtual Real value(Real t, const Point & p) const;

protected:
  Real _min;
  Real _max;
  Real _range;
};

#endif // REDBACKRANDOMFUNCTION
