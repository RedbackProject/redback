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

#include "InterfaceDarcy.h"
#include "MooseVariable.h"

#include <cmath>

registerMooseObject("RedbackApp", InterfaceDarcy);

InputParameters
InterfaceDarcy::validParams()
{
  InputParameters params = InterfaceKernel::validParams();
  params.addRequiredCoupledVar("fault_lewis_number", "Lewis number of the fault.");
  params.addParam<Real>("fault_thickness", 1., "The neighboring diffusion coefficient.");
  return params;
}

InterfaceDarcy::InterfaceDarcy(const InputParameters & parameters)
  : InterfaceKernel(parameters),
    _Le_fault(coupledValue("fault_lewis_number")),
    _gravity_term(getMaterialProperty<RealVectorValue>("fluid_gravity_term")),
    _thickness(getParam<Real>("fault_thickness"))
{
  if (!parameters.isParamValid("boundary"))
  {
    mooseError("In order to use the InterfaceDarcy dgkernel, you must specify a boundary where "
               "it will live.");
  }
}

Real
InterfaceDarcy::computeQpResidual(Moose::DGResidualType type)
{
  Real grad_interface = (_neighbor_value[_qp] - _u[_qp]) / (_Le_fault[_qp] * _thickness) -
                        _gravity_term[_qp] * _normals[_qp] / _Le_fault[_qp];

  switch (type)
  {
    // continuity of flux
    case Moose::Element:
      return -grad_interface * _test[_i][_qp];
      // grad_u is the natural boundary condition
      // adding this residual imposes (grad_u - grad_interface)=0 at the interface

    case Moose::Neighbor:
      return grad_interface * _test_neighbor[_i][_qp];

    default:
      mooseError("InterfaceDarcy type not supported.");
  }
  // should not reach this line
  return 0;
}

Real
InterfaceDarcy::computeQpJacobian(Moose::DGJacobianType type)
{
  switch (type)
  {
    case Moose::ElementElement:
      return _phi[_j][_qp] / (_Le_fault[_qp] * _thickness) * _test[_i][_qp];

    case Moose::NeighborNeighbor:
      return _phi_neighbor[_j][_qp] / (_Le_fault[_qp] * _thickness) * _test_neighbor[_i][_qp];

    case Moose::ElementNeighbor:
      return -_phi_neighbor[_j][_qp] / (_Le_fault[_qp] * _thickness) * _test[_i][_qp];

    case Moose::NeighborElement:
      return -_phi[_j][_qp] / (_Le_fault[_qp] * _thickness) * _test_neighbor[_i][_qp];
  }
  mooseError("Internal error.");
}
