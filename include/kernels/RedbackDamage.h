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

#ifndef REDBACKDAMAGE_H
#define REDBACKDAMAGE_H

#include "Kernel.h"
#include "RankTwoTensor.h"

class RedbackDamage : public Kernel
{
public:
  RedbackDamage(const InputParameters & parameters);
  static InputParameters validParams();

  virtual ~RedbackDamage();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  // const VariableValue & _pressure;
  const MaterialProperty<Real> & _damage_kernel;
  const MaterialProperty<Real> & _damage_kernel_jac;

private:
  Real _time_factor;
};

#endif /* REDBACKDAMAGE_H */
