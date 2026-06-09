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

class RedbackThermalDiffusion : public Kernel
{
public:
  RedbackThermalDiffusion(const InputParameters & parameters);
  static InputParameters validParams();

  virtual ~RedbackThermalDiffusion();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  const MaterialProperty<Real> & _diffusivity;

private:
  Real _time_factor;
};