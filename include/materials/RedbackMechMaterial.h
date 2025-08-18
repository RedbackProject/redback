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

#ifndef REDBACKMECHMATERIAL_H
#define REDBACKMECHMATERIAL_H

#include "Material.h"
#include "RankFourTensor.h"
#include "RankTwoTensor.h"
#include "RotationTensor.h"
//#include "FiniteStrainPlasticMaterial.h"

/**
 * This class copy-pastes the stack from tensorMechanics module
 * TODO: add required features to moose and use proper tensorMechanics module
 */
class RedbackMechMaterial : public Material
{
public:
  RedbackMechMaterial(const InputParameters & parameters);

  static InputParameters validParams();

  static MooseEnum damageMethodEnum();
  enum DamageMethod
  {
    BrittleDamage,
    CreepDamage,
    BreakageMechanics,
    DamageHealing,
    FromMultiApp
  };

protected:
  // Copy-paste from TensorMechanicsMaterial.h
  virtual void computeProperties() override;
  virtual void stepInitQpProperties();
  virtual void computeQpElasticityTensor();
  virtual void computeStrain();
  virtual void computeQpStrain();
  virtual void computeQpStrain(const RankTwoTensor & Fhat); // from FiniteStrainMaterial.h
  virtual void computeQpStress();
  virtual void initQpStatefulProperties() override;

  const VariableGradient & _grad_disp_x;
  const VariableGradient & _grad_disp_y;
  const VariableGradient & _grad_disp_z;

  const VariableGradient & _grad_disp_x_old;
  const VariableGradient & _grad_disp_y_old;
  const VariableGradient & _grad_disp_z_old;

  MaterialProperty<RankTwoTensor> & _stress;
  MaterialProperty<RankTwoTensor> & _total_strain;
  MaterialProperty<RankTwoTensor> & _elastic_strain;
  MaterialProperty<RankFourTensor> & _elasticity_tensor;
  MaterialProperty<RankFourTensor> & _Jacobian_mult;

  /// Individual material information
  RankFourTensor _Cijkl;

  // MaterialProperty<RankTwoTensor> & _d_stress_dT;
  // RankTwoTensor _strain_increment;

  /// Current deformation gradient
  // RankTwoTensor _dfgrd;

  // const VariableValue & _T;

  // Copy-paste from FiniteStrainMaterial.h
  MaterialProperty<RankTwoTensor> & _strain_rate;
  MaterialProperty<RankTwoTensor> & _strain_increment;
  const MaterialProperty<RankTwoTensor> & _total_strain_old;
  const MaterialProperty<RankTwoTensor> & _elastic_strain_old;
  const MaterialProperty<RankTwoTensor> & _stress_old;
  MaterialProperty<RankTwoTensor> & _rotation_increment;
  MaterialProperty<RankTwoTensor> & _dfgrd;

  // Copy-paste from FiniteStrainPlasticMaterial.h
  std::vector<Real> _yield_stress_vector;
  MaterialProperty<RankTwoTensor> & _plastic_strain;
  const MaterialProperty<RankTwoTensor> & _plastic_strain_old;
  MaterialProperty<Real> & _eqv_plastic_strain;
  const MaterialProperty<Real> & _eqv_plastic_strain_old;

  // virtual Real yieldFunction(const RankTwoTensor & stress, const Real
  // yield_stress);
  Real getSigEqv(const RankTwoTensor & stress);
  Real deltaFunc(unsigned int i, unsigned int j);
  Real getYieldStress(const Real equivalent_plastic_strain);

  // Copy-paste from FiniteStrainPlasticRateMaterial.h
  virtual void returnMap(const RankTwoTensor &,
                         const RankTwoTensor &,
                         const RankFourTensor &,
                         RankTwoTensor &,
                         RankTwoTensor &,
                         Real &,
                         Real &);
  // The following functions are needed in the return map, but the definition
  // is dependant on the yield criterion. Therefore we define them as abstract
  // virtual functions here such that no implementation is needed in
  // RedbackMechMaterial.C
  virtual void getJac(const RankTwoTensor &,
                      const RankFourTensor &,
                      Real,
                      Real,
                      Real,
                      Real,
                      Real,
                      Real,
                      RankFourTensor &) = 0;
  virtual void
  getFlowTensor(const RankTwoTensor &, Real, Real, Real, Real, Real, RankTwoTensor &) = 0;
  virtual Real getFlowIncrement(Real, Real, Real, Real, Real) = 0;
  virtual void get_py_qy(Real, Real, Real &, Real &, Real, bool &) = 0;

  Real _ref_pe_rate;
  Real _exponent;
  Real _chemo_mechanical_porosity_coeff;

  Real macaulayBracket(Real);

  // Redback specific
  Real _youngs_modulus;
  Real _poisson_ratio;
  MaterialProperty<Real> & _mises_stress;
  MaterialProperty<Real> & _mean_stress;
  MaterialProperty<Real> & _mises_strain_rate;
  MaterialProperty<Real> & _volumetric_strain;
  MaterialProperty<Real> & _volumetric_strain_rate;
  MaterialProperty<Real> & _total_volumetric_strain;
  MaterialProperty<Real> & _mechanical_porosity;
  MaterialProperty<Real> & _mass_removal_rate;
  MaterialProperty<Real> & _poromech_kernel;
  MaterialProperty<Real> & _poromech_jac;
  MaterialProperty<Real> & _mod_gruntfest_number;
  MaterialProperty<Real> & _mechanical_dissipation_mech;
  MaterialProperty<Real> & _mechanical_dissipation_jac_mech;
  MaterialProperty<Real> & _damage_kernel;
  MaterialProperty<Real> & _damage_kernel_jac;
  Real _damage_coeff, _dmg_exponent, _healing_coeff;

  Real _exponential;
  // const VariableValue & _dispx_dot;
  // const VariableValue & _dispy_dot;
  // const VariableValue & _dispz_dot;

  // MaterialProperty<RealVectorValue> & _solid_velocity;

  // Using variables
  bool _has_T;
  const VariableValue & _T;
  const VariableValue & _T_old;
  bool _has_pore_pres;
  const VariableValue & _pore_pres;
  const VariableValue & _total_porosity;
  bool _has_D;
  const VariableValue & _damage;
  const VariableValue & _damage_old;

  DamageMethod _damage_method;

  // Reading material properties from RedbackMaterial
  const MaterialProperty<Real> & _gr;
  const MaterialProperty<Real> & _lewis_number;
  const MaterialProperty<Real> & _ar;
  const MaterialProperty<Real> & _confining_pressure;
  const MaterialProperty<Real> & _alpha_1;
  const MaterialProperty<Real> & _alpha_2;
  const MaterialProperty<Real> & _alpha_3;

  const MaterialProperty<Real> & _delta;
  const MaterialProperty<Real> & _solid_thermal_expansion;
  const MaterialProperty<Real> & _solid_compressibility;
  const MaterialProperty<Real> & _mixture_compressibility;
  const MaterialProperty<Real> & _peclet_number;
  MaterialProperty<Real> & _returnmap_iter;

  Real _T0_param, _P0_param;

  virtual void computeRedbackTerms(RankTwoTensor &, Real, Real);
  virtual void get_py_qy_damaged(Real, Real, Real &, Real &, Real, bool &);
  virtual void form_damage_kernels(Real);

  virtual void formDamageDissipation(RankTwoTensor &);
  virtual void formBrittleDamage();
  virtual void formCreepDamage(Real);

  Real _damage_dissipation;

  /// initial stress components
  std::vector<const Function *> _initial_stress;
};

#endif // REDBACKMECHMATERIAL_H
