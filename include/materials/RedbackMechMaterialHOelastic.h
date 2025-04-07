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

#ifndef RedbackMechMaterialHOelastic_H
#define RedbackMechMaterialHOelastic_H

//#ifdef COSSERAT_DYNLIB_EXISTS

#include "RedbackMechMaterial.h"

class RedbackMechMaterialHOelastic : public RedbackMechMaterial
{
public:
  RedbackMechMaterialHOelastic(const InputParameters & parameters);
  static InputParameters validParams();

protected:
  //virtual void stepInitQpProperties();
  virtual void initQpStatefulProperties() override;
  virtual void computeQpStrain(const RankTwoTensor & Fhat) override;
  virtual void computeQpStress() override;
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

  MaterialProperty<RankTwoTensor> & _symmetric_strain;
  MaterialProperty<RankTwoTensor> & _antisymmetric_strain;
  MaterialProperty<RankTwoTensor> & _symmetric_plastic_strain;
  MaterialProperty<RankTwoTensor> & _antisymmetric_plastic_strain;

  MaterialProperty<RankTwoTensor> & _curvature;
  MaterialProperty<RankTwoTensor> & _elastic_curvature;
  //MaterialProperty<RankTwoTensor> & _elastic_curvature_old;
  MaterialProperty<RankTwoTensor> & _total_curvature;
  //MaterialProperty<RankTwoTensor> & _total_curvature_old;

  MaterialProperty<RankTwoTensor> & _symmetric_stress;
  MaterialProperty<RankTwoTensor> & _antisymmetric_stress;
  MaterialProperty<RankTwoTensor> & _stress_couple;

  MaterialProperty<Real> & _stress_trace;

  MaterialProperty<RankTwoTensor> & _macro_rotation;

  MaterialProperty<RankFourTensor> & _elastic_flexural_rigidity_tensor;
  MaterialProperty<RankFourTensor> & _Jacobian_mult_couple;
  MaterialProperty<RankFourTensor> & _Jacobian_offdiag_bc;
  MaterialProperty<RankFourTensor> & _Jacobian_offdiag_cb;

  std::vector<Real> _Bijkl_vector;
  RankFourTensor _Bijkl;

  MaterialProperty<RankTwoTensor> & _curvature_increment;
  MaterialProperty<RankTwoTensor> & _plastic_curvature;
  const MaterialProperty<RankTwoTensor> & _stress_old;
  const MaterialProperty<RankTwoTensor> & _stress_older;
  const MaterialProperty<RankTwoTensor> & _stress_couple_old;
  const MaterialProperty<RankTwoTensor> & _stress_couple_older;

  //MaterialProperty<RankTwoTensor> & _plastic_curvature_old;


private:
  const VariableValue & _wc_x;
  const VariableValue & _wc_y;
  const VariableValue & _wc_z;

  const VariableGradient & _grad_wc_x;
  const VariableGradient & _grad_wc_y;
  const VariableGradient & _grad_wc_z;

  const VariableGradient & _grad_wc_x_old;
  const VariableGradient & _grad_wc_y_old;
  const VariableGradient & _grad_wc_z_old;


  /// determines the translation from B_ijkl to the Rank-4 tensor
  MooseEnum _fill_method_bending;
};


//#endif // COSSERAT_DYNLIB_EXISTS

#endif // RedbackMechMaterialHOelastic_H
