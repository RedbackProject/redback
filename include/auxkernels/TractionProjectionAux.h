/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#ifndef TRACTIONPROJECTIONAUX_H
#define TRACTIONPROJECTIONAUX_H

#include "AuxKernel.h"
#include "RankTwoTensor.h"

class TractionProjectionAux : public AuxKernel
{
public:
  TractionProjectionAux(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeValue();

  const MaterialProperty<RankTwoTensor> & _tensor;
  bool _normal;
  bool _has_pres;
  const VariableValue & _pressure;

private:
  Real _angle;
};

#endif // TRACTIONPROJECTIONAUX_H
