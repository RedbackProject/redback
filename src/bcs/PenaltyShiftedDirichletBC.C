//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PenaltyShiftedDirichletBC.h"

registerMooseObject("MooseApp", PenaltyShiftedDirichletBC);

InputParameters
PenaltyShiftedDirichletBC::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  params.addClassDescription("Applies the penalty term for Nitsche-type weak Dirichlet boundary condition"
                             "for the velocity variable of Navier-Stokes equations"
                             "used in the context of the shifted boundary method.");
  params.addParam<Real>("penalty", 1.0, "Penalty scalar");
  params.addParam<Real>("mesh_size", 1.0, "Mesh dimension in one direction");
  params.addParam<Real>("nb_elements", 1.0, "Number of elements in one direction");
  params.addParam<Real>("value", 0.0, "Boundary value of the variable");
  params.addCoupledVar("distance", 0.0, "distance to interface");
  params.declareControllable("value");
  return params;
}

PenaltyShiftedDirichletBC::PenaltyShiftedDirichletBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
  _p(getParam<Real>("penalty")),
  _mesh_size(getParam<Real>("mesh_size")),
  _nb_elem(getParam<Real>("nb_elements")),
  _v(getParam<Real>("value")),
  _d_to_stl(coupledVectorValue("distance"))
{
}

Real
PenaltyShiftedDirichletBC::computeQpResidual()
{
  return 2*_p*_nb_elem/_mesh_size * (_test[_i][_qp] + _grad_test[_i][_qp] * _d_to_stl[_qp]) * (-_v + _u[_qp] + _grad_u[_qp] * _d_to_stl[_qp]);
}

Real
PenaltyShiftedDirichletBC::computeQpJacobian()
{
  return 2*_p*_nb_elem/_mesh_size * (_test[_i][_qp] + _grad_test[_i][_qp] * _d_to_stl[_qp]) * (_phi[_j][_qp] + _grad_phi[_j][_qp] * _d_to_stl[_qp]);
}
