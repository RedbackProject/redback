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

#pragma once

#include "AuxKernel.h"
#include "RankTwoTensor.h"

class RedbackDiffVarsAux : public AuxKernel
{
public:
  RedbackDiffVarsAux(const InputParameters & parameters);
  static InputParameters validParams();

  virtual ~RedbackDiffVarsAux() {}

protected:
  virtual Real computeValue();
  const VariableValue & _variable_1;
  const VariableValue & _variable_2;
};