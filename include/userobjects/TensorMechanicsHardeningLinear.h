/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#ifndef TENSORMECHANICSHARDENINGLINEAR_H
#define TENSORMECHANICSHARDENINGLINEAR_H

#include "SolidMechanicsHardeningModel.h"

class TensorMechanicsHardeningLinear;


template<>
InputParameters validParams<TensorMechanicsHardeningLinear>();

/**
 * Linear hardening
 * The value = _val_0*(1 +_rate*intnl)
 */
class TensorMechanicsHardeningLinear : public SolidMechanicsHardeningModel
{
 public:
  static InputParameters validParams();

  TensorMechanicsHardeningLinear(const InputParameters & parameters);

  virtual Real value(Real intnl) const;

  virtual Real derivative(Real intnl) const;

  virtual std::string modelName() const;

 private:

  /// The value = _val_0*(1 +_rate*intnl)
  Real _val_0;

  /// The value = _val_0*(1 +_rate*intnl)
  Real _rate;

};

#endif // TENSORMECHANICSHARDENINGLINEAR_H
