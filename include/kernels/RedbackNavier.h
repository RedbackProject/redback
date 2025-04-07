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

#ifndef REDBACKNAVIER_H
#define REDBACKNAVIER_H

#include "Kernel.h"

class RedbackNavier : public Kernel
{
public:
  RedbackNavier(const InputParameters & parameters);
  static InputParameters validParams();

  virtual ~RedbackNavier();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  const unsigned int _component;

  const VariableValue & _fluid_vel_x;
  const VariableValue & _fluid_vel_y;
  const VariableValue & _fluid_vel_z;

  const unsigned int _vel_fluid_x_var;
  const unsigned int _vel_fluid_y_var;
  const unsigned int _vel_fluid_z_var;

private:
  Real _time_factor;
};

#endif /* REDBACKNAVIER_H */
