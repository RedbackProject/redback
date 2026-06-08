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

#ifndef SIDEINTEGRALVOIDPOSTPROCESSOR_H
#define SIDEINTEGRALVOIDPOSTPROCESSOR_H

#include "SideIntegralPostprocessor.h"
#include "MooseVariableInterface.h"

/**
 * This postprocessor computes a volume integral of the specified variable.
 *
 * Note that specializations of this integral are possible by deriving from this
 * class and overriding computeQpIntegral().
 */
class SideIntegralVoidPostprocessor : public SideIntegralPostprocessor,
                                      public MooseVariableInterface<Real>
{
public:
  SideIntegralVoidPostprocessor(const InputParameters & parameters);
  static InputParameters validParams();

protected:
  virtual Real computeQpIntegral() override;

  const VariableValue & _u;
  const unsigned int _i;
  const unsigned int _j;
  /// Holds the solution at current quadrature points
  const VariableValue & _disp_x;
  const VariableValue & _disp_y;
  const VariableValue & _disp_z;

  const VariableValue & _dispx_dot;
  const VariableValue & _dispy_dot;
  const VariableValue & _dispz_dot;

  bool _vel;
};

#endif
