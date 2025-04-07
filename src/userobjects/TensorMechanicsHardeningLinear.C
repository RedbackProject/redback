/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#include "TensorMechanicsHardeningLinear.h"

registerMooseObject("RedBackApp",TensorMechanicsHardeningLinear);


InputParameters
TensorMechanicsHardeningLinear::validParams()
{
  InputParameters params = SolidMechanicsHardeningModel::validParams();
  params.addRequiredParam<Real>("value_0", "The value of the parameter at internal_parameter = 0");
  params.addParam<Real>("rate", 0, "Let p = internal_parameter.  Then value = _val_0(1 +_rate*intnl)");
  params.addClassDescription("Hardening is Linear");
  return params;
}

TensorMechanicsHardeningLinear::TensorMechanicsHardeningLinear(const InputParameters & parameters) :
  SolidMechanicsHardeningModel(parameters),
  _val_0(getParam<Real>("value_0")),
  _rate(getParam<Real>("rate"))
{
}

Real
TensorMechanicsHardeningLinear::value(Real intnl) const
{
  return _val_0*(1 +_rate*intnl);
}

Real
TensorMechanicsHardeningLinear::derivative(Real intnl) const
{
  return (_rate*_val_0 *intnl);
}

std::string
TensorMechanicsHardeningLinear::modelName() const
{
  return "Linear";
}
