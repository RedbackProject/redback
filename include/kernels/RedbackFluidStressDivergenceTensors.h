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

#ifndef REDBACKFLUIDSTRESSDIVERGENCETENSORS_H
#define REDBACKFLUIDSTRESSDIVERGENCETENSORS_H

#include "Kernel.h"
#include "RankTwoTensor.h"

/**
 * RedbackFluidStressDivergenceTensors mostly copies from StressDivergence.  There are small changes
 * to use RankFourTensor and RankTwoTensors instead of SymmElasticityTensors and SymmTensors.  This
 * is done to allow for more mathematical transparancy.
 */
class RedbackFluidStressDivergenceTensors : public Kernel
{
public:
  RedbackFluidStressDivergenceTensors(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  // VariableValue & _pore_pres;
  const VariableGradient & _grad_pore_pressure;
  RealVectorValue _fluid_stress_row;

  const MaterialProperty<RankTwoTensor> & _fluid_stress;
  // const MaterialProperty<RankFourTensor> & _Jacobian_fluid_mult;

  const unsigned int _component;

  const MaterialProperty<Real> & _fluid_density;
  const MaterialProperty<Real> & _reynolds_number;
  const MaterialProperty<Real> & _viscosity_ratio;

private:
  // const bool _xdisp_coupled;
  // const bool _ydisp_coupled;
  // const bool _zdisp_coupled;
  // const bool _temp_coupled;

  const unsigned int _vel_fluid_x_var;
  const unsigned int _vel_fluid_y_var;
  const unsigned int _vel_fluid_z_var;
  // const unsigned int _temp_var;
  const unsigned int _porepressure_var;

  const MaterialProperty<RealVectorValue> & _gravity_term;
  bool _stokes;
};

#endif // REDBACKFLUIDSTRESSDIVERGENCETENSORS_H
