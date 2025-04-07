//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "InterfaceDisp.h"

registerMooseObject("RedbackApp", InterfaceDisp);

InputParameters
InterfaceDisp::validParams()
{
  InputParameters params = InterfaceKernel::validParams();
  params.addParam<Real>(
      "penalty", 1, "The penalty that penalizes jump between master and neighbor variables.");
  params.addCoupledVar("tangent_jump",
                       0.0,
                       "The variable describing the tangential jump on the interface. Note that "
                       "the tangent vector is "
                       "oriented +90deg from the normal vector.");
  params.addRequiredRangeCheckedParam<unsigned int>(
      "component",
      "component >= 0 & component <= 1",
      "An integer corresponding to the direction the variable "
      "this kernel acts in. (0 for x, 1 for y)");
  params.addClassDescription("Imposes the jump between the displacements variables.");
  return params;
}

InterfaceDisp::InterfaceDisp(const InputParameters & parameters)
  : InterfaceKernel(parameters),
    _penalty(getParam<Real>("penalty")),
    _jump(coupledValue("tangent_jump")),
    _component(getParam<unsigned int>("component"))
{
}

Real
InterfaceDisp::computeQpResidual(Moose::DGResidualType type)
{
  RealVectorValue fault_tangent(-_normals[_qp](1),
                                _normals[_qp](0)); // 90deg rotation of the normal vector
  Real r = _u[_qp] - _neighbor_value[_qp] + _jump[_qp] * fault_tangent(_component);

  switch (type)
  {
    case Moose::Element:
      return _test[_i][_qp] * _penalty * r;

    case Moose::Neighbor:
      return _test_neighbor[_i][_qp] * -_penalty * r;
  }
  // Should not reach this line
  return 0;
}

Real
InterfaceDisp::computeQpJacobian(Moose::DGJacobianType type)
{
  Real jac = 0;

  switch (type)
  {
    case Moose::ElementElement:
      jac = _test[_i][_qp] * _penalty * _phi[_j][_qp];
      break;

    case Moose::ElementNeighbor:
      jac = _test[_i][_qp] * _penalty * -_phi_neighbor[_j][_qp];
      break;

    case Moose::NeighborElement:
      jac = _test_neighbor[_i][_qp] * -_penalty * _phi[_j][_qp];
      break;

    case Moose::NeighborNeighbor:
      jac = _test_neighbor[_i][_qp] * -_penalty * -_phi_neighbor[_j][_qp];
      break;
  }

  return jac;
}
