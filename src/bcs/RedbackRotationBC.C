/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "RedbackRotationBC.h"
#include "MooseMesh.h"


InputParameters 
RedbackRotationBC::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  params += MaterialPropertyInterface::validParams();
//  params.addCoupledVar("disp_y", "the disp in y");
//  params.addCoupledVar("disp_z", "the disp in z");
//  params.addCoupledVar("wc_x", "The Cosserat rotation about x");
//  params.addCoupledVar("wc_y", "The Cosserat rotation about y");
//  params.addCoupledVar("wc_z", "The Cosserat rotation about z");
  params.addRequiredParam<unsigned int>("component1", "An integer corresponding to the direction the variable this kernel acts in. (0 for x, 1 for y, 2 for z)");
  params.addRequiredParam<unsigned int>("component2", "An integer corresponding to the direction the variable this kernel acts in. (0 for x, 1 for y, 2 for z)");
//  params.addRequiredParam<unsigned int>("value", "value of the stress imposed");

  return params;
}

RedbackRotationBC::RedbackRotationBC(const InputParameters & parameters) :
    IntegratedBC(parameters),
//    _stress(getMaterialPropertyByName<RankTwoTensor>("stress")),
    _component1(getParam<unsigned int>("component1")),
    _component2(getParam<unsigned int>("component2"))
//    _ndisp(coupledComponents("displacements")),

{
//  if (_ndisp)
//  {
//    for (unsigned int i = 0; i < _ndisp; ++i)
//    {
//      _disp[i] = &coupledValue("displacements", i);
//      _disp_var[i] = coupled("displacements", i);
//    }
//  }
}

Real
RedbackRotationBC::computeQpResidual()
{
  return -_test[_i][_qp];//*_stress[_qp](i,j)
}
