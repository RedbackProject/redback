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

#include "RedbackNavier.h"

registerMooseObject("RedbackApp", RedbackNavier);

InputParameters
RedbackNavier::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addRequiredParam<unsigned int>("component",
                                        "An integer corresponding to the direction the variable "
                                        "this kernel acts in. (0 for x, 1 for y, 2 for z)");

  params.addParam<Real>("time_factor", 1.0, "Time rescaling factor (global parameter!)");

  params.addRequiredCoupledVar("fluid_vel_x", "The x component of fluid velocity");
  params.addRequiredCoupledVar("fluid_vel_y", "The y component of fluid velocity");
  params.addCoupledVar("fluid_vel_z", 0.0, "The z component of fluid velocity");
  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

RedbackNavier::RedbackNavier(const InputParameters & parameters)
  : Kernel(parameters),
    _component(getParam<unsigned int>("component")),

    _fluid_vel_x(coupledValue("fluid_vel_x")),
    _fluid_vel_y(coupledValue("fluid_vel_y")),
    _fluid_vel_z(coupledValue("fluid_vel_z")),

    _vel_fluid_x_var(coupled("fluid_vel_x")),
    _vel_fluid_y_var(coupled("fluid_vel_y")),
    _vel_fluid_z_var(coupled("fluid_vel_z")),

    _time_factor(getParam<Real>("time_factor"))
{
}

RedbackNavier::~RedbackNavier() {}

Real
RedbackNavier::computeQpResidual()
{
  return _time_factor * _test[_i][_qp] * _grad_u[_qp] *
         RealVectorValue(_fluid_vel_x[_qp], _fluid_vel_y[_qp], _fluid_vel_z[_qp]);
}

Real
RedbackNavier::computeQpJacobian()
{
  // currently not benchmarked
  return (RealVectorValue(_fluid_vel_x[_qp], _fluid_vel_y[_qp], _fluid_vel_z[_qp]) *
              _grad_phi[_j][_qp] +
          _phi[_j][_qp] * _grad_u[_qp](_component)) *
         _test[_i][_qp];
}

Real
RedbackNavier::computeQpOffDiagJacobian(unsigned int jvar)
{
  // currently not benchmarked
  if (jvar == _vel_fluid_x_var)
    return _phi[_j][_qp] * _grad_u[_qp](0) * _test[_i][_qp];
  else if (jvar == _vel_fluid_y_var)
    return _phi[_j][_qp] * _grad_u[_qp](1) * _test[_i][_qp];
  else if (jvar == _vel_fluid_z_var)
    return _phi[_j][_qp] * _grad_u[_qp](2) * _test[_i][_qp];

  else
    return 0;
}
