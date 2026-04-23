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

#pragma once

#include "ADMaterial.h"

class ADPyLinearMaterial : public ADMaterial
{
public:
  static InputParameters validParams();
  ADPyLinearMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  /// Lateral displacement variable
  const ADVariableValue & _y;

  /// Output distributed soil reaction p = k * y
  ADMaterialProperty<Real> & _py_force;

  /// Linear Winkler stiffness, possibly depth-dependent: k = k0 + k_rate * depth
  const Real _k0;
  const Real _k_rate;

  /// Which coordinate is treated as depth: 0=x, 1=y, 2=z
  const unsigned int _depth_component;

  /// Reference coordinate for zero depth
  const Real _depth_origin;

  /// Clamp negative depth to zero if desired
  const bool _positive_depth_only;
};