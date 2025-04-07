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

#include "RedbackPolarTensorMaterialAux.h"

registerMooseObject("RedbackApp", RedbackPolarTensorMaterialAux);

InputParameters
RedbackPolarTensorMaterialAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addRequiredParam<MaterialPropertyName>("rank_two_tensor",
                                                "The rank two material tensor name");
  params.addRequiredRangeCheckedParam<unsigned int>(
      "index_i",
      "index_i >= 0 & index_i <= 1",
      "An index i to select r (i=0) or theta (i=1) output");
  params.addRequiredRangeCheckedParam<unsigned int>(
      "index_j",
      "index_j >= 0 & index_j <= 1",
      "An index j to select r (i=0) or theta (i=1) output");
  return params;
}

RedbackPolarTensorMaterialAux::RedbackPolarTensorMaterialAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _tensor(getMaterialProperty<RankTwoTensor>("rank_two_tensor")),
    _i(getParam<unsigned int>("index_i")),
    _j(getParam<unsigned int>("index_j"))
{
}

Real
RedbackPolarTensorMaterialAux::computeValue()
{
  // Real x = (*_current_node)(0); // WRONG
  // Real y = (*_current_node)(1);
  // Real z = (*_current_node)(2);

  RealVectorValue centroid = _current_elem->centroid();
  Real x = centroid(0);
  Real y = centroid(1);
  Real r = std::sqrt(x * x + y * y);
  Real epsilon = 1e-10; // tolerance on radius to avoid singularity
  Real theta;

  // Real argument = y/x;
  // Real theta = std::atan(argument)*(argument <0 ? -1: 1); //(argument < 0 ? libMesh::pi -
  // std::atan(argument): std::atan(argument));
  if (r < epsilon)
    theta = 0;
  else
  {
    Real argument = x / r;
    theta = std::acos(argument) * (y < 0 ? -1 : 1);
  }

  /*Real theta;
  if ((x < 0) && (y < 0))
  {
    theta = -(libMesh::pi - std::atan(argument));
  } else if ((x < 0) && (y > 0))
  {
    theta = libMesh::pi + std::atan(argument);
  } else
  {
    theta = std::atan(argument);
  }*/

  Real result; // value returned

  if ((_i == 0) && (_j == 0))
  {
    // User is asking for tensor_r_r
    result = _tensor[ _qp ](0, 0) * std::cos(theta) * std::cos(theta) +
             _tensor[ _qp ](0, 1) * std::sin(theta) * std::cos(theta) +
             _tensor[ _qp ](1, 0) * std::sin(theta) * std::cos(theta) +
             _tensor[ _qp ](1, 1) * std::sin(theta) * std::sin(theta);
  }
  else if ((_i == 1) && (_j == 1))
  {
    // User is asking for tensor_theta_theta
    result = _tensor[ _qp ](0, 0) * std::sin(theta) * std::sin(theta) -
             _tensor[ _qp ](0, 1) * std::sin(theta) * std::cos(theta) -
             _tensor[ _qp ](1, 0) * std::sin(theta) * std::cos(theta) +
             _tensor[ _qp ](1, 1) * std::cos(theta) * std::cos(theta);
  }
  else if ((_i == 0) && (_j == 1))
  {
    // User is asking for tensor_r_theta
    result = -_tensor[ _qp ](0, 0) * std::sin(theta) * std::cos(theta) +
             _tensor[ _qp ](0, 1) * std::cos(theta) * std::cos(theta) -
             _tensor[ _qp ](1, 0) * std::sin(theta) * std::sin(theta) +
             _tensor[ _qp ](1, 1) * std::sin(theta) * std::cos(theta);
  }
  else if ((_i == 1) && (_j == 0))
  {
    // User is asking for tensor_theta_r
    result = -_tensor[ _qp ](0, 0) * std::sin(theta) * std::cos(theta) +
             _tensor[ _qp ](1, 0) * std::cos(theta) * std::cos(theta) -
             _tensor[ _qp ](0, 1) * std::sin(theta) * std::sin(theta) +
             _tensor[ _qp ](1, 1) * std::sin(theta) * std::cos(theta);
  }
else
{
  std::cout << " wrong indexes for the transformation in polar coordinates " << std::endl;
}

  return result;
}
