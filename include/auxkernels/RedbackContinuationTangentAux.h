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

#ifndef REDBACKCONTINUATIONTANGENTAUX_H
#define REDBACKCONTINUATIONTANGENTAUX_H

#include "AuxNodalScalarKernel.h"

/**
 *
 */
class RedbackContinuationTangentAux : public AuxNodalScalarKernel
{
public:
  RedbackContinuationTangentAux(const InputParameters & parameters);
  static InputParameters validParams();

  virtual ~RedbackContinuationTangentAux();

  virtual void compute();

protected:
  virtual Real computeValue();
  Real _ds_old_param;
  unsigned int _nb_sum_vars, _nb_sum_vars_old, _nb_sum_vars_older; // number of variables to sum
  std::vector<const VariableValue *> _sum_vars;                    // coupled variables to sum
  std::vector<const VariableValue *> _sum_vars_old;                // old coupled variables to sum
  std::vector<const VariableValue *> _sum_vars_older;              // older coupled variables to sum
  bool _are_nodal_vars_involved, _are_elemental_vars_involved;
};

#endif /* REDBACKCONTINUATIONTANGENTAUX_H */
