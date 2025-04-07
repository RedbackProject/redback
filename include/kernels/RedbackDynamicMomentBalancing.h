/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#ifndef RedbackDynamicMomentBalancing_H
#define RedbackDynamicMomentBalancing_H
#include "RedbackMomentBalancing.h"


class RedbackDynamicMomentBalancing : public RedbackMomentBalancing
{
public:
  RedbackDynamicMomentBalancing(const InputParameters & parameters);
  static InputParameters validParams();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  const MaterialProperty<RankTwoTensor> & _stress_older;
  const MaterialProperty<RankTwoTensor> & _stress_old;

  // Rayleigh damping parameter _zeta and HHT time integration parameter _alpha
  const Real _zeta;
  const Real _alpha;

};

#endif //RedbackDynamicMomentBalancing_H
