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

#include "RedbackContinuation.h"

registerMooseObject("RedbackApp", RedbackContinuation);

InputParameters
RedbackContinuation::validParams()
{
  InputParameters params = ODEKernel::validParams();
  params.addParam<Real>("ds", 1.0, "Continuation increment");
  params.addParam<Real>("ds_old", 1.0, "Old continuation increment");
  params.addParam<Real>("continuation_parameter_old", 0.0, "Old value of continuation parameter");
  params.addParam<Real>(
      "continuation_parameter_older", 0.0, "Older value of continuation parameter");
  // params.addCoupledVar("directional_derivative", 0.0, "The directional derivative function");
  params.addParam<PostprocessorName>("directional_derivative_nodal",
                                     "Postprocessor name for nodal part of directional derivative");
  params.addParam<PostprocessorName>(
      "directional_derivative_elemental",
      "Postprocessor name for elemental part of (average value of) directional derivative");
  // params.addParam<PostprocessorName>("number_of_nodes",
  //      "Postprocessor name to get number of nodes");
  // params.addParam<PostprocessorName>("number_of_elements",
  //      "Postprocessor name to get number of elements");)
  return params;
}

RedbackContinuation::RedbackContinuation(const InputParameters & parameters)
  : ODEKernel(parameters),
    _ds_param(getParam<Real>("ds")),
    _ds_old_param(getParam<Real>("ds_old")),
    _continuation_parameter_old_param(getParam<Real>("continuation_parameter_old")),
    _continuation_parameter_older_param(getParam<Real>("continuation_parameter_older")),
    //_directional_derivative(coupledScalarValue("directional_derivative"))
    //_num_nodes(isParamValid("number_of_nodes")
    //  ? &getPostprocessorValue("number_of_nodes")
    //  : NULL),
    //_num_elements(isParamValid("number_of_elements")
    //  ? &getPostprocessorValue("number_of_elements")
    //  : NULL),
    _dd_nodal(isParamValid("directional_derivative_nodal")
                  ? &getPostprocessorValue("directional_derivative_nodal")
                  : NULL),
    _dd_elemental(isParamValid("directional_derivative_elemental")
                      ? &getPostprocessorValue("directional_derivative_elemental")
                      : NULL)
{
  _are_nodal_vars_involved = (NULL != _dd_nodal);
  _are_elemental_vars_involved = (NULL != _dd_elemental);
  // if (NULL == _num_nodes)
  //  mooseError("ScalarKernel RedbackContinuation requires postprocessor \"number_of_nodes\"");
  // if (NULL == _num_elements)
  //  mooseError("ScalarKernel RedbackContinuation requires postprocessor \"number_of_elements\"");
}

Real
RedbackContinuation::computeQpResidual()
{
  Real my_sum = 0;
  if (_are_nodal_vars_involved)
    my_sum += *_dd_nodal;
  if (_are_elemental_vars_involved)
    my_sum += *_dd_elemental;
  Real lambda_dot_old_param =
      (_continuation_parameter_old_param - _continuation_parameter_older_param) / _ds_old_param;
  my_sum += lambda_dot_old_param * (_u[_i] - _continuation_parameter_old_param);
  return my_sum - _ds_param;
}

Real
RedbackContinuation::computeQpJacobian()
{
  // dF/dx
  Real lambda_dot_old_param =
      (_continuation_parameter_old_param - _continuation_parameter_older_param) / _ds_old_param;
  // TODO: compute lambda_dot_old_param only once...
  return lambda_dot_old_param;
}

/*Real
RedbackContinuation::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _y_var)
    return -2.;         // dF/dy
  else
    return 0.;          // everything else
}
*/
