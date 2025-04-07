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

#include "DarcyFluxBC.h"

registerMooseObject("RedbackApp", DarcyFluxBC);

InputParameters
DarcyFluxBC::validParams()
{
  InputParameters params = FluxBC::validParams();
  params.addClassDescription(
      "Computes a boundary residual contribution consistent with the Diffusion Kernel. "
      "Does not impose a boundary condition; instead computes the boundary "
      "contribution corresponding to the current value of grad(u) and accumulates "
      "it in the residual vector.");
  return params;
}

DarcyFluxBC::DarcyFluxBC(const InputParameters & parameters)
  : FluxBC(parameters), _Le(getMaterialProperty<Real>("lewis_number"))
{
}

RealGradient
DarcyFluxBC::computeQpFluxResidual()
{
  return _Le[_qp] * _grad_u[_qp];
}

RealGradient
DarcyFluxBC::computeQpFluxJacobian()
{
  return _Le[_qp] * _grad_phi[_j][_qp];
}
