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

// AuxKernel to compute difference of 2 (aux)variables
#include "RedbackDiffVarsAux.h"

registerMooseObject("RedbackApp", RedbackDiffVarsAux);

InputParameters
RedbackDiffVarsAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addCoupledVar("variable_1", 0.0, "First variable");
  params.addCoupledVar("variable_2", 0.0, "Second variable");
  return params;
}

RedbackDiffVarsAux::RedbackDiffVarsAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _variable_1(coupledValue("variable_1")),
    _variable_2(coupledValue("variable_2"))
{
}

Real
RedbackDiffVarsAux::computeValue()
{
  Real difference;
  difference = _variable_1[_qp] - _variable_2[_qp];
  return difference;
}
