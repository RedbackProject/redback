/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#ifndef RANKTWODOUBLECONTRACTION_H
#define RANKTWODOUBLECONTRACTION_H

#include "AuxKernel.h"
#include "RankTwoTensor.h"

class RankTwoDoubleContraction;

class RankTwoDoubleContraction : public AuxKernel
{
public:
  RankTwoDoubleContraction(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeValue();

  const MaterialProperty<RankTwoTensor> & _tensor1;
  const MaterialProperty<RankTwoTensor> & _tensor2;
};

#endif // RANKTWODOUBLECONTRACTION_H
