/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "SideIntegralVoidPostprocessor.h"

registerMooseObject("RedbackApp", SideIntegralVoidPostprocessor);

InputParameters
SideIntegralVoidPostprocessor::validParams()
{
  InputParameters params = SideIntegralPostprocessor::validParams();
  params.addRequiredCoupledVar("variable",
                               "The name of the variable that this boundary condition applies to");
  params.addRequiredRangeCheckedParam<unsigned int>(
	      "index_i", "index_i >= 0 & index_i <= 2",
	      "The index i of ij for the tensor to output (0, 1, 2)");
  params.addRequiredRangeCheckedParam<unsigned int>(
	      "index_j", "index_j >= 0 & index_j <= 2",
	      "The index j of ij for the tensor to output (0, 1, 2)");
  params.addCoupledVar("disp_x", 0.0, "The x displacement");
  params.addCoupledVar("disp_y", 0.0, "The y displacement");
  params.addCoupledVar("disp_z", 0.0, "The z displacement");
  params.addParam<bool>(
    "velocity", false, "Compute the macroscopic strain rate in the voids");
  return params;
}

SideIntegralVoidPostprocessor::SideIntegralVoidPostprocessor(
    const InputParameters & parameters)
  : SideIntegralPostprocessor(parameters),
    MooseVariableInterface(this, false),
    _u(coupledValue("variable")),
    _i(getParam<unsigned int>("index_i")),
    _j(getParam<unsigned int>("index_j")),

    _disp_x(isCoupled("disp_x") ? coupledValue("disp_x") : _zero),
    _disp_y(isCoupled("disp_y") ? coupledValue("disp_y") : _zero),
    _disp_z(isCoupled("disp_z") ? coupledValue("disp_z") : _zero),

    _dispx_dot(isCoupled("disp_x") ? coupledDot("disp_x") : _zero),
    _dispy_dot(isCoupled("disp_y") ? coupledDot("disp_y") : _zero),
    _dispz_dot(isCoupled("disp_z") ? coupledDot("disp_z") : _zero),
    _vel(getParam<bool>("velocity"))
{
  addMooseVariableDependency(mooseVariable());
}

Real
SideIntegralVoidPostprocessor::computeQpIntegral()
{
  RealVectorValue vector = RealVectorValue(_disp_x[ _qp ], _disp_y[ _qp ], _disp_z[ _qp ]);
  if (_vel)
    vector = RealVectorValue(_dispx_dot[ _qp ], _dispy_dot[ _qp ], _dispz_dot[ _qp ]);

  return 0.5 * (vector(_i) * _normals[_qp](_j) + vector(_j) * _normals[_qp](_i));
}
