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

#include "ADPySimpleMaterial.h"
#include <cmath>


registerMooseObject("RedbackApp", ADPySimpleMaterial);

InputParameters
ADPySimpleMaterial::validParams()
{
  InputParameters params = ADMaterial::validParams();
  params.addClassDescription("Computes a simple smooth nonlinear p-y soil reaction.");

  params.addRequiredCoupledVar("y", "Lateral pile displacement variable");

  params.addParam<MaterialPropertyName>(
      "py_force_name",
      "py_force",
      "Name of the AD material property storing the distributed p-y reaction.");

  params.addRequiredParam<Real>("pult0", "Ultimate resistance at zero depth.");
  params.addParam<Real>("pult_rate", 0.0, "Rate of increase of ultimate resistance with depth.");
  params.addRequiredParam<Real>("y50", "Reference displacement controlling curvature of the p-y law.");

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
      "If true, depth is clamped below at zero before computing pult.");

  return params;
}

ADPySimpleMaterial::ADPySimpleMaterial(const InputParameters & parameters)
  : ADMaterial(parameters),
    _y(adCoupledValue("y")),
_py_force(declareADProperty<Real>(getParam<MaterialPropertyName>("py_force_name"))),
    _pult0(getParam<Real>("pult0")),
    _pult_rate(getParam<Real>("pult_rate")),
    _y50(getParam<Real>("y50")),
    _depth_component(getParam<unsigned int>("depth_component")),
    _depth_origin(getParam<Real>("depth_origin")),
    _positive_depth_only(getParam<bool>("positive_depth_only"))
{
  if (_y50 <= 0.0)
    paramError("y50", "y50 must be strictly positive.");

  if (_depth_component > 2)
    paramError("depth_component", "depth_component must be 0, 1, or 2.");
}

void
ADPySimpleMaterial::computeQpProperties()
{
  Real depth = _q_point[_qp](_depth_component) - _depth_origin;

  if (_positive_depth_only && depth < 0.0)
    depth = 0.0;

  const Real pult = _pult0 + _pult_rate * depth;

  if (pult < 0.0)
    mooseError("Computed pult is negative at qp ", _qp, ". Check pult0/pult_rate/depth settings.");

  const ADReal y = _y[_qp];

  _py_force[_qp] = pult * tanh(y / _y50);
}