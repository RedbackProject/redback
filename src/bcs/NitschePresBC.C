//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "NitschePresBC.h"

registerMooseObject("RedbackApp", NitschePresBC);

InputParameters
NitschePresBC::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  params.addClassDescription("Applies a Nitsche-type weak Dirichlet boundary condition"
                             "for the pressure variable of Navier-Stokes equations"
                             "used in the context of the shifted boundary method.");
  params.addRequiredParam<unsigned>("component", "The velocity component that this is applied to.");
  params.addParam<Real>("value", 0.0, "Boundary value of the variable");
  params.addCoupledVar("distance", 0.0, "distance to interface");
  params.addCoupledVar("vel", 0.0, "vel");
  params.declareControllable("value");
  return params;
}

NitschePresBC::NitschePresBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
  _component(getParam<unsigned>("component")),
  _v(getParam<Real>("value")),
  _d_to_stl(coupledVectorValue("distance")),
  _vel(coupledValue("vel")),
  _grad_vel(coupledGradient("vel")),
  _vel_var_number(coupled("vel"))
{
}

Real
NitschePresBC::computeQpResidual()
{
  return _test[_i][_qp] * _normals[_qp](_component) * (-_v + _vel[_qp] + _grad_vel[_qp] * _d_to_stl[_qp]);
}

Real
NitschePresBC::computeQpJacobian()
{
  return 0;
}

Real
NitschePresBC::computeQpOffDiagJacobian(unsigned jvar)
{
  if (jvar == _vel_var_number)
  {
    return _test[_i][_qp] * _normals[_qp](_component) * (_phi[_j][_qp] + _grad_phi[_j][_qp] * _d_to_stl[_qp]);
  }
  return 0;
}
