//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef SIDEPOINTVALUEPOSTPROCESSOR_H
#define SIDEPOINTVALUEPOSTPROCESSOR_H

#include "SidePostprocessor.h"
#include "RankTwoTensor.h"

/**
 * This postprocessor computes a volume integral of the specified variable.
 *
 * Note that specializations of this integral are possible by deriving from this
 * class and overriding computeQpIntegral().
 */
class SidePointValuePostprocessor : public SidePostprocessor
{
public:
  SidePointValuePostprocessor(const InputParameters & parameters);
  static InputParameters validParams();

  virtual void initialize() override;
  virtual void execute() override;
  virtual Real getValue() const override;
  virtual void threadJoin(const UserObject & y) override;

protected:
  unsigned int _qp;

  const MaterialProperty<RankTwoTensor> & _tensor;
  bool _normal;
  bool _show;
  bool _has_pres;
  const VariableValue & _pressure;

  /// The point to locate
  const Point & _point;

  /// The value of the variable at the desired location
  Real _value;
};

#endif
