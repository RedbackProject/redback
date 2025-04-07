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

#ifndef REDBACKMECHDISSIP_H
#define REDBACKMECHDISSIP_H

#include "Kernel.h"
#include "RankTwoTensor.h"

class RedbackMechDissip : public Kernel
{
public:
  RedbackMechDissip(const InputParameters & parameters);
  static InputParameters validParams();

  virtual ~RedbackMechDissip();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  // const VariableValue & _pressure;
  const MaterialProperty<Real> & _mechanical_dissipation;
  const MaterialProperty<Real> & _mechanical_dissipation_jac;
  
  // d(plastic_heat)/d(total_strain)
  const MaterialProperty<RankTwoTensor> & _dplastic_heat_dstrain;
  const MaterialProperty<RankTwoTensor> & _dplastic_heat_dcurvature;
  
  // number of coupled displacement variables
  unsigned int _ndisp;

  // MOOSE variable number for the displacement variables
  std::vector<unsigned int> _disp_var;
  
    // number of coupled displacement variables
  unsigned int _nwc;

  // MOOSE variable number for the displacement variables
  std::vector<unsigned int> _wc_var;


private:
  Real _time_factor;
};

#endif /* REDBACKMECHDISSIP_H */
