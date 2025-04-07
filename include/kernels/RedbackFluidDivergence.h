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

#ifndef REDBACKFLUIDDIVERGENCE_H
#define REDBACKFLUIDDIVERGENCE_H

#include "Kernel.h"

class RedbackFluidDivergence : public Kernel
{
public:
  RedbackFluidDivergence(const InputParameters & parameters);
  static InputParameters validParams();

  virtual ~RedbackFluidDivergence();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned jvar);

  const MaterialProperty<Real> & _div_fluid_kernel;

private:
  // Real _time_factor;

  // Variable numberings
  unsigned _x_vel_var_number;
  unsigned _y_vel_var_number;
  unsigned _z_vel_var_number;
};

#endif /* REDBACKFLUIDDIVERGENCE_H */
