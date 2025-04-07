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

#include "RedbackChemEndo.h"

registerMooseObject("RedbackApp", RedbackChemEndo);

InputParameters
RedbackChemEndo::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addParam<Real>("time_factor", 1.0, "Time rescaling factor (global parameter!)");

  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

RedbackChemEndo::RedbackChemEndo(const InputParameters & parameters)
  : Kernel(parameters),
    _chemical_endothermic_energy(getMaterialProperty<Real>("chemical_endothermic_energy")),
    _chemical_endothermic_energy_jac(
        getMaterialProperty<Real>("chemical_endothermic_energy_jacobian")),
    _time_factor(getParam<Real>("time_factor"))
{
}

RedbackChemEndo::~RedbackChemEndo() {}

Real
RedbackChemEndo::computeQpResidual()
{
  return _time_factor * _test[_i][_qp] * _chemical_endothermic_energy[_qp];
}

Real
RedbackChemEndo::computeQpJacobian()
{
  return _time_factor * _test[_i][_qp] * _chemical_endothermic_energy_jac[_qp] * _phi[_j][_qp];
}
