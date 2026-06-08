//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PerzynaStressUpdate.h"

registerMooseObject("SolidMechanicsApp", PerzynaStressUpdate);
registerMooseObject("SolidMechanicsApp", ADPerzynaStressUpdate);


template <bool is_ad>
InputParameters
PerzynaStressUpdateTempl<is_ad>::validParams()
{
  InputParameters params = RadialReturnPerzynaStressUpdateBaseTempl<is_ad>::validParams();
  params.addClassDescription(
      "This class uses the stress update material in a radial return isotropic power law plasticity "
      "model. This class can be used in conjunction with other plasticity and creep materials "
      "for more complex simulations.");

  // Linear strain hardening parameters
  params.addCoupledVar("temperature", "Coupled temperature");
  params.addRequiredParam<Real>("viscosity", "Leading coefficient in power-law equation");
  params.addRequiredParam<Real>("melting_viscosity", "Viscosity at melting temperature");
  params.addRequiredParam<Real>("yield_stress", "Initial yield stress of the material");
  params.addParam<Real>("alpha_yield", 0.0, "Thermal softening coefficient of yield stress");
  params.addParam<Real>("alpha_Viscous", 0.0, "Thermal softening coefficient of viscosity");
  params.addRequiredParam<Real>("n_exponent", "Exponent on effective stress in power-law equation");
  params.addParam<Real>("m_exponent", 0.0, "Exponent on time in power-law equation");
  params.addRequiredParam<Real>("activation_energy", "Activation energy");
  params.addParam<Real>("gas_constant", 8.3143, "Universal gas constant");
  params.addParam<Real>("start_time", 0.0, "Start time (if not zero)");
  return params;
}

template <bool is_ad>
PerzynaStressUpdateTempl<is_ad>::PerzynaStressUpdateTempl(
    const InputParameters & parameters)
  : RadialReturnPerzynaStressUpdateBaseTempl<is_ad>(parameters),
    _temperature(this->isParamValid("temperature")
                     ? &this->template coupledGenericValue<is_ad>("temperature")
                     : nullptr),
    _viscosity(this->template getParam<Real>("viscosity")),
    _melting_viscosity(this->template getParam<Real>("melting_viscosity")),
    _yield_stress(this->template getParam<Real>("yield_stress")),
    _alpha_yield(this->template getParam<Real>("alpha_yield")),
    _alpha_Viscous(this->template getParam<Real>("alpha_Viscous")),
    _n_exponent(this->template getParam<Real>("n_exponent")),
    _m_exponent(this->template getParam<Real>("m_exponent")),
    _activation_energy(this->template getParam<Real>("activation_energy")),
    _gas_constant(this->template getParam<Real>("gas_constant")),
    _start_time(this->template getParam<Real>("start_time")),
    _effective_viscosity(1.0)
{
  if (_start_time < this->_app.getStartTime() && (std::trunc(_m_exponent) != _m_exponent))
    this->paramError("start_time",
                     "Start time must be equal to or greater than the Executioner start_time if a "
                     "non-integer m_exponent is used");
}

template <bool is_ad>
void
PerzynaStressUpdateTempl<is_ad>::computeStressInitialize(
    const GenericReal<is_ad> & effective_trial_stress,
    const GenericRankFourTensor<is_ad> & elasticity_tensor)
{
  using std::exp, std::pow;

  RadialReturnStressUpdateTempl<is_ad>::computeStressInitialize(effective_trial_stress,
                                                                              elasticity_tensor);

  if (_temperature)
  {
    GenericReal<is_ad> visc1 = _melting_viscosity * exp(_activation_energy / (_gas_constant * (*_temperature)[_qp]));
    GenericReal<is_ad> visc2 = _viscosity * exp(((*_temperature)[_qp] - 273.0) / (273.0 * _alpha_Viscous));
    _effective_viscosity = 1.0 / std::min(visc1, visc2);
  }
  else
    _effective_viscosity = 1 / _melting_viscosity;

  if ((_t - _start_time) > 0)
    _exp_time = pow(_t - _start_time, _m_exponent);
  else
    _exp_time = 1;
}

