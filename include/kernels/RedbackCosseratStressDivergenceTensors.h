/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#ifndef RedbackCosseratStressDivergenceTensors_H
#define RedbackCosseratStressDivergenceTensors_H

//#include "StressDivergenceTensors.h"
#include "RedbackStressDivergenceTensorsNew.h"


//class RedbackCosseratStressDivergenceTensors : public StressDivergenceTensors
class RedbackCosseratStressDivergenceTensors : public RedbackStressDivergenceTensorsNew
{
public:
  RedbackCosseratStressDivergenceTensors(const InputParameters & parameters);
  static InputParameters validParams();

protected:
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);
  const MaterialProperty<RankFourTensor> & _Jacobian_offdiag_bc;
  const MaterialProperty<RankFourTensor> & _Jacobian_offdiag_cb;

private:
  const unsigned int _wc_x_var;
  const unsigned int _wc_y_var;
  const unsigned int _wc_z_var;
};

#endif //RedbackCosseratStressDivergenceTensors_H
