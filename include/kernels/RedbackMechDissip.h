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

#pragma once

#include "Kernel.h"
#include "RankTwoTensor.h"

class RedbackMechDissip : public Kernel
{
public:
  RedbackMechDissip(const InputParameters & parameters);
  static InputParameters validParams();

  virtual ~RedbackMechDissip();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  // const VariableValue & _pressure;
  const MaterialProperty<Real> & _mechanical_dissipation;
  const MaterialProperty<Real> & _mechanical_dissipation_jac;

private:
  Real _time_factor;
};
