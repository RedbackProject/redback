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

#ifndef REDBACKVARIABLEEQUALSFUNCTION_H
#define REDBACKVARIABLEEQUALSFUNCTION_H

#include "TimeKernel.h"

class RedbackVariableEqualsFunction : public TimeKernel
{
public:
  RedbackVariableEqualsFunction(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  const Function & _func;
};

#endif // REDBACKVARIABLEEQUALSFUNCTION_H
