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

#include "MooseMesh.h"
#include "Poromechanics.h"
#include "FEProblemBase.h"

registerMooseObject("RedbackApp", Poromechanics);

InputParameters
Poromechanics::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addRequiredCoupledVar("disp_x", "The x displacement");
  params.addRequiredCoupledVar("disp_y", "The y displacement");
  params.addCoupledVar("disp_z", 0.0, "The z displacement");
  params.addParam<Real>("Peclet_number", 1, "Peclet number");
  params.addParam<Real>("mixture_compressibility", 1, "mixture compressibility, beta_m*");

  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

Poromechanics::Poromechanics(const InputParameters & parameters)
  : Kernel(parameters),
    _grad_disp_x(coupledGradient("disp_x")),
    _grad_disp_y(coupledGradient("disp_y")),
    _grad_disp_z(_mesh.dimension() == 3 ? coupledGradient("disp_z") : _grad_zero),
    _grad_disp_x_old(_fe_problem.isTransient() ? coupledGradientOld("disp_x") : _grad_zero),
    _grad_disp_y_old(_fe_problem.isTransient() ? coupledGradientOld("disp_y") : _grad_zero),
    _grad_disp_z_old(_fe_problem.isTransient() && _mesh.dimension() == 3
                         ? coupledGradientOld("disp_z")
                         : _grad_zero),
    _peclet_number(getParam<Real>("Peclet_number")),
    _mixture_compressibility(getParam<Real>("mixture_compressibility"))
{
}

Poromechanics::~Poromechanics() {}

Real
Poromechanics::computeQpResidual()
{
  Real def_grad = _grad_disp_x[_qp](0) + _grad_disp_y[_qp](1) + _grad_disp_z[_qp](2);
  Real def_grad_old =
      _grad_disp_x_old[_qp](0) + _grad_disp_y_old[_qp](1) + _grad_disp_z_old[_qp](2);
  Real def_grad_rate = (def_grad - def_grad_old) / _dt;

  return def_grad_rate * _peclet_number * _test[_i][_qp] / _mixture_compressibility;
}

Real
Poromechanics::computeQpJacobian()
{
  return 0;
}

Real
Poromechanics::computeQpOffDiagJacobian()
{
  return 0;
}
