//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "NitscheVelBC.h"

registerMooseObject("RedbackApp", NitscheVelBC);

InputParameters
NitscheVelBC::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  params.addClassDescription("Applies a Nitsche-type weak Dirichlet boundary condition"
                             "for the velocity variable of Navier-Stokes equations"
                             "used in the context of the shifted boundary method.");
  params.addParam<Real>("value", 0.0, "Boundary value of the variable");
  params.addCoupledVar("distance", 0.0, "distance to interface");
  params.declareControllable("value");
  return params;
}

NitscheVelBC::NitscheVelBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
  _mu(getMaterialProperty<Real>("mu")),
  _v(getParam<Real>("value")),
  _d_to_stl(coupledVectorValue("distance"))
{
}

Real
NitscheVelBC::computeQpResidual()
{
  return - _mu[_qp] * _grad_test[_i][_qp] * _normals[_qp] * (-_v + _u[_qp] + _grad_u[_qp] * _d_to_stl[_qp]);
}

Real
NitscheVelBC::computeQpJacobian()
{
  return - _mu[_qp] * _grad_test[_i][_qp] * _normals[_qp] * (_phi[_j][_qp] + _grad_phi[_j][_qp] * _d_to_stl[_qp]);
}
