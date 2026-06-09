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

#include "Action.h"

class RankTwoScalarAction : public Action
{
public:
  RankTwoScalarAction(const InputParameters & params);

  static InputParameters validParams();

  MultiMooseEnum scalarOptions();
  virtual void act() override;
};