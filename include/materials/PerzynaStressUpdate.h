//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "RadialReturnPerzynaStressUpdateBase.h"

/**
 * This class uses the stress update material in a radial return isotropic creep
 * model.  This class is one of the basic radial return constitutive models; more complex
 * constitutive models combine creep and plasticity.
 *
 * This class inherits from RadialReturnPerzynaStressUpdateBase and must be used
 * in conjunction with ComputeMultipleInelasticStress.  This class calculates
 * creep based on stress, temperature, and time effects.  This class also
 * computes the creep strain as a stateful material property.
 */
template <bool is_ad>
class PerzynaStressUpdateTempl : public RadialReturnPerzynaStressUpdateBaseTempl<is_ad>
{
public:
  static InputParameters validParams();

  PerzynaStressUpdateTempl(const InputParameters & parameters);

  virtual Real computeStrainEnergyRateDensity(
      const GenericMaterialProperty<RankTwoTensor, is_ad> & stress,
      const GenericMaterialProperty<RankTwoTensor, is_ad> & strain_rate) override;

  virtual bool substeppingCapabilityEnabled() override;

  virtual void resetIncrementalMaterialProperties() override;

  virtual void
  computeStressInitialize(const GenericReal<is_ad> & effective_trial_stress,
                          const GenericRankFourTensor<is_ad> & elasticity_tensor) override;
  virtual GenericReal<is_ad> computeResidual(const GenericReal<is_ad> & effective_trial_stress,
                                             const GenericReal<is_ad> & scalar) override
  {
    return computeResidualInternal<GenericReal<is_ad>>(effective_trial_stress, scalar);
  }
  virtual GenericReal<is_ad> computeDerivative(const GenericReal<is_ad> & effective_trial_stress,
                                               const GenericReal<is_ad> & scalar) override;
  virtual void
  computeStressFinalize(const GenericRankTwoTensor<is_ad> & plastic_strain_increment) override;

protected:
  virtual GenericChainedReal<is_ad>
  computeResidualAndDerivative(const GenericReal<is_ad> & effective_trial_stress,
                               const GenericChainedReal<is_ad> & scalar) override
  {
    return computeResidualInternal<GenericChainedReal<is_ad>>(effective_trial_stress, scalar);
  }

  /// Temperature variable value
  const GenericVariableValue<is_ad> * const _temperature;

  /// Leading coefficient
  const Real _viscosity;

  /// Viscosity at melting temperature
  const Real _melting_viscosity;

  /// initial yield stress
  const Real _yield_stress;

  /// Thermal softening coefficient of yield stress
  const Real _alpha_yield;

  /// Thermal softening coefficient of viscosity
  const Real _alpha_Viscous;

  /// Exponent on the effective stress
  const Real _n_exponent;

  /// Exponent on time
  const Real _m_exponent;

  /// Activation energy for exp term
  const Real _activation_energy;

  /// Gas constant for exp term
  const Real _gas_constant;

  /// Simulation start time
  const Real _start_time;

  /// Exponential calculated from activiaction, gas constant, and temperature
  GenericReal<is_ad> _effective_viscosity;

  /// Exponential calculated from current time
  Real _exp_time;

  usingTransientInterfaceMembers;
  using RadialReturnPerzynaStressUpdateBaseTempl<is_ad>::_qp;
  using RadialReturnPerzynaStressUpdateBaseTempl<is_ad>::_three_shear_modulus;
  using RadialReturnPerzynaStressUpdateBaseTempl<is_ad>::_plastic_strain;
  using RadialReturnPerzynaStressUpdateBaseTempl<is_ad>::_plastic_strain_old;

private:
  template <typename ScalarType>
  ScalarType computeResidualInternal(const GenericReal<is_ad> & effective_trial_stress,
                                     const ScalarType & scalar);
};

typedef PerzynaStressUpdateTempl<false> PerzynaStressUpdate;
typedef PerzynaStressUpdateTempl<true> ADPerzynaStressUpdate;
