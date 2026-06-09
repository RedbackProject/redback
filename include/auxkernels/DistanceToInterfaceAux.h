//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#ifdef LIBIGL_ENABLED

#include "AuxKernel.h"
#include <igl/readSTL.h>
#include <igl/AABB.h>

class DistanceToInterfaceAux : public VectorAuxKernel
{
public:
  DistanceToInterfaceAux(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual RealVectorValue computeValue() override;

  std::string _stl_name;

  Eigen::MatrixXd _V;
  Eigen::MatrixXi _F;
  igl::AABB<Eigen::MatrixXd,3> _tree;
  Eigen::MatrixXd _N;
};

#endif
