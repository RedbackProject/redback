//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "FullSolveMultiApp.h"

class RedbackFullSolveMultiApp : public FullSolveMultiApp
{
public:
  static InputParameters validParams();
  RedbackFullSolveMultiApp(const InputParameters & parameters);

  virtual bool solveStep(Real dt, Real target_time, bool auto_advance) override;

private:
  Real readFile(const FileName & file_name);

  FileName _times_file;

  Real _porosity_change;
  Real _porosity_change_old;
};