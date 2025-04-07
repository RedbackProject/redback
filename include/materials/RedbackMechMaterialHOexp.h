/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/


#ifndef RedbackMechMaterialHOexp_H
#define RedbackMechMaterialHOexp_H


//#ifdef COSSERAT_DYNLIB_EXISTS

#include "RedbackMechMaterial.h"

// Forward Declarations

class RedbackMechMaterialHOexp : public RedbackMechMaterial
{
public:
  RedbackMechMaterialHOexp(const InputParameters & parameters);
  static InputParameters validParams();

protected:
  virtual void initQpStatefulProperties() override;
  virtual void computeQpStrain(const RankTwoTensor & Fhat) override;
  virtual void computeQpStress() override;
  int corsigma(int , int );
  int cormoment(int , int );
  void remplSigmaOld(const RankTwoTensor & , Real* vect, int );
  void remplMomentOld(const RankTwoTensor &, Real* vect, int );
  void recupSigmaNew(RankTwoTensor &, Real* vect, int );
  void recupMomentNew(RankTwoTensor &, Real* vect, int );


  virtual void computeQpElasticityTensor() override;

  virtual void returnMap(const RankTwoTensor &,
                 const RankTwoTensor &,
                 const RankFourTensor &,
                 RankTwoTensor &,
                 RankTwoTensor &,
                 Real &,
                 Real &) override;
  virtual void getJac(const RankTwoTensor &, const RankFourTensor &, Real, Real, Real, Real, Real, Real, RankFourTensor &) override;
  virtual void
  getFlowTensor(const RankTwoTensor &, Real, Real, Real, Real, Real, RankTwoTensor &) override;
  virtual Real getFlowIncrement(Real, Real, Real, Real, Real) override;
  virtual void get_py_qy(Real, Real, Real &, Real &, Real, bool &) override;
  virtual void computeRedbackTerms(RankTwoTensor &, Real, Real) override;


  MaterialProperty<RankTwoTensor> & _symmetric_strain;
  MaterialProperty<RankTwoTensor> & _antisymmetric_strain;
  MaterialProperty<RankTwoTensor> & _symmetric_plastic_strain;
  MaterialProperty<RankTwoTensor> & _antisymmetric_plastic_strain;

  MaterialProperty<RankTwoTensor> & _curvature;
  MaterialProperty<RankTwoTensor> & _elastic_curvature;
  const MaterialProperty<RankTwoTensor> & _elastic_curvature_old;
  MaterialProperty<RankTwoTensor> & _total_curvature;
  const MaterialProperty<RankTwoTensor> & _total_curvature_old;

  MaterialProperty<RankTwoTensor> & _symmetric_stress;
  const MaterialProperty<RankTwoTensor> & _symmetric_stress_old;
  MaterialProperty<RankTwoTensor> & _antisymmetric_stress;
  const MaterialProperty<RankTwoTensor> & _antisymmetric_stress_old;
  MaterialProperty<RankTwoTensor> & _stress_couple;
  const MaterialProperty<RankTwoTensor> & _stress_couple_old;


  MaterialProperty<Real> & _stress_trace;

  MaterialProperty<RankTwoTensor> & _macro_rotation;

  MaterialProperty<RankFourTensor> & _elastic_flexural_rigidity_tensor;
  MaterialProperty<RankFourTensor> & _Jacobian_mult_couple;
  MaterialProperty<RankFourTensor> & _Jacobian_offdiag_bc;
  MaterialProperty<RankFourTensor> & _Jacobian_offdiag_cb;


  const std::vector<Real> _Bijkl_vector, _Cijkl_vector;
  RankFourTensor _Bijkl;

  MaterialProperty<RankTwoTensor> & _curvature_increment;
  MaterialProperty<RankTwoTensor> & _plastic_curvature;
  const MaterialProperty<RankTwoTensor> & _plastic_curvature_old;
  MaterialProperty<RankTwoTensor> & _deviatoric_plastic_strain;

  MaterialProperty<RankTwoTensor> & _deviatoric_stress;
  MaterialProperty<Real> & _volumetric_stress;
  MaterialProperty<Real> & _stress_invariant;
  MaterialProperty<Real> & _hardening_variable;
  const MaterialProperty<Real> & _hardening_variable_old;
  MaterialProperty<Real> & _active_surfaces;
  MaterialProperty<Real> & _lagrange_multiplier;
  MaterialProperty<Real> & _failure_surface;
  const MaterialProperty<RankTwoTensor> & _stress_older;
  const MaterialProperty<RankTwoTensor> & _stress_couple_older;
  MaterialProperty<Real> & _mechanical_dissipation_tot;
  const MaterialProperty<Real> & _mechanical_dissipation_tot_old;
  std::string _plasticity_type;

  /// Minimum fraction of applied strain that may be applied during adaptive stepsizing
  Real _min_stepsize;

  Real _plast_factor;
  /// Number of time division due to the non convergence of the routine for the return-map
  MaterialProperty<Real> & _iter;
  /// Even if the returnMap fails, return the best values found for stress and internal parameters
  bool _ignore_failures;

  MaterialProperty<Real> & _poromech_kernel;
  MaterialProperty<Real> & _poromech_jac;
  MaterialProperty<RankTwoTensor> & _dplastic_heat_dstrain;
  MaterialProperty<RankTwoTensor> & _dplastic_heat_dcurvature;


private:
  const VariableValue & _wc_x;
  const VariableValue & _wc_y;
  const VariableValue & _wc_z;

  const VariableValue & _wc_x_old;
  const VariableValue & _wc_y_old;
  const VariableValue & _wc_z_old;


  const VariableGradient & _grad_wc_x;
  const VariableGradient & _grad_wc_y;
  const VariableGradient & _grad_wc_z;

  const VariableGradient & _grad_wc_x_old;
  const VariableGradient & _grad_wc_y_old;
  const VariableGradient & _grad_wc_z_old;


  /// determines the translation from B_ijkl to the Rank-4 tensor
  MooseEnum _fill_method_bending;

  Real _cohesion, _friction_coefficient, _dilatancy_coefficient, _hardening_mech_modulus, _beta_star, _tolerance_tau;
  Real _shear_modulus, _bulk_modulus, _cosserat_shear_modulus, _cosserat_radius;
  Real _plastic_potential_multiplier;
};



//#endif // COSSERAT_DYNLIB_EXISTS

#endif // RedbackMechMaterialHOexp_H
