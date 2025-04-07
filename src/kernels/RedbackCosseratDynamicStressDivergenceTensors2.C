/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#include "RedbackCosseratDynamicStressDivergenceTensors2.h"
#include "Material.h"
#include "RankFourTensor.h"
#include "ElasticityTensorTools.h"
#include "RankTwoTensor.h"
#include "MooseMesh.h"

InputParameters 
RedbackCosseratDynamicStressDivergenceTensors2::validParams()
{
  InputParameters params = DynamicStressDivergenceTensors::validParams();
  params.addCoupledVar("wc_x", "The Cosserat rotation about x");
  params.addCoupledVar("wc_y", "The Cosserat rotation about y");
  params.addCoupledVar("wc_z", "The Cosserat rotation about z");
  return params;
}

RedbackCosseratDynamicStressDivergenceTensors2::RedbackCosseratDynamicStressDivergenceTensors2(const InputParameters & parameters) :
    DynamicStressDivergenceTensors(parameters),
    _wc_x_var(coupled("wc_x")),
    _wc_y_var(coupled("wc_y")),
    _wc_z_var(coupled("wc_z"))
{
}

Real
RedbackCosseratDynamicStressDivergenceTensors2::computeQpOffDiagJacobian(unsigned int jvar)
{
  unsigned int coupled_component = 3;

  // What does 2D look like here?
  if (jvar == _wc_x_var)
    coupled_component = 0;
  else if (jvar == _wc_y_var)
    coupled_component = 1;
  else if (jvar == _wc_z_var)
    coupled_component = 2;

  if (coupled_component < 3)
    return //_Jacobian_mult[_qp].elasticJacobianwc(_component, coupled_component, _grad_test[_i][_qp], _phi[_j][_qp]*(1+_alpha+_zeta/_dt));
    ElasticityTensorTools::elasticJacobianWC(_Jacobian_mult[_qp], _component, coupled_component, _grad_test[_i][_qp], _phi[_j][_qp]*(1+_alpha+_zeta[_qp]/_dt));
  else;

  return StressDivergenceTensors::computeQpOffDiagJacobian(jvar);
}
