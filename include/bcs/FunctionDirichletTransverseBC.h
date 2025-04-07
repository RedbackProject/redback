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
/****************************************************************/
/* Dirichlet boundary condition to impose displacement in a     */
/* direction transversal to element normal vector, by taking a  */
/* vectorial product of the normal with a given vector          */
/****************************************************************/

#ifndef FUNCTIONDIRICHLETTRANSVERSEBC
#define FUNCTIONDIRICHLETTRANSVERSEBC

#include "DirichletBCBase.h"

class Function;

/**
 * Defines a boundary condition that forces the value to be a user specified
 * function at the boundary.
 */
class FunctionDirichletTransverseBC : public DirichletBCBase
{
public:
  FunctionDirichletTransverseBC(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  /**
   * Evaluate the function at the current quadrature point and timestep.
   */
  virtual Real computeQpValue();

  const VariableValue & _u_old;

  /// Function being used for evaluation of this BC
  const Function & _func;

  /// Center point to calculate transversal direction for boundary point.
  const RealVectorValue & _center;

  /// Axis of rotation, vector to calculate transversal direction for boundary
  /// point.
  const RealVectorValue & _axis;

  const Real _angular_velocity;

  /// Direction index (0 for X, 1 for Y, 2 for Z), since we can't really rely on
  /// variable name "disp_x"...
  const unsigned int & _dir_index;
};

#endif /* FUNCTIONDIRICHLETTRANSVERSEBC */
