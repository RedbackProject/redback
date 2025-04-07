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

#ifndef REDBACKTHERMALCONVECTION_H
#define REDBACKTHERMALCONVECTION_H

#include "Kernel.h"

class RedbackThermalConvection : public Kernel
{
public:
  RedbackThermalConvection(const InputParameters & parameters);
  static InputParameters validParams();

  virtual ~RedbackThermalConvection() {}

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  const MaterialProperty<RealVectorValue> & _mixture_convective_energy;
  // MaterialProperty<Real> & _mixture_convective_energy_jac;
  // MaterialProperty<Real> & _mixture_convective_energy_off_jac;

  unsigned int _pore_pres_var; // variable number of the pore pressure variable

private:
  Real _time_factor;
};

#endif // REDBACKTHERMALCONVECTION_H
