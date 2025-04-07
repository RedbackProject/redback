/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "RedbackCosseratStressDivergenceTensors.h"
#include "Material.h"
#include "RankFourTensor.h"
#include "ElasticityTensorTools.h"
#include "RankTwoTensor.h"
#include "MooseMesh.h"

InputParameters 
RedbackCosseratStressDivergenceTensors::validParams()
{
  //InputParameters params = validParams<StressDivergenceTensors>();
  InputParameters params = RedbackStressDivergenceTensorsNew::validParams();
  params.addCoupledVar("wc_x", "The Cosserat rotation about x");
  params.addCoupledVar("wc_y", "The Cosserat rotation about y");
  params.addCoupledVar("wc_z", "The Cosserat rotation about z");
  return params;
}

RedbackCosseratStressDivergenceTensors::RedbackCosseratStressDivergenceTensors(const InputParameters & parameters) :
   //StressDivergenceTensors(parameters),
    RedbackStressDivergenceTensorsNew(parameters),
    _Jacobian_offdiag_bc(getMaterialPropertyByName<RankFourTensor>("coupled_Jacobian_off")),
    _Jacobian_offdiag_cb(getMaterialPropertyByName<RankFourTensor>("Jacobian_off")),
    _wc_x_var(coupled("wc_x")),
    _wc_y_var(coupled("wc_y")),
    _wc_z_var(coupled("wc_z"))
{
}

Real
RedbackCosseratStressDivergenceTensors::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real result = 0;
  unsigned int coupled_component = 3;
  //std::cout << "_wc_x_var=" << _wc_x_var <<std::endl;
  //std::cout << "_wc_y_var=" << _wc_y_var <<std::endl;
  //std::cout << "_wc_z_var=" << _wc_z_var <<std::endl;

  //int test  = _base_name.compare("coupled_");
//  std::string test_s = "coupled";
//  int test2  = test_s.compare("coupled");
//  std::cout << std::endl << "test s = " << test_s << std::endl;
//  std::cout << "_base_name = " << _base_name << std::endl;
  //std::cout << "_base_name.compare(coupled_) = " << test << std::endl;
//  std::cout << "coupled.compare(coupled) = " << test2 << std::endl;
    if (jvar == _wc_x_var)
      coupled_component = 0;
    else if (jvar == _wc_y_var)
      coupled_component = 1;
    else if (jvar == _wc_z_var)
      coupled_component = 2;

  //    std::cout << "coupled component = " << coupled_component << std::endl;

    if (coupled_component < 3){
      if (_base_name.compare("coupled_") == 0){
        result = ElasticityTensorTools::elasticJacobian(_Jacobian_offdiag_bc[_qp], _component, coupled_component, _grad_test[_i][_qp], _grad_phi[_j][_qp]);

        //result = _Jacobian_offdiag_bc[_qp].elasticJacobian(_component, coupled_component, _grad_test[_i][_qp], _grad_phi[_j][_qp]);
        //  std::cout << "Hello from the other siiiiiiide ***********************" << std::endl;
        }
        else{
          result = ElasticityTensorTools::elasticJacobianWC(_Jacobian_mult[_qp], _component, coupled_component, _grad_test[_i][_qp], _phi[_j][_qp]);
          //result = _Jacobian_mult[_qp].elasticJacobianwc(_component, coupled_component, _grad_test[_i][_qp], _phi[_j][_qp]);
          result += ElasticityTensorTools::elasticJacobian(_Jacobian_offdiag_cb[_qp], _component, coupled_component, _grad_test[_i][_qp], _grad_phi[_j][_qp]);
          //result += _Jacobian_offdiag_cb[_qp].elasticJacobian(_component, coupled_component, _grad_test[_i][_qp], _grad_phi[_j][_qp]);

      //  std::cout << "Hello from the other siiiiiiide 2" << std::endl;
        }
      }
    else
      {
        //result = StressDivergenceTensors::computeQpOffDiagJacobian(jvar);
        result = RedbackStressDivergenceTensorsNew::computeQpOffDiagJacobian(jvar);
      }


/*
      if (coupled_component < 3)
  else
  {
    // What does 2D look like here?
    if (jvar == _wc_x_var)
      coupled_component = 0;
    else if (jvar == _wc_y_var)
      coupled_component = 1;
    else if (jvar == _wc_z_var)
      coupled_component = 2;
  }

  if (coupled_component < 3)
    result = _Jacobian_mult[_qp].elasticJacobianwc(_component, coupled_component, _grad_test[_i][_qp], _phi[_j][_qp]);
    result += _Jacobian_offdiag_cb.elasticJacobian(_component, coupled_component, _grad_test[_i][_qp], _grad_phi[_j][_qp]);
  else
{
  result = StressDivergenceTensors::computeQpOffDiagJacobian(jvar);
}
  //if (dans le cas precis numero 3)
  //  result +=  un autre terme;
    //return _Jacobian_mult[_qp].elasticJacobianwc(_component, coupled_component, _grad_test[_i][_qp], _phi[_j][_qp]);
    //_Jacobian_offdiag_cb.elasticJacobian(_component, coupled_component, _grad_test[_i][_qp], _grad_phi[_j][_qp])

  //return StressDivergenceTensors::computeQpOffDiagJacobian(jvar);
*/

  return result;
}
