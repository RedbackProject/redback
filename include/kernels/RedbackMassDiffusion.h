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

#ifndef REDBACKMASSDIFFUSION_H
#define REDBACKMASSDIFFUSION_H

#include "Kernel.h"

class RedbackMassDiffusion : public Kernel
{
public:
  RedbackMassDiffusion(const InputParameters & parameters);
  static InputParameters validParams();

  virtual ~RedbackMassDiffusion();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  //  const VariableValue & _T;

  const MaterialProperty<Real> & _Le;
  const MaterialProperty<RealVectorValue> & _gravity_term;

private:
  Real _time_factor;
};

#endif /* REDBACKMASSDIFFUSION_H */
