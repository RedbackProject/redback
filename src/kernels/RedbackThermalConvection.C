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

#include "RedbackThermalConvection.h"

registerMooseObject("RedbackApp", RedbackThermalConvection);

InputParameters
RedbackThermalConvection::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addCoupledVar("pore_pres", 0.0, "Pore pressure variable.");
  params.addParam<Real>("time_factor", 1.0, "Time rescaling factor (global parameter!)");

  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

RedbackThermalConvection::RedbackThermalConvection(const InputParameters & parameters)
  : Kernel(parameters),
    _mixture_convective_energy(getMaterialProperty<RealVectorValue>("mixture_convective_energy")),
    //_mixture_convective_energy_jac(getMaterialProperty<Real>("mixture_convective_energy_jacobian")),
    //_mixture_convective_energy_off_jac(getMaterialProperty<Real>("mixture_convective_energy_off_jacobian")),
    _pore_pres_var(coupled("pore_pres")),
    _time_factor(getParam<Real>("time_factor"))
{
}

Real
RedbackThermalConvection::computeQpResidual()
{
  return _test[_i][_qp] * (_time_factor * _mixture_convective_energy[_qp] * _grad_u[_qp]);
}

Real
RedbackThermalConvection::computeQpJacobian()
{
  return 0;
  // return _test[_i][_qp] * _time_factor * _mixture_convective_energy[_qp] *
  // _grad_phi[_j][_qp]
  //  + _test[_i][_qp] * _time_factor * _mixture_convective_energy_jac[_qp] *
  //  _phi[_j][_qp];
}

Real
RedbackThermalConvection::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  /*if (jvar == _pore_pres_var)
  {
    return _test[_i][_qp] * _time_factor *
  _mixture_convective_energy_off_jac[_qp] * _phi[_j][_qp];
  }*/
  return 0;
}