template <bool is_ad>
template <typename ScalarType>
ScalarType
PerzynaStressUpdateTempl<is_ad>::computeResidualInternal(const GenericReal<is_ad> & effective_trial_stress, 
                                                         const ScalarType & scalar)
{
  using std::pow;

  const ScalarType effective_yield = _yield_stress * (1 - ((*_temperature)[_qp] - 273.0) * _alpha_yield);

  const ScalarType f = effective_trial_stress / std::sqrt(3) - effective_yield - _three_shear_modulus * scalar;

  if (MetaPhysicL::raw_value(f) <= 0)
    return -scalar;
  
  const ScalarType overstress = f / effective_yield;

  const ScalarType plastic_rate =  _effective_viscosity * pow(overstress, _n_exponent) * _exp_time;

  return plastic_rate * _dt - scalar;
}

template <bool is_ad>
GenericReal<is_ad>
PerzynaStressUpdateTempl<is_ad>::computeDerivative(
    const GenericReal<is_ad> & effective_trial_stress,
    const GenericReal<is_ad> & scalar)
{

  using std::exp, std::pow;

  const GenericReal<is_ad> effective_yield = _yield_stress * (1 - ((*_temperature)[_qp] - 273.0) * _alpha_yield);

  const GenericReal<is_ad> f = effective_trial_stress / std::sqrt(3) - effective_yield - _three_shear_modulus * scalar;
  if (MetaPhysicL::raw_value(f) <= 0)
    return -1;
  
  const GenericReal<is_ad> overstress = f / effective_yield;

  const GenericReal<is_ad> plastic_rate =  ( - _n_exponent * _three_shear_modulus * _effective_viscosity / effective_yield) * pow(overstress, _n_exponent - 1) * _exp_time;

  return plastic_rate * _dt - 1;
}




template <bool is_ad>
Real
PerzynaStressUpdateTempl<is_ad>::computeStrainEnergyRateDensity(
    const GenericMaterialProperty<RankTwoTensor, is_ad> & stress,
    const GenericMaterialProperty<RankTwoTensor, is_ad> & strain_rate)
{
  if (_n_exponent <= 1)
    return 0.0;

  Real plastic_factor = _n_exponent / (_n_exponent + 1);
  
  return MetaPhysicL::raw_value(plastic_factor * stress[_qp].doubleContraction(strain_rate[_qp]));
}

template <bool is_ad>
void
PerzynaStressUpdateTempl<is_ad>::computeStressFinalize(
    const GenericRankTwoTensor<is_ad> & plastic_strain_increment)
{
  _plastic_strain[_qp] += plastic_strain_increment;
}

template <bool is_ad>
void
PerzynaStressUpdateTempl<is_ad>::resetIncrementalMaterialProperties()
{
  _plastic_strain[_qp] = _plastic_strain_old[_qp];
}

template <bool is_ad>
bool
PerzynaStressUpdateTempl<is_ad>::substeppingCapabilityEnabled()
{
  return this->_use_substepping != RadialReturnStressUpdateTempl<is_ad>::SubsteppingType::NONE;
}

template class PerzynaStressUpdateTempl<false>;
template class PerzynaStressUpdateTempl<true>;
template Real PerzynaStressUpdateTempl<false>::computeResidualInternal<Real>(const Real &,
                                                                                   const Real &);
template ADReal
PerzynaStressUpdateTempl<true>::computeResidualInternal<ADReal>(const ADReal &,
                                                                      const ADReal &);
template ChainedReal
PerzynaStressUpdateTempl<false>::computeResidualInternal<ChainedReal>(const Real &,
                                                                            const ChainedReal &);
template ChainedADReal
PerzynaStressUpdateTempl<true>::computeResidualInternal<ChainedADReal>(const ADReal &,
                                                                             const ChainedADReal &);
