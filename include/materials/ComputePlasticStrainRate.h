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

#ifndef COMPUTEPLASTICSTRAINRATE_H
#define COMPUTEPLASTICSTRAINRATE_H

#include "DerivativeMaterialInterface.h"
#include "Material.h"
#include "RankFourTensor.h"
#include "RankTwoTensor.h"

/**
 * ComputePlasticStrainRate computes stress * (plastic_strain - plastic_strain_old)
 * and, if currentlyComputingJacobian, then the derivative of this quantity wrt total strain
 */
class ComputePlasticStrainRate : public DerivativeMaterialInterface<Material>
{
public:
  ComputePlasticStrainRate(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual void computeQpProperties() override;

  /// optional parameter that allows multiple mechanics materials to be defined
  std::string _base_name;

  /// plastic strain
  const MaterialProperty<RankTwoTensor> & _plastic_strain;

  /// old value of plastic strain
  const MaterialProperty<RankTwoTensor> & _plastic_strain_old;

  MaterialProperty<RankTwoTensor> & _plastic_strain_rate;
};

#endif // COMPUTEPLASTICSTRAINRATE_H
