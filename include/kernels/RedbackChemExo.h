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

#ifndef REDBACKCHEMEXO_H
#define REDBACKCHEMEXO_H

#include "Kernel.h"

class RedbackChemExo : public Kernel
{
public:
  RedbackChemExo(const InputParameters & parameters);
  static InputParameters validParams();

  virtual ~RedbackChemExo();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  const MaterialProperty<Real> & _chemical_exothermic_energy;
  const MaterialProperty<Real> & _chemical_exothermic_energy_jac;

private:
  Real _time_factor;
};

#endif /* REDBACKCHEMEXO_H */
