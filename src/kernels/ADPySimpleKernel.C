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


#include "ADPySimpleKernel.h"

registerMooseObject("RedBackApp", ADPySimpleKernel);

InputParameters
ADPySimpleKernel::validParams()
{
  InputParameters params = ADKernelValue::validParams();
  params.addClassDescription("Adds a distributed p-y soil reaction term to the weak form.");
  params.addParam<MaterialPropertyName>(
      "py_force_name",
      "py_force",
      "Name of the AD material property storing the distributed p-y reaction.");
  return params;
}

ADPySimpleKernel::ADPySimpleKernel(const InputParameters & parameters)
  : ADKernelValue(parameters),
    _py_force(getADMaterialProperty<Real>("py_force_name"))
{
}

ADReal
ADPySimpleKernel::precomputeQpResidual()
{
  // ADKernelValue automatically multiplies by the test function.
  // This returns the value term p(y,z).
  return _py_force[_qp];
}