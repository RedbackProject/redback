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

#include "RedbackChemPressure.h"

registerMooseObject("RedbackApp", RedbackChemPressure);

InputParameters
RedbackChemPressure::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addCoupledVar("temperature", 0.0, "Temperature variable.");
  params.addParam<Real>("time_factor", 1.0, "Time rescaling factor (global parameter!)");

  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

RedbackChemPressure::RedbackChemPressure(const InputParameters & parameters)
  : Kernel(parameters),
    _chemical_source_mass(getMaterialProperty<Real>("chemical_source_mass")),
    _chemical_source_mass_jac(getMaterialProperty<Real>("chemical_source_mass_jacobian")),
    _temp_var(coupled("temperature")),
    _time_factor(getParam<Real>("time_factor"))
{
}

RedbackChemPressure::~RedbackChemPressure() {}

Real
RedbackChemPressure::computeQpResidual()
{
  return -_time_factor * _test[_i][_qp] * _chemical_source_mass[_qp];
}

Real
RedbackChemPressure::computeQpJacobian()
{
  return 0;
}

Real
RedbackChemPressure::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _temp_var)
  {
    return -_time_factor * _test[_i][_qp] * _chemical_source_mass_jac[_qp] * _phi[_j][_qp];
  }
  return 0;
}
