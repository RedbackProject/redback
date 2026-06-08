/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "RankTwoDoubleContraction.h"

registerMooseObject("RedbackApp", RankTwoDoubleContraction);

InputParameters
RankTwoDoubleContraction::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("Compute a tensorial product of two RankTwoTensors");
  params.addRequiredParam<MaterialPropertyName>("rank_two_tensor1", "The first rank two material tensor name");
  params.addRequiredParam<MaterialPropertyName>("rank_two_tensor2", "The second rank two material tensor name");
  return params;
}

RankTwoDoubleContraction::RankTwoDoubleContraction(const InputParameters & parameters) :
    AuxKernel(parameters),
    _tensor1(getMaterialProperty<RankTwoTensor>("rank_two_tensor1")),
    _tensor2(getMaterialProperty<RankTwoTensor>("rank_two_tensor2"))
{
}

Real
RankTwoDoubleContraction::computeValue()
{
  return _tensor1[ _qp ].doubleContraction(_tensor2[ _qp ]);
}
