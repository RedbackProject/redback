//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "RadialReturnPerzynaStressUpdateBase.h"

template <bool is_ad>
InputParameters
RadialReturnPerzynaStressUpdateBaseTempl<is_ad>::validParams()
{
  InputParameters params = RadialReturnStressUpdateTempl<is_ad>::validParams();
  params.set<std::string>("effective_inelastic_strain_name") = "effective_plastic_strain";
  return params;
}

template <bool is_ad>
RadialReturnPerzynaStressUpdateBaseTempl<is_ad>::RadialReturnPerzynaStressUpdateBaseTempl(
    const InputParameters & parameters)
  : RadialReturnStressUpdateTempl<is_ad>(parameters),
    _plastic_strain(this->template declareGenericProperty<RankTwoTensor, is_ad>(this->_base_name +
                                                                              "plastic_strain")),
    _plastic_strain_old(
        this->template getMaterialPropertyOld<RankTwoTensor>(this->_base_name + "plastic_strain"))
{
}

template <bool is_ad>
void
RadialReturnPerzynaStressUpdateBaseTempl<is_ad>::initQpStatefulProperties()
{
  _plastic_strain[_qp].zero();
  
  RadialReturnStressUpdateTempl<is_ad>::initQpStatefulProperties();
}

template <bool is_ad>
void
RadialReturnPerzynaStressUpdateBaseTempl<is_ad>::propagateQpStatefulProperties()
{
  _plastic_strain[_qp] = _plastic_strain_old[_qp];

  propagateQpStatefulPropertiesRadialReturn();
}

template <bool is_ad>
Real
RadialReturnPerzynaStressUpdateBaseTempl<is_ad>::computeStressDerivative(
    const Real /*effective_trial_stress*/, const Real /*scalar*/)
{
  mooseError("computeStressDerivative called: no stress derivative computation is needed for AD");
}

template <>
Real
RadialReturnPerzynaStressUpdateBaseTempl<false>::computeStressDerivative(
    const Real effective_trial_stress, const Real scalar)
{  
  return -(computeDerivative(effective_trial_stress, scalar) + 1.0) /  this->_three_shear_modulus;
}

template <bool is_ad>
void
RadialReturnPerzynaStressUpdateBaseTempl<is_ad>::computeStressFinalize(
    const GenericRankTwoTensor<is_ad> & plastic_strain_increment)
{

  _plastic_strain[_qp] = _plastic_strain_old[_qp] + plastic_strain_increment;

}

template class RadialReturnPerzynaStressUpdateBaseTempl<false>;
template class RadialReturnPerzynaStressUpdateBaseTempl<true>;
