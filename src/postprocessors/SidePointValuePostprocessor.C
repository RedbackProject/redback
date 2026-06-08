//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SidePointValuePostprocessor.h"
#include "RankTwoTensor.h"

#include "libmesh/quadrature.h"

registerMooseObject("RedbackApp", SidePointValuePostprocessor);

InputParameters
SidePointValuePostprocessor::validParams()
{
  InputParameters params = SidePostprocessor::validParams();
  params.addParam<MaterialPropertyName>("rank_two_tensor", "The rank two material tensor name.");
  params.addParam<bool>("normal", false, "If true, the normal value to the fault will be computed");
  params.addParam<bool>("show", false, "If true, the coordinates of the qps will be printed");
  params.addCoupledVar("pressure", 0.0, "Dimensionless pore pressure");
  params.addRequiredParam<Point>("point",
                                 "The physical point where the solution will be evaluated.");
  return params;
}

SidePointValuePostprocessor::SidePointValuePostprocessor(const InputParameters & parameters)
  : SidePostprocessor(parameters),
    _qp(0),
    _tensor(getMaterialProperty<RankTwoTensor>("rank_two_tensor")),
    _normal(getParam<bool>("normal")),
    _show(getParam<bool>("show")),
    _has_pres(isCoupled("pressure")),
    _pressure(_has_pres ? coupledValue("pressure") : _zero),
    _point(getParam<Point>("point")),
    _value(0)
{
}

void
SidePointValuePostprocessor::initialize()
{
}

void
SidePointValuePostprocessor::execute()
{
  Real tol = 1e-1;
  for (_qp = 0; _qp < _qrule->n_points(); _qp++)
  {
    if (_show)
      printf("qpoint = (%f, %f, %f)\n", _q_point[_qp](0), _q_point[_qp](1), _q_point[_qp](2));
    if (std::abs(_point(0) - _q_point[_qp](0)) < tol &&
        std::abs(_point(1) - _q_point[_qp](1)) < tol &&
        std::abs(_point(2) - _q_point[_qp](2)) < tol)
    {
      // traction ti = sigma_ij . n_i
      // traction ti = (sigma_ij' - pI) . n_i
      // ti = ti_t + ti_n
      // ti_t = ti - ||ti_n||.n_i
      // ti_t = ti - (ti . n_i).n_i
      RankTwoTensor total_stress = _tensor[_qp];
      total_stress.addIa(-_pressure[_qp]);
      RealVectorValue traction = total_stress * _normals[_qp];
      if (_normal)
      {
        _value = traction * _normals[_qp];
        break;
      }
      RealVectorValue tangent_traction = traction - (traction * _normals[_qp]) * _normals[_qp];
      _value = std::sqrt(tangent_traction.norm_sq());
    }
  }
}

void
SidePointValuePostprocessor::threadJoin(const UserObject & y)
{
  const SidePointValuePostprocessor & pps = static_cast<const SidePointValuePostprocessor &>(y);
  if (pps._value != 0)
    _value = pps._value;
}

Real
SidePointValuePostprocessor::getValue() const
{
  return _value;
}
