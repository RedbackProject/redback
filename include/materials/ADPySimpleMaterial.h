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

class ADPySimpleMaterial : public ADMaterial
{
public:
  static InputParameters validParams();
  ADPySimpleMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  /// Lateral displacement variable
  const ADVariableValue & _y;

  /// Output property: distributed soil reaction p(y,z)
  ADMaterialProperty<Real> & _py_force;
  /// Parameters for pult(z) = pult0 + pult_rate * depth
  const Real _pult0;
  const Real _pult_rate;

  /// Characteristic displacement controlling curvature
  const Real _y50;

  /// Which coordinate is "depth": 0=x, 1=y, 2=z
  const unsigned int _depth_component;

  /// Optional offset so depth can be measured from ground level
  const Real _depth_origin;

  /// If true, clamp depth below at zero
  const bool _positive_depth_only;
};