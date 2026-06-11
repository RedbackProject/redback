/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*     REDBACK - Rock mEchanics with Dissipative feedBACKs      */
/*                                                              */
/*              (c) 2014 CSIRO and UNSW Australia               */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*            Prepared by CSIRO and UNSW Australia              */
/*            But now with the big boys of TU Delft             */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef REDBACKMECHMATERIALCONTACT_H
#define REDBACKMECHMATERIALCONTACT_H

#include "RedbackMechMaterial.h"

class RedbackMechMaterialContact : public RedbackMechMaterial
{
public:
  static InputParameters validParams();
  RedbackMechMaterialContact(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;
  virtual void computeQpElasticityTensor() override;

  virtual void returnMap(const RankTwoTensor & sig_old,
                         const RankTwoTensor & delta_d,
                         const RankFourTensor & E_ijkl,
                         RankTwoTensor & dp,
                         RankTwoTensor & sig,
                         Real & p_y,
                         Real & q_y) override;

  virtual void getFlowTensor(const RankTwoTensor & sig,
                             Real q,
                             Real p,
                             Real q_y,
                             Real p_y,
                             Real yield_stress,
                             RankTwoTensor & flow_tensor_dev) override;

  virtual Real getFlowIncrement(Real sig_eqv,
                                Real p,
                                Real q_y,
                                Real p_y,
                                Real yield_stress) override;

  virtual void getJac(const RankTwoTensor & sig,
                      const RankFourTensor & E_ijkl,
                      Real flow_incr_dev,
                      Real q,
                      Real p,
                      Real p_y,
                      Real q_y,
                      Real yield_stress,
                      RankFourTensor & dresid_dsig) override;

  virtual void get_py_qy(Real p,
                         Real q,
                         Real & p_y,
                         Real & q_y,
                         Real yield_stress,
                         bool & is_plastic) override;

  Real computeCharacteristicHeight(const Elem & elem) const;
  Real computeReferenceHeight(const Elem & elem) const;
  Real computeContactReferenceHeight(const Elem & elem) const;
  Real computeCurrentHeight(const Real h_contact_0, const Real eps_n) const;

  void computeElementBasis(const Elem & elem,
                           RealVectorValue & normal,
                           RealVectorValue & tangent) const;

  Real computeNormalStrain(const RankTwoTensor & eps,
                           const RealVectorValue & normal) const;

  Real computeTangentialStrain(const RankTwoTensor & eps,
                               const RealVectorValue & normal,
                               const RealVectorValue & tangent) const;

  Real computeTangentialSlipRate(const Real h0,
                                 const Real gamma_t,
                                 const Real gamma_t_old) const;

  Real computeFrictionCoefficient(const Real vt) const;

protected:
  const MooseEnum _h_method;
  const MooseEnum _friction_model;
  const MooseEnum _reference_height_method;

  const Real _youngs_modulus_contact;
  const Real _alpha_n;
  const Real _alpha_t;
  const Real _gap_exponent;
  const Real _isotropic_stiffness_ratio;
  const Real _stabilization_poisson_ratio;
  const Real _min_gap;

  const Real _friction_coefficient_static;
  const Real _friction_coefficient_dynamic;
  const Real _friction_velocity_reference;

  MaterialProperty<Real> & _contact_h0;
  MaterialProperty<Real> & _contact_gap;
  MaterialProperty<Real> & _contact_reference_height;
  MaterialProperty<Real> & _contact_current_height;
  MaterialProperty<Real> & _contact_active;
  MaterialProperty<Real> & _contact_normal_strain;
  MaterialProperty<Real> & _contact_normal_stress;
  MaterialProperty<Real> & _contact_tangential_strain;
  const MaterialProperty<Real> & _contact_tangential_strain_old;
  MaterialProperty<Real> & _contact_tangential_stress;
  MaterialProperty<Real> & _contact_sliding;
  MaterialProperty<Real> & _contact_tangential_slip_rate;
  MaterialProperty<Real> & _contact_friction_coefficient;

  MaterialProperty<RealVectorValue> & _contact_normal;
  MaterialProperty<RealVectorValue> & _contact_tangent;
  MaterialProperty<Real> & _contact_kn;
  MaterialProperty<Real> & _contact_kt;
  MaterialProperty<Real> & _contact_height_ratio;

};

#endif // REDBACKMECHMATERIALCONTACT_H