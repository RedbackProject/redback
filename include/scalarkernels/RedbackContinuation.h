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

#ifndef REDBACKCONTINUATION_H
#define REDBACKCONTINUATION_H

#include "ODEKernel.h"

class RedbackContinuation : public ODEKernel
{
public:
  RedbackContinuation(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  Real _ds_param, _ds_old_param, _continuation_parameter_old_param,
      _continuation_parameter_older_param;
  // VariableValue & _directional_derivative;
  // const PostprocessorValue * const _num_nodes;
  // const PostprocessorValue * const _num_elements;
  const PostprocessorValue * const _dd_nodal;     // nodal part of directional derivative
  const PostprocessorValue * const _dd_elemental; // elemental part of directional derivative
  bool _are_nodal_vars_involved, _are_elemental_vars_involved;

  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  // virtual Real computeQpOffDiagJacobian(unsigned int jvar);
};

#endif /* REDBACKCONTINUATION_H */
