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
#ifndef RANKTWOCONTRACTIONPOSTPROCESSOR_H
#define RANKTWOCONTRACTIONPOSTPROCESSOR_H

#include "GeneralPostprocessor.h"
#include "RankTwoTensor.h"

class RankTwoContractionPostprocessor : public GeneralPostprocessor
{
public:
  RankTwoContractionPostprocessor(const InputParameters & parameters);
  static InputParameters validParams();

  virtual void initialize() override;
  virtual void execute() override;
  virtual PostprocessorValue getValue() const override;

protected:
  const PostprocessorValue & _1_index00;
  const PostprocessorValue & _1_index01;
  const PostprocessorValue & _1_index02;
  const PostprocessorValue & _1_index10;
  const PostprocessorValue & _1_index11;
  const PostprocessorValue & _1_index12;
  const PostprocessorValue & _1_index20;
  const PostprocessorValue & _1_index21;
  const PostprocessorValue & _1_index22;

  const PostprocessorValue & _2_index00;
  const PostprocessorValue & _2_index01;
  const PostprocessorValue & _2_index02;
  const PostprocessorValue & _2_index10;
  const PostprocessorValue & _2_index11;
  const PostprocessorValue & _2_index12;
  const PostprocessorValue & _2_index20;
  const PostprocessorValue & _2_index21;
  const PostprocessorValue & _2_index22;
};

#endif /* RANKTWOCONTRACTIONPOSTPROCESSOR_H */
