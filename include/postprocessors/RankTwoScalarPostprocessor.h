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

#ifndef RANKTWOSCALARPOSTPROCESSOR_H
#define RANKTWOSCALARPOSTPROCESSOR_H

#include "GeneralPostprocessor.h"
#include "RankTwoTensor.h"

class RankTwoScalarPostprocessor : public GeneralPostprocessor
{
public:
  RankTwoScalarPostprocessor(const InputParameters & parameters);

  static InputParameters validParams();

  virtual void initialize() override;
  virtual void execute() override;
  virtual PostprocessorValue getValue() const override;

protected:
  const PostprocessorValue & _index00;
  const PostprocessorValue & _index01;
  const PostprocessorValue & _index02;
  const PostprocessorValue & _index10;
  const PostprocessorValue & _index11;
  const PostprocessorValue & _index12;
  const PostprocessorValue & _index20;
  const PostprocessorValue & _index21;
  const PostprocessorValue & _index22;
  MooseEnum _scalar_type;
};

#endif /* RANKTWOSCALARPOSTPROCESSOR_H */
