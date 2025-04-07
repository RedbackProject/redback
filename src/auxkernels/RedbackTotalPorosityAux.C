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

#include "RedbackTotalPorosityAux.h"

registerMooseObject("RedbackApp", RedbackTotalPorosityAux);

InputParameters
RedbackTotalPorosityAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addParam<bool>("is_mechanics_on", false, "is mechanics on?");
  // params.addParam<bool>("is_chemistry_on", false, "is chemistry on?");
  params.addCoupledVar("mechanical_porosity", 0.0, "Mechanical porosity (as AuxKernel)");
  return params;
}

RedbackTotalPorosityAux::RedbackTotalPorosityAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _is_mechanics_on(getParam<bool>("is_mechanics_on")),
    _delta_porosity_mech(_is_mechanics_on ? coupledValue("mechanical_porosity") : _zero),
    _delta_porosity_chem(getMaterialProperty<Real>("chemical_porosity")),
    _initial_porosity(getMaterialProperty<Real>("initial_porosity")),

    //_has_T(isCoupled("temperature")),
    //_T(_has_T ? coupledValue("temperature") : _zero),

    _mass_removal_rate(_is_mechanics_on ? getMaterialProperty<Real>("mass_removal_rate")
                                        : getZeroMaterialProperty<Real>("mass_removal_rate"))
{
}

Real
RedbackTotalPorosityAux::computeValue()
{
  Real total_porosity;
  // Moose::out << "RedbackTotalPorosityAux::computeValue() at _qp" << _qp <<
  // "\n";
  total_porosity = _initial_porosity[_qp] + _delta_porosity_chem[_qp];
  if (_is_mechanics_on)
    total_porosity = _initial_porosity[_qp] +
                     _delta_porosity_chem[_qp] * (1 + _mass_removal_rate[_qp]) +
                     _delta_porosity_mech[_qp];

  total_porosity = fmin(1.0, fmax(0.0, total_porosity));
  return total_porosity;
}
