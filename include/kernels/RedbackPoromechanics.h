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

#ifndef REDBACKPOROMECHANICS_H
#define REDBACKPOROMECHANICS_H

#include "Kernel.h"

/**
 * Kernel that is calling coupledDot
 */
class RedbackPoromechanics : public Kernel
{
public:
  RedbackPoromechanics(const InputParameters & parameters);
  static InputParameters validParams();

  virtual ~RedbackPoromechanics();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  const MaterialProperty<Real> & _poromech_kernel;
  const MaterialProperty<Real> & _poromech_jac;

  unsigned int _temp_var; // variable number of the temperature variable

private:
  Real _time_factor;
};

#endif /* REDBACKPOROMECHANICS_H */
