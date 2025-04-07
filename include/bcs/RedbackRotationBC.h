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

#ifndef REDBACKROTATIONBC_H
#define REDBACKROTATIONBC_H

#include "IntegratedBC.h"


class RedbackRotationBC : public IntegratedBC

{
public:
  RedbackRotationBC(const InputParameters & parameters);
  static InputParameters validParams();

protected:
  virtual Real computeQpResidual();

//  const VariableValue & _disp_x;
//  const VariableValue & _disp_y;
//  const VariableValue & _disp_z;
//  const VariableValue & _wc_x;
//  const VariableValue & _wc_y;
//  const VariableValue & _wc_z;


//  const VariableGradient & _grad_ux;
//  const VariableGradient & _grad_uy;
//  const VariableGradient & _grad_uz;

  Real _component1;
  Real _component2;
//  Real _scalar;

};

#endif //RedbackRotationBC_H
