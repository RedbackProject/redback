/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/
#include "RankTwoContractionPostprocessor.h"

registerMooseObject("RedbackApp", RankTwoContractionPostprocessor);

InputParameters
RankTwoContractionPostprocessor::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  params.addRequiredParam<PostprocessorName>("1_index00", "First tensor's element");
  params.addRequiredParam<PostprocessorName>("1_index01", "First tensor's element");
  params.addRequiredParam<PostprocessorName>("1_index02", "First tensor's element");
  params.addRequiredParam<PostprocessorName>("1_index10", "First tensor's element");
  params.addRequiredParam<PostprocessorName>("1_index11", "First tensor's element");
  params.addRequiredParam<PostprocessorName>("1_index12", "First tensor's element");
  params.addRequiredParam<PostprocessorName>("1_index20", "First tensor's element");
  params.addRequiredParam<PostprocessorName>("1_index21", "First tensor's element");
  params.addRequiredParam<PostprocessorName>("1_index22", "First tensor's element");

  params.addRequiredParam<PostprocessorName>("2_index00", "Second tensor's element");
  params.addRequiredParam<PostprocessorName>("2_index01", "Second tensor's element");
  params.addRequiredParam<PostprocessorName>("2_index02", "Second tensor's element");
  params.addRequiredParam<PostprocessorName>("2_index10", "Second tensor's element");
  params.addRequiredParam<PostprocessorName>("2_index11", "Second tensor's element");
  params.addRequiredParam<PostprocessorName>("2_index12", "Second tensor's element");
  params.addRequiredParam<PostprocessorName>("2_index20", "Second tensor's element");
  params.addRequiredParam<PostprocessorName>("2_index21", "Second tensor's element");
  params.addRequiredParam<PostprocessorName>("2_index22", "Second tensor's element");

  return params;
}

RankTwoContractionPostprocessor::RankTwoContractionPostprocessor(const InputParameters & parameters) :
    GeneralPostprocessor(parameters),
    _1_index00(getPostprocessorValue("1_index00")),
    _1_index01(getPostprocessorValue("1_index01")),
    _1_index02(getPostprocessorValue("1_index02")),
    _1_index10(getPostprocessorValue("1_index10")),
    _1_index11(getPostprocessorValue("1_index11")),
    _1_index12(getPostprocessorValue("1_index12")),
    _1_index20(getPostprocessorValue("1_index20")),
    _1_index21(getPostprocessorValue("1_index21")),
    _1_index22(getPostprocessorValue("1_index22")),

    _2_index00(getPostprocessorValue("2_index00")),
    _2_index01(getPostprocessorValue("2_index01")),
    _2_index02(getPostprocessorValue("2_index02")),
    _2_index10(getPostprocessorValue("2_index10")),
    _2_index11(getPostprocessorValue("2_index11")),
    _2_index12(getPostprocessorValue("2_index12")),
    _2_index20(getPostprocessorValue("2_index20")),
    _2_index21(getPostprocessorValue("2_index21")),
    _2_index22(getPostprocessorValue("2_index22"))
{
}

void
RankTwoContractionPostprocessor::initialize()
{
}

void
RankTwoContractionPostprocessor::execute()
{
}

PostprocessorValue
RankTwoContractionPostprocessor::getValue() const
{
  RankTwoTensor tensor_1 = RankTwoTensor(
    _1_index00, _1_index01, _1_index02, _1_index10, _1_index11, _1_index12, _1_index20, _1_index21, _1_index22);
  RankTwoTensor tensor_2 = RankTwoTensor(
    _2_index00, _2_index01, _2_index02, _2_index10, _2_index11, _2_index12, _2_index20, _2_index21, _2_index22);
  return tensor_1.doubleContraction(tensor_2);
}
