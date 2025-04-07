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

#include "MooseMesh.h"
#include "RedbackContinuationTangentAux.h"

registerMooseObject("RedbackApp", RedbackContinuationTangentAux);

InputParameters
RedbackContinuationTangentAux::validParams()
{
  InputParameters params = AuxNodalScalarKernel::validParams();
  params.addParam<Real>("ds_old", 1.0, "Old continuation increment");

  params.addCoupledVar("sum_vars",
                       "Coupled variables to sum "
                       "(nodal or elemental)");
  params.addCoupledVar("sum_vars_old",
                       "Old coupled variables to sum "
                       "(nodal or elemental). Must be in same order as 'sum_vars'");
  params.addCoupledVar("sum_vars_older",
                       "Older coupled variables to sum "
                       "(nodal or elemental). Must be in same order as 'sum_vars'");

  return params;
}

RedbackContinuationTangentAux::RedbackContinuationTangentAux(const InputParameters & parameters)
  : AuxNodalScalarKernel(parameters),
    _ds_old_param(getParam<Real>("ds_old")),
    _nb_sum_vars(coupledComponents("sum_vars")),
    _nb_sum_vars_old(coupledComponents("sum_vars_old")),
    _nb_sum_vars_older(coupledComponents("sum_vars_older")),
    _sum_vars(_nb_sum_vars),
    _sum_vars_old(_nb_sum_vars_old),
    _sum_vars_older(_nb_sum_vars_older)
{
  // Check user passed correct number of variables & old & older
  if (_nb_sum_vars_old != _nb_sum_vars || _nb_sum_vars_older != _nb_sum_vars)
    mooseError("RedbackContinuationTangentAux requires three inputs "
               "of same size: 'sum_vars' (size ",
               _nb_sum_vars,
               "), 'sum_vars_old' (size ",
               _nb_sum_vars_old,
               "), 'sum_vars_older' (size ",
               _nb_sum_vars_older,
               ").");

  // Get pointers to variables and
  // find if we have nodal or elemental coupled variables
  _are_nodal_vars_involved = false;
  _are_elemental_vars_involved = false;
  for (unsigned int i = 0; i < _nb_sum_vars; ++i)
  {
    _sum_vars[i] = &coupledValue("sum_vars", i);
    _sum_vars_old[i] = &coupledValue("sum_vars_old", i);
    _sum_vars_older[i] = &coupledValue("sum_vars_older", i);
    if ((getVar("sum_vars", i))->isNodal())
      _are_nodal_vars_involved = true;
    else
      _are_elemental_vars_involved = true;
  }
}

RedbackContinuationTangentAux::~RedbackContinuationTangentAux() {}

void
RedbackContinuationTangentAux::compute()
{
  int nb_nodes = _subproblem.mesh().getMesh().n_nodes();
  std::vector<dof_id_type> all_node_ids(nb_nodes);

  // const libMesh::MeshBase::node_iterator begin = _subproblem.mesh().getMesh().nodes_begin();
  // const libMesh::MeshBase::node_iterator end = _subproblem.mesh().getMesh().nodes_end();
  // for (libMesh::MeshBase::node_iterator k = begin; k != end; ++k)
  for (int k = 0; k < nb_nodes; ++k)
  {
    all_node_ids[k] = k;
  }
  _subproblem.reinitNodes(all_node_ids, _tid); // compute variables at nodes

  for (_i = 0; _i < _var.order(); ++_i) // _var.order()=1 for a scalar
  {
    Real value = computeValue();
    _communicator.sum(value);
    _var.setValue(_i, value); // update variable data, which is referenced by other kernels, so the
                              // value is up-to-date
  }
}

Real
RedbackContinuationTangentAux::computeValue()
{
  Real sum = 0;

  for (unsigned int k = 0; k < _nb_sum_vars; k++)
  {
    for (unsigned int i = 0; i < _sum_vars[k]->size(); i++)
    {
      sum += ((*_sum_vars[k])[i] - (*_sum_vars_old[k])[i]) *
             ((*_sum_vars_old[k])[i] - (*_sum_vars_older[k])[i]);
    }
  }
  sum = sum / _ds_old_param;
  return sum;
}
