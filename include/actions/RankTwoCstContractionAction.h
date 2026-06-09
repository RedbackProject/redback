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

#pragma once

#include "Action.h"

class RankTwoCstContractionAction : public Action
{
public:
  RankTwoCstContractionAction(const InputParameters & params);

  static InputParameters validParams();

  MultiMooseEnum scalarOptions();
  virtual void act() override;

private:
  std::string _base_name;
};