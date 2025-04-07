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

#ifndef REDBACKPOROMECHHO_H
#define REDBACKPOROMECHHO_H

#include "Kernel.h"

class RedbackPoroMechHO : public Kernel
{
public:
  RedbackPoroMechHO(const InputParameters & parameters);
  static InputParameters validParams();

  virtual ~RedbackPoroMechHO();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  const MaterialProperty<Real> & _poromech_kernel;
  const MaterialProperty<Real> & _poromech_jac;

  // number of coupled displacement variables
  unsigned int _ndisp;

  // MOOSE variable number for the displacement variables
  std::vector<unsigned int> _disp_var;
  
  const MaterialProperty<Real> & _solid_thermal_expansion;

private:
  const bool _temp_coupled;
  const unsigned int _temp_var;
  Real _time_factor;
};

#endif /* REDBACKPOROMECHHO_H */