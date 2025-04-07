/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#include "RedbackDynamicMomentBalancing.h"
#include "PermutationTensor.h"
#include "Material.h"
#include "RankFourTensor.h"
#include "ElasticityTensorTools.h"
#include "RankTwoTensor.h"

registerMooseObject("RedbackApp", RedbackDynamicMomentBalancing);

InputParameters 
RedbackDynamicMomentBalancing::validParams()
{
  InputParameters params = RedbackMomentBalancing::validParams();
  params.addClassDescription("Residual due to stress related Rayleigh damping and HHT time integration terms ");
  params.addParam<Real>("zeta", 0, "zeta parameter for the Rayleigh damping");
  params.addParam<Real>("alpha", 0, "alpha parameter for HHT time integration");
  return params;
}

RedbackDynamicMomentBalancing::RedbackDynamicMomentBalancing(const InputParameters & parameters) :
RedbackMomentBalancing(parameters),
_stress_older(getMaterialPropertyOlderByName<RankTwoTensor>("stress")),
_stress_old(getMaterialPropertyOldByName<RankTwoTensor>("stress")),
_zeta(getParam<Real>("zeta")),
_alpha(getParam<Real>("alpha"))
{
}

Real
RedbackDynamicMomentBalancing::computeQpResidual()
{
  if ((_dt > 0)){
    Real the_sum = 0.0;
    Real the_sum_old = 0.0;
    Real the_sum_older = 0.0;


    for (unsigned int j = 0; j < LIBMESH_DIM; ++j){
      for (unsigned int k = 0; k < LIBMESH_DIM; ++k){
        the_sum += PermutationTensor::eps(_component, j, k)*_stress[_qp](j, k);
        the_sum_old += PermutationTensor::eps(_component, j, k)*_stress_old[_qp](j, k);
        the_sum_older += PermutationTensor::eps(_component, j, k)*_stress_older[_qp](j, k);
      }
    }

    return the_sum * _test[_i][_qp] *(1+_alpha+(1+_alpha)*_zeta/_dt)
      -(_alpha+(1+2*_alpha)*_zeta/_dt) * the_sum_old * _test[_i][_qp]
      + (_alpha*_zeta/_dt) * the_sum_older * _test[_i][_qp] ;
  }
  else
    return 0;
}

Real
RedbackDynamicMomentBalancing::computeQpJacobian()
{
  return //_Jacobian_mult[_qp].momentJacobianwc(_component, _component, _test[_i][_qp], _phi[_j][_qp]*(1+_alpha+_zeta/_dt));
  ElasticityTensorTools::momentJacobianWC(_Jacobian_mult[_qp], _component, _component, _test[_i][_qp], _phi[_j][_qp]*(1+_alpha+_zeta/_dt));
}

Real
RedbackDynamicMomentBalancing::computeQpOffDiagJacobian(unsigned int jvar)
{
  unsigned int coupled_component = 3;

  // What does 2D look like here?
  if (jvar == _xdisp_var)
    coupled_component = 0;
  else if (jvar == _ydisp_var)
    coupled_component = 1;
  else if (jvar == _zdisp_var)
    coupled_component = 2;

  if (coupled_component < 3)
    return //_Jacobian_mult[_qp].momentJacobian(_component, coupled_component, _test[_i][_qp], _grad_phi[_j][_qp]*(1+_alpha+_zeta/_dt));
ElasticityTensorTools::momentJacobian(_Jacobian_mult[_qp], _component, coupled_component, _test[_i][_qp],  _grad_phi[_j][_qp]*(1+_alpha+_zeta/_dt));
  // What does 2D look like here?
  if (jvar == _wc_x_var)
    coupled_component = 0;
  else if (jvar == _wc_y_var)
    coupled_component = 1;
  else if (jvar == _wc_z_var)
    coupled_component = 2;

  if (coupled_component < 3)
    return //_Jacobian_mult[_qp].momentJacobianwc(_component, coupled_component, _test[_i][_qp], _phi[_j][_qp]*(1+_alpha+_zeta/_dt));
ElasticityTensorTools::momentJacobianWC(_Jacobian_mult[_qp], _component, coupled_component, _test[_i][_qp], _phi[_j][_qp]*(1+_alpha+_zeta/_dt));
  return 0;
}
