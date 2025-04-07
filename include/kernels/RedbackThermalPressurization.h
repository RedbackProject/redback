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

#ifndef REDBACKTHERMALPRESSURIZATION_H
#define REDBACKTHERMALPRESSURIZATION_H

#include "Kernel.h"

/**
 * Kernel that is calling coupledDot
 */
class RedbackThermalPressurization : public Kernel
{
public:
  RedbackThermalPressurization(const InputParameters & parameters);
  static InputParameters validParams();

  virtual ~RedbackThermalPressurization();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  const VariableValue & _temp_dot;
  const VariableValue & _dtemp_dot_dtemp;

  const MaterialProperty<Real> & _pressurization_coefficient;
  unsigned int _temp_var; // variable number of the temperature variable

private:
  Real _time_factor;
};

#endif /* REDBACKTHERMALPRESSURIZATION_H */
