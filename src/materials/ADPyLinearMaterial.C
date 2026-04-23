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

#include "ADPyLinearMaterial.h"

registerMooseObject("YourAppApp", ADPyLinearMaterial);

InputParameters
ADPyLinearMaterial::validParams()
{
  InputParameters params = ADMaterial::validParams();
  params.addClassDescription("Computes a linear Winkler p-y soil reaction p = k*y.");

  params.addRequiredCoupledVar("y", "Lateral pile displacement variable");

  params.addParam<MaterialPropertyName>(
      "py_force_name",
      "py_force",
      "Name of the AD material property storing the distributed p-y reaction.");

  params.addRequiredParam<Real>("k0", "Linear Winkler stiffness at zero depth.");
  params.addParam<Real>("k_rate", 0.0, "Rate of increase of Winkler stiffness with depth.");

  params.addParam<unsigned int>(
      "depth_component",
      2,
      "Coordinate component used as depth: 0=x, 1=y, 2=z.");

  params.addParam<Real>(
      "depth_origin",
      0.0,
      "Reference coordinate for zero depth. Depth = q_point(component) - depth_origin.");

  params.addParam<bool>(
      "positive_depth_only",
      true,
      "If true, depth is clamped below at zero before computing k.");

  return params;
}

ADPyLinearMaterial::ADPyLinearMaterial(const InputParameters & parameters)
  : ADMaterial(parameters),
    _y(adCoupledValue("y")),
    _py_force(declareADProperty<Real>(getParam<MaterialPropertyName>("py_force_name"))),
    _k0(getParam<Real>("k0")),
    _k_rate(getParam<Real>("k_rate")),
    _depth_component(getParam<unsigned int>("depth_component")),
    _depth_origin(getParam<Real>("depth_origin")),
    _positive_depth_only(getParam<bool>("positive_depth_only"))
{
  if (_depth_component > 2)
    paramError("depth_component", "depth_component must be 0, 1, or 2.");
}

void
ADPyLinearMaterial::computeQpProperties()
{
  Real depth = _q_point[_qp](_depth_component) - _depth_origin;

  if (_positive_depth_only && depth < 0.0)
    depth = 0.0;

  const Real k = _k0 + _k_rate * depth;

  if (k < 0.0)
    mooseError("Computed Winkler stiffness k is negative at qp ", _qp, ".");

  _py_force[_qp] = k * _y[_qp];
}