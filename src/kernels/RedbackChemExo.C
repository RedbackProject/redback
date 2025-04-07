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

#include "RedbackChemExo.h"

registerMooseObject("RedbackApp", RedbackChemExo);

InputParameters
RedbackChemExo::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addParam<Real>("time_factor", 1.0, "Time rescaling factor (global parameter!)");

  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

RedbackChemExo::RedbackChemExo(const InputParameters & parameters)
  : Kernel(parameters),
    _chemical_exothermic_energy(getMaterialProperty<Real>("chemical_exothermic_energy")),
    _chemical_exothermic_energy_jac(
        getMaterialProperty<Real>("chemical_exothermic_energy_jacobian")),
    _time_factor(getParam<Real>("time_factor"))
{
}

RedbackChemExo::~RedbackChemExo() {}

Real
RedbackChemExo::computeQpResidual()
{
  return -_time_factor * _test[_i][_qp] * _chemical_exothermic_energy[_qp];
}

Real
RedbackChemExo::computeQpJacobian()
{
  return -_time_factor * _test[_i][_qp] * _chemical_exothermic_energy_jac[_qp] * _phi[_j][_qp];
}
