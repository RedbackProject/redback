
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

#include "Material.h"
#include "RankTwoTensor.h"
#include "RankFourTensor.h"

class ContactMaterial : public Material
{
public:
  static InputParameters validParams();
  ContactMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;
  virtual void initQpStatefulProperties() override;
  RankTwoTensor computeDeformationGradient() const;
  RankTwoTensor computeCauchyStress(const RankTwoTensor & F) const;
  RankFourTensor computeNumericalTangent(const RankTwoTensor & F) const;

  Point computeNormal() const;
  Point computeTangent(const Point & N0) const;
  Point computeCurrentNormal(const RankTwoTensor & F, const Point & N0) const;
  Point computeCurrentTangent(const RankTwoTensor & F, const Point & S0) const;

  Real computeElementHeight(const Point & N0) const;

protected:
  const VariableGradient & _grad_disp_x;
  const VariableGradient & _grad_disp_y;
  const VariableGradient & _grad_disp_z;

  const unsigned int _dim;

  const Real _youngs_modulus_iso;
  const Real _poisson_ratio_iso;

  const Real _k_n;
  const Real _contact_exponent;

  const MooseEnum _h0_method;
  const MooseEnum _contact_measure;
  const MooseEnum _friction_model;

  const Real _tangent_perturbation;

  MaterialProperty<RankTwoTensor> & _stress;
  MaterialProperty<RankFourTensor> & _Jacobian_mult;
  MaterialProperty<RankTwoTensor> & _strain;

  MaterialProperty<Real> & _biot_coefficient;
  MaterialProperty<RealVectorValue> & _mixture_gravity_term;

  MaterialProperty<Real> & _lambda_n;
  MaterialProperty<Real> & _J4;
  MaterialProperty<Real> & _closure;
  MaterialProperty<Real> & _closure_gap;
  MaterialProperty<Real> & _h_current;
  MaterialProperty<Real> & _h0;
  MaterialProperty<Real> & _h0_current;
};