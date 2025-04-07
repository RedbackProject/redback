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

#ifndef REDBACKCHEMPRESSURE_H
#define REDBACKCHEMPRESSURE_H

#include "Kernel.h"

class RedbackChemPressure : public Kernel
{
public:
  RedbackChemPressure(const InputParameters & parameters);
  static InputParameters validParams();

  virtual ~RedbackChemPressure();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  // const VariableValue & _temp;
  const MaterialProperty<Real> & _chemical_source_mass;
  const MaterialProperty<Real> & _chemical_source_mass_jac;

  unsigned int _temp_var; // variable number of the temperature variable

private:
  Real _time_factor;
};

#endif /* REDBACKCHEMPRESSURE_H */
