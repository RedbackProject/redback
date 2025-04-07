/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#ifndef REDBACKINERTIALFORCE_H
#define REDBACKINERTIALFORCE_H

#include "Kernel.h"
#include "Material.h"

class RedbackInertialForce : public Kernel
{
public:

  RedbackInertialForce(const InputParameters & parameters);
  static InputParameters validParams();

protected:
  virtual Real computeQpResidual();

  virtual Real computeQpJacobian();

private:
  const MaterialProperty<Real> & _density;
  const VariableValue & _u_old;
  const VariableValue & _vel_old;
  const VariableValue & _accel_old;
  const Real _beta;
  const Real _gamma;
  const Real _eta;
  const Real _alpha;
  const Real _microinertia;

};

#endif //RedbackInertialForce_H
