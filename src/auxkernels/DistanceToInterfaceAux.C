/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#ifdef LIBIGL_ENABLED

#include "DistanceToInterfaceAux.h"
#include <igl/point_mesh_squared_distance.h>
#include <igl/AABB.h>

registerMooseObject("RedbackApp", DistanceToInterfaceAux);

InputParameters
DistanceToInterfaceAux::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();
  params.addClassDescription(
      "Compute the minimal distance to the stl interface at every point of the sideset");
  params.addRequiredParam<std::string>("stl_name","name of the stl file (with the extension)");
  return params;
}

DistanceToInterfaceAux::DistanceToInterfaceAux(const InputParameters & parameters)
  : VectorAuxKernel(parameters),
    _stl_name(getParam<std::string>("stl_name"))
{
  igl::readSTL(_stl_name, _V, _F, _N);
  _tree.init(_V,_F);
}

RealVectorValue
DistanceToInterfaceAux::computeValue()
{
  Eigen::VectorXd sqrD;
  Eigen::VectorXi I;
  Eigen::MatrixXd C;
  Eigen::MatrixXd P = (Eigen::MatrixXd(1,3)<<
       (*_current_node)(0),(*_current_node)(1),(*_current_node)(2)).finished();
  // igl::point_mesh_squared_distance(P,_V,_F,sqrD,I,C);
  _tree.squared_distance(_V,_F,P,sqrD,I,C);
  Eigen::MatrixXd D = C-P;
  // std::cout<<sqrD(0)<<std::endl;
  return RealVectorValue(D(0,0),D(0,1),D(0,2));
}

#endif