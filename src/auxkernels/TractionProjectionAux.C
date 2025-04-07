/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "TractionProjectionAux.h"

registerMooseObject("RedbackApp", TractionProjectionAux);

InputParameters
TractionProjectionAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription(
      "Compute normal or tangential projection of a traction (stress projected on a vector) in 2D");
  params.addRequiredParam<MaterialPropertyName>("stress_tensor", "The stress tensor name");
  params.addRequiredParam<Real>(
      "surface_angle",
      "The (direct) angle of the surface in degrees/radians from the horizontal axis");
  params.addParam<bool>(
      "normal", false, "True to compute normal value (to the vector), false for tangential.");
  params.addCoupledVar("pressure", 0.0, "Dimensionless pore pressure");
  params.addParam<bool>("convert_to_radians",
                        false,
                        "If true, the value you entered will be "
                        "multiplied by Pi/180");
  return params;
}

TractionProjectionAux::TractionProjectionAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _tensor(getMaterialProperty<RankTwoTensor>("stress_tensor")),
    _normal(getParam<bool>("normal")),
    _has_pres(isCoupled("pressure")),
    _pressure(_has_pres ? coupledValue("pressure") : _zero),
    _angle(getParam<bool>("convert_to_radians") ? getParam<Real>("surface_angle") * M_PI / 180.0
                                                : getParam<Real>("surface_angle"))
{
}

Real
TractionProjectionAux::computeValue()
{
  // traction ti = sigma_ij . n_i
  // traction ti = (sigma_ij' - pI) . n_i
  // ti = ti_t + ti_n
  // ti_t = ti - ||ti_n||.n_i
  // ti_t = ti - (ti . n_i).n_i
  RealVectorValue fault_normal(-sin(_angle), cos(_angle));
  RankTwoTensor total_stress = _tensor[_qp];
  total_stress.addIa(-_pressure[_qp]);
  RealVectorValue traction = total_stress * fault_normal;
  if (_normal)
    return traction * fault_normal;
  RealVectorValue fault_tangent(cos(_angle), sin(_angle));
  // return (traction - (traction * fault_normal) * fault_normal) * fault_tangent;
  return traction * fault_tangent;
}
