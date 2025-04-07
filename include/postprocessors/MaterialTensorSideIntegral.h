/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef MATERIALTENSORSIDEINTEGRAL_H
#define MATERIALTENSORSIDEINTEGRAL_H

#include "RankTwoTensor.h"
#include "SideIntegralPostprocessor.h"

/**
 * This postprocessor computes an element integral of a
 * component of a material tensor as specified by the user-supplied indices.
 */
class MaterialTensorSideIntegral : public SideIntegralPostprocessor
{
public:
  MaterialTensorSideIntegral(const InputParameters & parameters);
  static InputParameters validParams();

protected:
  virtual Real computeQpIntegral();

private:
  const MaterialProperty<RankTwoTensor> & _tensor;
  const unsigned int _i;
  const unsigned int _j;
};
#endif // MATERIALTENSORSIDEINTEGRAL_H
