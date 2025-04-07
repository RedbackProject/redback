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

#include "RedbackMassConvection.h"

registerMooseObject("RedbackApp", RedbackMassConvection);

InputParameters
RedbackMassConvection::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addCoupledVar("temperature", 0.0, "Temperature variable.");
  params.addParam<Real>("time_factor", 1.0, "Time rescaling factor (global parameter!)");

  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

RedbackMassConvection::RedbackMassConvection(const InputParameters & parameters)
  : Kernel(parameters),
    _grad_temp(coupledGradient("temperature")),
    //_dtemp_dot_dtemp(coupledDotDu("temperature")),

    _pressure_convective_mass(getMaterialProperty<RealVectorValue>("pressure_convective_mass")),
    _thermal_convective_mass(getMaterialProperty<RealVectorValue>("thermal_convective_mass")),
    //_convective_mass_jac_vec(getMaterialProperty<RealVectorValue>("convective_mass_jacobian_vector")),
    //_convective_mass_jac_real(getMaterialProperty<Real>("convective_mass_jacobian_real")),
    //_convective_mass_off_diag_vec(getMaterialProperty<RealVectorValue>("convective_mass_off_diagonal_vector")),
    //_convective_mass_off_diag_real(getMaterialProperty<Real>("convective_mass_off_diagonal_real")),
    _temp_var(coupled("temperature")),
    _time_factor(getParam<Real>("time_factor"))
{
}

Real
RedbackMassConvection::computeQpResidual()
{
  return _time_factor * _test[_i][_qp] *
         (_pressure_convective_mass[_qp] * _grad_u[_qp] -
          _thermal_convective_mass[_qp] * _grad_temp[_qp]); // scalar product done by "*"
}

Real
RedbackMassConvection::computeQpJacobian()
{
  return 0;
  // return _time_factor*_test[_i][_qp] * (_convective_mass_jac_vec[_qp] *
  // _grad_phi[_j][_qp]
  //  + _convective_mass_jac_real[_qp]* _phi[_j][_qp]);// * _phi[_j][_qp];
}

Real
RedbackMassConvection::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  /*if (jvar == _temp_var)
  {
    return -_time_factor*_test[_i][_qp] * (_convective_mass_off_diag_vec[_qp] *
  _grad_phi[_j][_qp] +
  _convective_mass_off_diag_real[_qp]* _phi[_j][_qp]);// * _phi[_j][_qp]);
  }*/
  return 0;
}
