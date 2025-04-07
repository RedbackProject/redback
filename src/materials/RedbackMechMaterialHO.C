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

//#ifdef COSSERAT_DYNLIB_EXISTS

#include "Function.h"
#include "RedbackMechMaterialHO.h"
#include "MooseException.h"
#include "multisurfaceplasticityhard.h"
// #include "NonlinearSystem.h"
#include "FEProblem.h"
#include "PermutationTensor.h"

registerMooseObject("RedbackApp", RedbackMechMaterialHO);

/**
 * RedbackMechMaterialHO handles a high order material.
 */
InputParameters
RedbackMechMaterialHO::validParams()
{
  InputParameters params = RedbackMechMaterial::validParams();
  params.addCoupledVar("wc_x", 0, "Cosserat rotation around x axis");
  params.addCoupledVar("wc_y", 0, "Cosserat rotation around y axis");
  params.addCoupledVar("wc_z", 0, "Cosserat rotation around z axis");
  params.addRequiredParam<std::vector<Real> >("B_ijkl", "Flexural bending rigidity tensor.  Should have 9 entries.");
  //params.addParam<Real>("bulk_modulus", 0.0, "Bulk modulus, K");
  //params.addParam<Real>("shear_modulus", 0.0, "Shear modulus, G");
  //params.addParam<Real>("cosserat_shear_modulus", 0.0, "Cosserat shear modulus, G_c");
  params.addParam<Real>("cohesion", 1, "Material cohesion value");
  params.addParam<Real>("friction_coefficient", 0, "Friction coefficient");
  params.addParam<Real>("dilatancy_coefficient", 0, "Dilatancy coefficient");
  params.addParam<Real>("hardening_mech_modulus", 0, " mechanical hardening parameter value");
  params.addParam<Real>("beta_star", 0, " storage capacity in the fluid mass balance");
  params.addParam<Real>("tolerance_tau", 1E-5, " tolerance added in the plastic strain invariant to have a better convergence in plasticity");
  MooseEnum fm = RankFourTensor::fillMethodEnum();
  fm = "general_isotropic";
  params.addParam<MooseEnum>("fill_method_bending", fm, "The fill method for the 'bending' tensor.");
  params.addParam<std::string>("plasticity_type", "Name that allows to switch for different subroutines for the return map algorithm");
  params.addParam<bool>("ignore_failures", false, "The return-map algorithm will return with the best admissible stresses and internal parameters that it can, even if they don't fully correspond to the applied strain increment.  To speed computations, this flag can be set to true, the max_NR_iterations set small, and the min_stepsize large.");
  params.addRangeCheckedParam<Real>("min_stepsize", 0.01, "min_stepsize>0 & min_stepsize<=1", "If ordinary Newton-Raphson + line-search fails, then the applied strain increment is subdivided, and the return-map is tried again.  This parameter is the minimum fraction of applied strain increment that may be applied before the algorithm gives up entirely");
//  params.addParam<Real>("plast_factor", 1, "factor in the exponential for hardening");
  return params;
}

RedbackMechMaterialHO::RedbackMechMaterialHO(const InputParameters & parameters) :
    RedbackMechMaterial(parameters),
    _symmetric_strain(declareProperty<RankTwoTensor>("symmetric_strain")),
    _antisymmetric_strain(declareProperty<RankTwoTensor>("antisymmetric_strain")),
    _symmetric_plastic_strain(declareProperty<RankTwoTensor>("symmetric_strain")),
    _antisymmetric_plastic_strain(declareProperty<RankTwoTensor>("antisymmetric_strain")),
    _curvature(declareProperty<RankTwoTensor>("curvature")),
    _elastic_curvature(declareProperty<RankTwoTensor>("elastic_curvature")),
    _elastic_curvature_old(getMaterialPropertyOld<RankTwoTensor>("elastic_curvature")),
    _total_curvature(declareProperty<RankTwoTensor>("total_curvature")),
    _total_curvature_old(getMaterialPropertyOld<RankTwoTensor>("total_curvature")),
    _symmetric_stress(declareProperty<RankTwoTensor>("symmetric_stress")),
    _symmetric_stress_old(getMaterialPropertyOld<RankTwoTensor>("symmetric_stress")),
    _antisymmetric_stress(declareProperty<RankTwoTensor>("antisymmetric_stress")),
    _antisymmetric_stress_old(getMaterialPropertyOld<RankTwoTensor>("antisymmetric_stress")),
    _stress_couple(declareProperty<RankTwoTensor>("coupled_stress")),
    _stress_couple_old(getMaterialPropertyOld<RankTwoTensor>("coupled_stress")),
    _stress_trace(declareProperty<Real>("stress_trace")),
    _macro_rotation(declareProperty<RankTwoTensor>("macro_rotation")),
    _elastic_flexural_rigidity_tensor(declareProperty<RankFourTensor>("elastic_flexural_rigidity_tensor")),
    _Jacobian_mult_couple(declareProperty<RankFourTensor>("coupled_Jacobian_mult")),
    _Jacobian_offdiag_bc(declareProperty<RankFourTensor>("coupled_Jacobian_off")),
    _Jacobian_offdiag_cb(declareProperty<RankFourTensor>("Jacobian_off")),
    _Bijkl_vector(getParam<std::vector<Real> >("B_ijkl")),
    _Cijkl_vector(getParam<std::vector<Real> >("C_ijkl")),
    _Bijkl(),
    _curvature_increment(declareProperty<RankTwoTensor>("curvature_increment")),
    _plastic_curvature(declareProperty<RankTwoTensor>("plastic_curvature")),
    _plastic_curvature_old(getMaterialPropertyOld<RankTwoTensor>("plastic_curvature")),
    _deviatoric_plastic_strain(declareProperty<RankTwoTensor>("deviatoric_plastic_strain")),
    _deviatoric_stress(declareProperty<RankTwoTensor>("deviatoric_stress")),
    _volumetric_stress(declareProperty<Real>("volumetric_stress")),
    _stress_invariant(declareProperty<Real>("stress_invariant")),
    _hardening_variable(declareProperty<Real>("hardening_variable")),
    _hardening_variable_old(getMaterialPropertyOld<Real>("hardening_variable")),
    _active_surfaces(declareProperty<Real>("active_surfaces")),
    _lagrange_multiplier(declareProperty<Real>("lagrange_multiplier")),
    _failure_surface(declareProperty<Real>("failure_surface")),
    _stress_older(getMaterialPropertyOlder<RankTwoTensor>("stress")),
    _stress_couple_older(getMaterialPropertyOlder<RankTwoTensor>("coupled_stress")),
    _mechanical_dissipation_tot(declareProperty<Real>("mechanical_dissipation_tot")),
    _mechanical_dissipation_tot_old(getMaterialPropertyOld<Real>("mechanical_dissipation_tot")),
    _plasticity_type(isParamValid("plasticity_type") ? getParam<std::string>("plasticity_type") + "_" : ""),
    _min_stepsize(getParam<Real>("min_stepsize")),
    //_plast_factor(getParam<Real>("plast_factor")),
    _iter(declareProperty<Real>("plastic_local_iterations")), // this is really an unsigned int, but for visualisation i convert it to Real
    _ignore_failures(getParam<bool>("ignore_failures")),
    _poromech_kernel(declareProperty<Real>("poromechanics_kernel")),
    _poromech_jac(declareProperty<Real>("poromechanics_jacobian")),
    _dplastic_heat_dstrain(declareProperty<RankTwoTensor>("dplastic_heat_dstrain")),
    _dplastic_heat_dcurvature(declareProperty<RankTwoTensor>("dplastic_heat_dcurvature")),
    _wc_x(coupledValue("wc_x")),
    _wc_y(coupledValue("wc_y")),
    _wc_z(coupledValue("wc_z")),
    _wc_x_old(coupledValueOld("wc_x")),
    _wc_y_old(coupledValueOld("wc_y")),
    _wc_z_old(coupledValueOld("wc_z")),
    _grad_wc_x(coupledGradient("wc_x")),
    _grad_wc_y(coupledGradient("wc_y")),
    _grad_wc_z(coupledGradient("wc_z")),
    _grad_wc_x_old(_fe_problem.isTransient() ? coupledGradientOld("wc_x") : _grad_zero),
    _grad_wc_y_old(_fe_problem.isTransient() ? coupledGradientOld("wc_y") : _grad_zero),
    _grad_wc_z_old(_fe_problem.isTransient() ? coupledGradientOld("wc_z") : _grad_zero),
    _fill_method_bending(getParam<MooseEnum>("fill_method_bending")),
    _cohesion(getParam<Real>("cohesion")),
    _friction_coefficient(getParam<Real>("friction_coefficient")),
    _dilatancy_coefficient(getParam<Real>("dilatancy_coefficient")),
    _hardening_mech_modulus(getParam<Real>("hardening_mech_modulus")),
    _beta_star(getParam<Real>("beta_star")),
    _tolerance_tau(getParam<Real>("tolerance_tau"))
{
  _Bijkl.fillFromInputVector(_Bijkl_vector, (RankFourTensor::FillMethod)(int)_fill_method_bending);
  _shear_modulus = _Cijkl_vector[ 1 ];
  _bulk_modulus = _Cijkl_vector[ 0 ] + 2*_shear_modulus/3.0;
  _cosserat_shear_modulus = _Cijkl_vector[ 2 ];
  _cosserat_radius = std::sqrt(_Bijkl_vector[ 1 ]/(2 * _shear_modulus));

}

void
RedbackMechMaterialHO::initQpStatefulProperties()
{
  RedbackMechMaterial::initQpStatefulProperties();
  _symmetric_strain[ _qp ].zero();
  _antisymmetric_strain[ _qp ].zero();
  _symmetric_plastic_strain[ _qp ].zero();
  _antisymmetric_plastic_strain[ _qp ].zero();
  _curvature[ _qp ].zero();
  _elastic_curvature[ _qp ].zero();
  _total_curvature[ _qp ].zero();
  _symmetric_stress[ _qp ].zero();
  _antisymmetric_stress[ _qp ].zero();
  _stress_couple[ _qp ].zero();
  _stress_trace[ _qp ] = 0.0;
  _macro_rotation[ _qp ].zero();
  _elastic_flexural_rigidity_tensor[ _qp ] = _Bijkl;
  _Jacobian_mult_couple[ _qp ].zero();
  _Jacobian_offdiag_bc[ _qp ].zero();
  _Jacobian_offdiag_cb[ _qp ].zero();
  _curvature_increment[ _qp ].zero();
  _plastic_curvature[ _qp ].zero();
  _deviatoric_plastic_strain[ _qp ].zero();
  _deviatoric_stress[ _qp ].zero();
  _volumetric_stress[ _qp ] = 0.0;
  _stress_invariant[ _qp ] = 0.0;
  _hardening_variable[ _qp ] = 0.0;
  _active_surfaces[ _qp ] = 0.0;
  _lagrange_multiplier[ _qp ] = 0.0;
  _failure_surface[ _qp ] = 0.0;
  _iter[_qp] = 0.0; // this is really an unsigned int, but for visualisation i convert it to Real
  _poromech_kernel[ _qp ] = 0;
  _poromech_jac[ _qp ] = 0;
}


void
RedbackMechMaterialHO::computeQpStrain(const RankTwoTensor & Fhat)
{
  //strain = (grad_disp + grad_disp^T)/2
  RankTwoTensor grad_tensor(_grad_disp_x[_qp], _grad_disp_y[_qp], _grad_disp_z[_qp]);
  RealVectorValue wc_vector(_wc_x[_qp], _wc_y[_qp], _wc_z[_qp]);

  for (unsigned i = 0; i < LIBMESH_DIM; ++i){
    for (unsigned j = 0; j < LIBMESH_DIM; ++j){
      for (unsigned k = 0; k < LIBMESH_DIM; ++k){
        grad_tensor(i, j) += PermutationTensor::eps(i, j, k) * wc_vector(k);
      }}}

  _symmetric_strain[_qp] = (grad_tensor + grad_tensor.transpose()) / 2.0;
  _antisymmetric_strain[_qp] = (grad_tensor - grad_tensor.transpose()) / 2.0;
  _total_strain[_qp] = grad_tensor;

  RankTwoTensor old_deformation(_grad_disp_x_old[_qp],
                     _grad_disp_y_old[_qp],
                     _grad_disp_z_old[_qp]); // Old Deformation gradient
  RealVectorValue wc_vector_old(_wc_x_old[_qp], _wc_y_old[_qp], _wc_z_old[_qp]);

   for (unsigned i = 0; i < LIBMESH_DIM; ++i){
    for (unsigned j = 0; j < LIBMESH_DIM; ++j){
     for (unsigned k = 0; k < LIBMESH_DIM; ++k){
       old_deformation(i, j) += PermutationTensor::eps(i, j, k) * wc_vector_old(k);
     }}}

  _strain_increment[_qp]= grad_tensor - old_deformation;
  _strain_increment[_qp].addIa(-_solid_thermal_expansion[_qp] * (_T[_qp] - _T_old[_qp]));

  /* Setting up a macro-rotation (antisymmetric part of the strain) tensor to be used in Cosserat BCs*/
  RankTwoTensor mgrad_tensor(_grad_disp_x[_qp], _grad_disp_y[_qp], _grad_disp_z[_qp]);
  _macro_rotation[_qp] = (mgrad_tensor - mgrad_tensor.transpose()) / 2.0;

  _rotation_increment[_qp].zero();
  _rotation_increment[_qp].addIa(1);

  RankTwoTensor wc_grad_tensor(_grad_wc_x[_qp], _grad_wc_y[_qp], _grad_wc_z[_qp]);
  _curvature[_qp] = wc_grad_tensor;

  RankTwoTensor old_curvature(_grad_wc_x_old[_qp],
                     _grad_wc_y_old[_qp],
                     _grad_wc_z_old[_qp]); // Old Deformation gradient

_curvature_increment[_qp] = wc_grad_tensor - old_curvature;
}

void
RedbackMechMaterialHO::computeQpStress()
{
  //RedbackMechMaterial::computeQpStress();

  _plastic_potential_multiplier = 1;
  //if (_has_T)
  //{
    // Q/(RT) = Ar/(1+delta T*)
  //  _plastic_potential_multiplier = std::exp(-_ar[ _qp ]) * std::exp(_ar[ _qp ] * _delta[ _qp ] * _T[ _qp ] / (1 + _delta[ _qp ] * _T[ _qp ]));
  //}

int NPROPS = 19;
int NSTR = 18;
int NILL = 1;
int NSVARSGP = 76;
int NPROPS2 = 9;
int NSTR2 = 18;
int NILL2 = 1;
int NSVARSGP2 = 76;


Real STRESSF[NSTR];
Real DEFORT[NSTR];
Real DSDE[NSTR * NSTR];
Real SVARSGP[NSVARSGP];
Real PROPS[NPROPS];

Real STRESSF2[NSTR2];
Real DEFORT2[NSTR2];
Real DSDE2[NSTR2 * NSTR2];
Real SVARSGP2[NSVARSGP2];
Real PROPS2[NPROPS2];

Real STRESSF3[NSTR2];
Real DEFORT3[NSTR2] ;
Real DSDE3[NSTR2 * NSTR2] ;
Real SVARSGP3[NSVARSGP2] ;
Real PROPS3[NPROPS2] ;

int nb_hardening = 1;
Real g_1;
Real g_2;
Real g_3;
Real g_4;
Real h_1;
Real h_2;
Real h_3;
Real h_4;

if (_plasticity_type.compare("DruckerPrager_cohesion3D_") == 0 || _plasticity_type.compare("DruckerPrager_friction3D_") == 0 ){
   g_1 = 8./5.;
   g_2 = 2./5.;
   g_3 = 8./5.;
   g_4 = 2./5.;
   h_1 = 2./3.;
   h_2 = -1./6.;
   h_3 = 2./3.;
   h_4 = -1./6.;
/*   g_1 = 8./3.;
   g_2 = -2./3.;
   g_3 = 8./3.;
   g_4 = -2./3.;
   h_1 = 2./5.;
   h_2 = 1./10.;
   h_3 = 2./5.;
   h_4 = 1./10.;*/}
else if (_plasticity_type.compare("DruckerPrager_cohesion2D_") == 0 || _plasticity_type.compare("DruckerPrager_friction2D_") == 0 ){
   g_1 = 3./2.;
   g_2 = 1./2.;
   g_3 = 1.;
   g_4 = 0.;
   h_1 = 3./4.;
   h_2 = -1./4.;
   h_3 = 1.;
   h_4 = 0.;
/*   g_1 = 3.;
   g_2 = -1.;
   g_3 = 4.;
   g_4 = 0.;
   h_1 = 3./8.;
   h_2 = 1./8.;
   h_3 = 1./4.;
   h_4 = 0.; */}
else if (_plasticity_type.compare("DeBorst_2D_") == 0 ){
   g_1 = 1./3.;
   g_2 = 1./3.;
   g_3 = 2./3.;
   g_4 = 0.;
   h_1 = 3./4.;
   h_2 = 3./4.;
   h_3 = 3./2.;
   h_4 = 0.;}
else {
  std::cout << " the plasticity type entered doesn't correspond to any of the ones registered " << std::endl;
}

Real plast;

if (_plasticity_type.compare("DruckerPrager_cohesion3D_") == 0 || _plasticity_type.compare("DruckerPrager_cohesion2D_") == 0 || _plasticity_type.compare("DeBorst_2D_") == 0 ){
plast = 0.;}
else if (_plasticity_type.compare("DruckerPrager_friction3D_") == 0 || _plasticity_type.compare("DruckerPrager_friction2D_") == 0 ){
plast = 1.;}
else {
  std::cout << " the plasticity type entered doesn't correspond to any of the ones registered " << std::endl;
}

//Real Hs;
/*
if (( _eqv_plastic_strain_old[_qp] < 0.1) ){
 Hs = _hardening_mech_modulus;}
 else {
 Hs = _hardening_mech_modulus / 2.0 ;}

if (( _eqv_plastic_strain_old[_qp] < 0.2) ){
   }
 else {
 Hs = _hardening_mech_modulus / 4.0 ;}
*/
PROPS[0]=_bulk_modulus;
PROPS[1]=_shear_modulus;
PROPS[2]=_cosserat_shear_modulus;
PROPS[3]=_cosserat_radius;
PROPS[4]=_friction_coefficient;
PROPS[5]=_cohesion;
PROPS[6]=_hardening_mech_modulus;
PROPS[7] = _dilatancy_coefficient;
PROPS[8]=0.0;
PROPS[9]=h_1;
PROPS[10]=h_2;
PROPS[11]=h_3;
PROPS[12]=h_4;
PROPS[13]=g_1;
PROPS[14]=g_2;
PROPS[15]=g_3;
PROPS[16]=g_4;
PROPS[17]= plast;
PROPS[18]=_tolerance_tau;


PROPS2[0]=_bulk_modulus;
PROPS2[1]=_shear_modulus;
PROPS2[2]=_cosserat_shear_modulus;
PROPS2[3]=_cosserat_radius;
PROPS2[4]=_friction_coefficient;
PROPS2[5]=_cohesion;
PROPS2[6]= _hardening_mech_modulus;
PROPS2[7] = _dilatancy_coefficient;
PROPS2[8]=0.0;
PROPS2[9]=h_1;
PROPS2[10]=h_2;
PROPS2[11]=h_3;
PROPS2[12]=h_4;
PROPS2[13]=g_1;
PROPS2[14]=g_2;
PROPS2[15]=g_3;
PROPS2[16]=g_4;
PROPS2[17]= plast;
PROPS2[18]=_tolerance_tau;


remplSigmaOld(_strain_increment[_qp], DEFORT, 0);
remplMomentOld(_curvature_increment[_qp], DEFORT, 0);
remplSigmaOld(_stress_old[_qp], STRESSF, 0);
remplMomentOld(_stress_couple_old[_qp], STRESSF, 0);
remplSigmaOld(_stress_old[_qp], SVARSGP, 0);
remplMomentOld(_stress_couple_old[_qp], SVARSGP, 0);
remplSigmaOld(_total_strain_old[_qp], SVARSGP, NSTR);
remplMomentOld(_total_curvature_old[_qp], SVARSGP, NSTR);
if (nb_hardening != 0) {
  for (unsigned int i = 0; i < nb_hardening; i++) {
    SVARSGP[2*NSTR+i] = _hardening_variable_old[_qp];
  }
}
remplSigmaOld(_plastic_strain_old[_qp], SVARSGP, 2*NSTR + nb_hardening);
remplMomentOld(_plastic_curvature_old[_qp], SVARSGP, 2*NSTR + nb_hardening);
SVARSGP[3*NSTR + nb_hardening] = 1;
SVARSGP[3*NSTR + 1 + nb_hardening] = 0.0;
SVARSGP[3*NSTR + 2 + nb_hardening] = 0.0;
remplSigmaOld(_elastic_strain_old[_qp], SVARSGP, 3*NSTR+3+ nb_hardening);
remplMomentOld(_elastic_curvature_old[_qp], SVARSGP, 3*NSTR+3+ nb_hardening);
for (unsigned int i = 0; i < NSTR*NSTR; ++i){
   DSDE[i] = 0.0;
  }


remplSigmaOld(_strain_increment[_qp], DEFORT2, 0);
remplMomentOld(_curvature_increment[_qp], DEFORT2, 0);
remplSigmaOld(_stress_old[_qp], STRESSF2, 0);
remplMomentOld(_stress_couple_old[_qp], STRESSF2, 0);
remplSigmaOld(_stress_old[_qp], SVARSGP2, 0);
remplMomentOld(_stress_couple_old[_qp], SVARSGP2, 0);
remplSigmaOld(_total_strain_old[_qp], SVARSGP2, NSTR);
remplMomentOld(_total_curvature_old[_qp], SVARSGP2, NSTR);
if (nb_hardening != 0) {
  for (unsigned int i = 0; i < nb_hardening; i++) {
    SVARSGP2[2*NSTR+i] = _hardening_variable_old[_qp];
  }
}
remplSigmaOld(_plastic_strain_old[_qp], SVARSGP2, 2*NSTR2 + nb_hardening);
remplMomentOld(_plastic_curvature_old[_qp], SVARSGP2, 2*NSTR2 + nb_hardening);
SVARSGP2[3*NSTR2 + nb_hardening] = 0;
SVARSGP2[3*NSTR2 + 1 + nb_hardening] = 0.0;
SVARSGP2[3*NSTR2 + 2 + nb_hardening] = 0.0;
remplSigmaOld(_elastic_strain_old[_qp], SVARSGP2, 3*NSTR2+3+ nb_hardening);
remplMomentOld(_elastic_curvature_old[_qp], SVARSGP2, 3*NSTR2+3+ nb_hardening);
for (unsigned int i = 0; i < NSTR*NSTR; ++i){
   DSDE2[i] = 0.0;
  }
bool return_successful = false;
Real step_size = 1.0;
Real time_simulated = 0.0;
unsigned int num_consecutive_successes = 0;
_iter[_qp] = 0;

// NonlinearSystem & system = _fe_problem.getNonlinearSystem();
// int nl_it = system._current_nl_its;

// std::vector<unsigned int> l_it_vector = system._current_l_its;
// int l_it = system._current_l_its.size();

// int element_id = _current_elem->id();
//if ((_t_step == 4) && (_dt > 0.03)&& (nl_it > 1))
//if ((_t_step > 4) && (_t_step < 6))
//std::cout <<"hardening variable is"<< SVARSGP2[2*NSTR] << std::endl;

// Following is necessary because I want strain_increment to be "const"
// but I also want to be able to subdivide an initial_stress
RankTwoTensor this_strain_increment = _strain_increment[_qp];
RankTwoTensor this_curvature_increment = _curvature_increment[_qp];

/*
if (_plasticity_type.compare("DruckerPrager_friction3D_") == 0){
  usermat1_(STRESSF,DEFORT,DSDE,&NSTR,PROPS,&NPROPS,SVARSGP,&NSVARSGP,&NILL);
}
else if (_plasticity_type.compare("DruckerPrager_friction2D_") == 0){
  usermat2_(STRESSF,DEFORT,DSDE,&NSTR,PROPS,&NPROPS,SVARSGP,&NSVARSGP,&NILL);
}
else if (_plasticity_type.compare("DeBorst_2D_") == 0){
  usermat3_(STRESSF,DEFORT,DSDE,&NSTR,PROPS,&NPROPS,SVARSGP,&NSVARSGP,&NILL);
}
else if (_plasticity_type.compare("DruckerPrager_cohesion3D_") == 0){
  usermat4_(STRESSF,DEFORT,DSDE,&NSTR,PROPS,&NPROPS,SVARSGP,&NSVARSGP,&NILL);
}
else if (_plasticity_type.compare("DruckerPrager_cohesion2D_") == 0){
  usermat5_(STRESSF,DEFORT,DSDE,&NSTR,PROPS,&NPROPS,SVARSGP,&NSVARSGP,&NILL);
}
else{
  std::cout << " the plasticity type entered doesn't correspond to any of the ones registered " << std::endl;
}


return_successful = (NILL==0);


if (!return_successful)
{
  Moose::out << "Failed to converge with the return map ******************************************************************************** ";

Real y_coord = _current_elem->centroid()(1);
Real x_coord = _current_elem->centroid()(0);

std::cout << "coordinates x of the point is "<< x_coord  <<std::endl;
std::cout << "coordinates y of the point is "<< y_coord  <<std::endl;
std::cout << "index inside the element is  "<< _qp  <<std::endl;
std::cout << "element Id is  "<< element_id  <<std::endl;
std::cout << "non linear iteration is "<< nl_it  <<std::endl;

std::cout << "time step is "<< _t_step  <<std::endl;
std::cout <<" NSTR " << NSTR2 <<std::endl;
std::cout <<" NPROPS " << NPROPS2 <<std::endl;
std::cout <<" NILL " << NILL2 <<std::endl;
std::cout <<" NSVARSGP " << NSVARSGP2 <<std::endl;

 for (unsigned int i = 0; i < NSTR2 ; ++i)
 {  char sprintf1 [50];
   std::sprintf(sprintf1," STRESS (%u) = %13.6e", i+1, STRESSF2[i]);
   std::cout << sprintf1 <<std::endl;
 }
for (unsigned int i = 0; i < NSTR2 ; ++i)
{char sprintf2 [50];
  std::sprintf(sprintf2," DE (%u) = %13.6e", i+1, DEFORT2[i]);
  std::cout << sprintf2 <<std::endl;
}

for (unsigned int i = 0; i < NSVARSGP2 ; ++i)
{
  char sprintf3 [50];
    std::sprintf(sprintf3," SVARSGP (%u) = %13.6e", i+1, SVARSGP2[i]);
    std::cout << sprintf3 <<std::endl;
}
for (unsigned int i = 0; i < NPROPS2 ; ++i)
{  char sprintf4 [50];
    std::sprintf(sprintf4," PROPS (%u) = %13.6e", i+1, PROPS2[i]);
    std::cout << sprintf4 <<std::endl;
}

//for (unsigned int i = 0; i < NSTR2 ; ++i){
//for (unsigned int j = 0; j < NSTR2 ; ++j){
//  char sprintf5 [50];
//      std::sprintf(sprintf5," DSDE ( %u , %u) = %13.6e", i+1,j+1, DSDE2[i*NSTR2 + j]);
//      std::cout << sprintf5 <<std::endl;
//}}

throw MooseException("MooseException due to the non convergence of the subroutine");
//mooseError("Exiting\n");
}
*/


while (time_simulated < 1.0 && step_size >= _min_stepsize)
{
/*  if (_plasticity_type.compare("DruckerPrager_friction3D_") == 0){
    usermat1_(STRESSF,DEFORT,DSDE,&NSTR,PROPS,&NPROPS,SVARSGP,&NSVARSGP,&NILL);
  }
  else if (_plasticity_type.compare("DruckerPrager_friction2D_") == 0){
    usermat2_(STRESSF,DEFORT,DSDE,&NSTR,PROPS,&NPROPS,SVARSGP,&NSVARSGP,&NILL);
  }
  else if (_plasticity_type.compare("DeBorst_2D_") == 0){
    usermat3_(STRESSF,DEFORT,DSDE,&NSTR,PROPS,&NPROPS,SVARSGP,&NSVARSGP,&NILL);
  }
  else if (_plasticity_type.compare("DruckerPrager_cohesion3D_") == 0){
    usermat4_(STRESSF,DEFORT,DSDE,&NSTR,PROPS,&NPROPS,SVARSGP,&NSVARSGP,&NILL);
  }
  else if (_plasticity_type.compare("DruckerPrager_cohesion2D_") == 0){
    usermat5_(STRESSF,DEFORT,DSDE,&NSTR,PROPS,&NPROPS,SVARSGP,&NSVARSGP,&NILL);
  }
  else{
    std::cout << " the plasticity type entered doesn't correspond to any of the ones registered " << std::endl;
  }
*/
  usermat5_(STRESSF,DEFORT,DSDE,&NSTR,PROPS,&NPROPS,SVARSGP,&NSVARSGP,&NILL);
  //Real verbose = 0;
  //Real y_coord = _current_elem->centroid()(1);
  //Real x_coord = _current_elem->centroid()(0);
  //if (y_coord > 0.45 && y_coord < 0.55 && x_coord > 0.45 && x_coord < 0.55  && _qp==0)
  //  verbose = 1;
  //if (verbose == 1 && NILL != 0)std::cout << " fortran not converging******************************** " << std::endl;

//    for (unsigned int i = 0; i < NSTR ; ++i){
//for (unsigned int j = 0; j < NSTR ; ++j){
//  char sprintf5 [50];
//     std::sprintf(sprintf5," DSDE ( %u , %u) = %13.6e", i+1,j+1, DSDE[i*NSTR + j]);
//     std::cout << sprintf5 <<std::endl;
//}}

  return_successful = (NILL==0);
  _iter[_qp] += 1;

  if (return_successful)
  {
    num_consecutive_successes += 1;
    time_simulated += step_size;

    if (time_simulated < 1.0)  // this condition is just for optimization: if time_simulated=1 then the "good" quantities are no longer needed
    {
      recupSigmaNew(_stress[_qp], SVARSGP, 0);
      recupMomentNew(_stress_couple[_qp], SVARSGP, 0);
      recupSigmaNew(_plastic_strain[_qp], SVARSGP, 2*NSTR + nb_hardening);
      recupMomentNew(_plastic_curvature[_qp], SVARSGP, 2*NSTR + nb_hardening);
      recupSigmaNew(_elastic_strain[_qp], SVARSGP, 3*NSTR+3+ nb_hardening);
      recupMomentNew(_elastic_curvature[_qp], SVARSGP, 3*NSTR+3+ nb_hardening);
      if (nb_hardening != 0) {
        for (unsigned int i = 0; i < nb_hardening; i++) {
          _hardening_variable[_qp] = SVARSGP[2*NSTR+i];
        }
      }
      _active_surfaces[_qp]=SVARSGP[3*NSTR+1+ nb_hardening];
      _failure_surface[_qp]=SVARSGP[3*NSTR+1+ nb_hardening];
      _lagrange_multiplier[_qp]=SVARSGP[3*NSTR+2+ nb_hardening];

      if (num_consecutive_successes >= 2)
        step_size *= 1.2;
    }
    step_size = std::min(step_size, 1.0 - time_simulated); // avoid overshoots
  }
  else
  {
    throw MooseException("Something happened, dt needs to be reduced");
    Moose::out << "the stepsize begins to be reduced " << _iter[_qp] << std::endl;
//    for (unsigned int i = 0; i < NSTR ; ++i){
//for (unsigned int j = 0; j < NSTR ; ++j){
//  char sprintf5 [50];
//     std::sprintf(sprintf5," DSDE ( %u , %u) = %13.6e", i+1,j+1, DSDE[i*NSTR + j]);
//     std::cout << sprintf5 <<std::endl;
//}}
// for (unsigned int i = 0; i < NSTR ; ++i)
// {  char sprintf1 [50];
//   std::sprintf(sprintf1," STRESS (%u) = %13.6e", i+1, STRESSF[i]);
//   std::cout << sprintf1 <<std::endl;
// }
    step_size *= 0.5;
    num_consecutive_successes = 0;
    remplSigmaOld(_stress_old[_qp], STRESSF, 0);
    remplMomentOld(_stress_couple_old[_qp], STRESSF, 0);
    remplSigmaOld(_stress_old[_qp], SVARSGP, 0);
    remplMomentOld(_stress_couple_old[_qp], SVARSGP, 0);
    remplSigmaOld(_total_strain_old[_qp], SVARSGP, NSTR);
    remplMomentOld(_total_curvature_old[_qp], SVARSGP, NSTR);
    if (nb_hardening != 0) {
      for (unsigned int i = 0; i < nb_hardening; i++) {
        SVARSGP[2*NSTR+i] = _hardening_variable_old[_qp];
      }
    }
    remplSigmaOld(_plastic_strain_old[_qp], SVARSGP, 2*NSTR + nb_hardening);
    remplMomentOld(_plastic_curvature_old[_qp], SVARSGP, 2*NSTR + nb_hardening);
    SVARSGP[3*NSTR + nb_hardening] = 0.0;
    SVARSGP[3*NSTR + 1 + nb_hardening] = 0.0;
    SVARSGP[3*NSTR + 2 + nb_hardening] = 0.0;
    remplSigmaOld(_elastic_strain_old[_qp], SVARSGP, 3*NSTR+3+ nb_hardening);
    remplMomentOld(_elastic_curvature_old[_qp], SVARSGP, 3*NSTR+3+ nb_hardening);
    for (unsigned i = 0; i < NSTR*NSTR; ++i){
    DSDE[i] = 0;
    }

    _strain_increment[_qp] = step_size*this_strain_increment;
    _curvature_increment[_qp] = step_size*this_curvature_increment;

    remplSigmaOld(_strain_increment[_qp], DEFORT, 0);
    remplMomentOld(_curvature_increment[_qp], DEFORT, 0);
  }
}

if (!return_successful)
{  Moose::out << "Failed to converge with the return map ******************************************************************************** ";

  if (_ignore_failures)
    {    //Moose::out << "Failed to converge with the return map ******************************************************************************** ";
    }
    else
    {
    Moose::out << "After reducing the stepsize to " << step_size
              << " with original strain increment with L2norm "
              << this_strain_increment.L2norm() << " the returnMap algorithm failed\n";
    mooseError("Exiting\n");
  }
}


recupSigmaNew(_stress[_qp], STRESSF, 0);
recupMomentNew(_stress_couple[_qp], STRESSF, 0);


recupSigmaNew(_plastic_strain[_qp], SVARSGP, 2*NSTR + nb_hardening);
recupMomentNew(_plastic_curvature[_qp], SVARSGP, 2*NSTR + nb_hardening);

_symmetric_plastic_strain[_qp] = (_plastic_strain[_qp] + _plastic_strain[_qp].transpose()) / 2.0;
_antisymmetric_plastic_strain[_qp] = (_plastic_strain[_qp] - _plastic_strain[_qp].transpose()) / 2.0;
_deviatoric_plastic_strain[_qp] = _plastic_strain[_qp].deviatoric();

Real normL2;
normL2 = 0.0;
for (unsigned int i = 0; i < 3; ++i)
  for (unsigned int j = 0; j < 3; ++j)
    normL2 += g_1 * _deviatoric_plastic_strain[_qp](i,j) * _deviatoric_plastic_strain[_qp](i,j)
            + g_2 * _deviatoric_plastic_strain[_qp](i,j) * _deviatoric_plastic_strain[_qp](j,i)
            + ( g_3 * _plastic_curvature[_qp](i,j) * _plastic_curvature[_qp](i,j)
              + g_4 * _plastic_curvature[_qp](i,j) * _plastic_curvature[_qp](j,i) )  * (std::pow(_cosserat_radius, 2.0));

_eqv_plastic_strain[_qp] = std::pow(normL2, 0.5);
_volumetric_strain[_qp] = _plastic_strain[_qp].trace(); // PLASTIC vol strain

recupSigmaNew(_elastic_strain[_qp], SVARSGP, 3*NSTR+3+ nb_hardening);
recupMomentNew(_elastic_curvature[_qp], SVARSGP, 3*NSTR+3+ nb_hardening);


if (nb_hardening != 0) {
  for (unsigned int i = 0; i < nb_hardening; i++) {
    _hardening_variable[_qp] = SVARSGP[2*NSTR+i];
  }
}
_active_surfaces[_qp]=SVARSGP[3*NSTR+ nb_hardening];
_failure_surface[_qp]=SVARSGP[3*NSTR+1+ nb_hardening];
_lagrange_multiplier[_qp]=SVARSGP[3*NSTR+2+ nb_hardening];

_total_strain[_qp] = _total_strain_old[_qp] + _strain_increment[_qp];
_total_curvature[_qp] = _total_curvature_old[_qp] + _curvature_increment[_qp];
_total_volumetric_strain[_qp] = _total_strain[_qp].trace();
_symmetric_stress[_qp] = (_stress[_qp] + _stress[_qp].transpose()) / 2.0;
_antisymmetric_stress[_qp] = (_stress[_qp] - _stress[_qp].transpose()) / 2.0;
_volumetric_stress[_qp] = (_stress[_qp].trace())/3;
_deviatoric_stress[_qp] = _stress[_qp].deviatoric();

Real normL2_stress;
normL2_stress=0.0;

for (unsigned int i = 0; i < 3; ++i){
  for (unsigned int j = 0; j < 3; ++j){
  normL2_stress += h_1 * _deviatoric_stress[_qp](i,j) * _deviatoric_stress[_qp](i,j)
                + h_2 * _deviatoric_stress[_qp](i,j) * _deviatoric_stress[_qp](j,i)
                + ( h_3 * _stress_couple[_qp](i,j) * _stress_couple[_qp](i,j)
                + h_4 * _stress_couple[_qp](i,j) * _stress_couple[_qp](j,i) ) / (std::pow(_cosserat_radius, 2.0));
}}
_stress_invariant[_qp] = std::pow(normL2_stress, 0.5);
//mise a jour des termes de la matrice tangente


for (unsigned int i = 0; i < 3; ++i){
  for (unsigned int j = 0; j < 3; ++j){
    for (unsigned int k = 0; k < 3; ++k){
      for (unsigned int l = 0; l < 3; ++l){
        int test = NSTR*corsigma(k,l) + corsigma(i,j);
        //std::cout << " i="<<i<<", j="<<j<<", k="<<k<<", l="<<l<<", _Jacobian_mult = "<<DSDE[NSTR*corsigma(k,l) + corsigma(i,j)]<< std::endl;
_Jacobian_mult[_qp](i,j,k,l) = DSDE[NSTR*corsigma(k,l) + corsigma(i,j)];
_Jacobian_mult_couple[_qp](i,j,k,l) = DSDE[NSTR*(cormoment(k,l) + NSTR / 2) + cormoment(i,j)+ NSTR / 2];
_Jacobian_offdiag_bc[_qp](i,j,k,l) = DSDE[NSTR*corsigma(k,l) + cormoment(i,j)+ NSTR / 2];
_Jacobian_offdiag_cb[_qp](i,j,k,l) = DSDE[NSTR*(cormoment(k,l) + NSTR / 2) + corsigma(i,j)];
}}}}

// Compute the energy dissipation and the properties declared
computeRedbackTerms(_stress[_qp], 0, 0);


}

int RedbackMechMaterialHO::corsigma(int num1, int num2){
  int result;

  if (num1 == 0 && num2 == 0){
    result = 0;
  }
  else if (num1 == 1 && num2 == 1){
    result = 1;
  }
  else if (num1 == 2 && num2 == 2){
    result = 2;
  }
  else if (num1 == 1 && num2 == 2){
    result = 3;
  }
  else if (num1 == 0 && num2 == 2){
    result = 4;
  }
  else if (num1 == 0 && num2 == 1){
    result = 5;
  }
  else if (num1 == 2 && num2 == 1){
    result = 6;
  }
  else if (num1 == 2 && num2 == 0){
    result = 7;
  }
  else if (num1 == 1 && num2 == 0){
    result = 8;
  }
  return result;
}

int RedbackMechMaterialHO::cormoment(int num1, int num2){
  int result;

  if (num1 == 0 && num2 == 0){
    result = 0;
  }
  else if (num1 == 0 && num2 == 1){
    result = 1;
  }
  else if (num1 == 0 && num2 == 2){
    result = 2;
  }
  else if (num1 == 1 && num2 == 0){
    result = 3;
  }
  else if (num1 == 1 && num2 == 1){
    result = 4;
  }
  else if (num1 == 1 && num2 == 2){
    result = 5;
  }
  else if (num1 == 2 && num2 == 0){
    result = 6;
  }
  else if (num1 == 2 && num2 == 1){
    result = 7;
  }
  else if (num1 == 2 && num2 == 2){
    result = 8;
  }

return result;
}

void RedbackMechMaterialHO::remplSigmaOld(const RankTwoTensor & tens_old,  Real* vect, int  ini){
  for (unsigned i = 0; i < 3; ++i){
    vect[ini + i]=tens_old(i,i);
  }
  vect[ini +3]=tens_old(1,2);
  vect[ini+4]=tens_old(0,2);
  vect[ini+5]=tens_old(0,1);
  vect[ini+6]=tens_old(2,1);
  vect[ini+7]=tens_old(2,0);
  vect[ini+8]=tens_old(1,0);
}


void RedbackMechMaterialHO::remplMomentOld(const RankTwoTensor & tens_old, Real* vect, int  ini){
  vect[ini+9]=tens_old(0,0);
  vect[ini+10]=tens_old(0,1);
  vect[ini+11]=tens_old(0,2);
  vect[ini+12]=tens_old(1,0);
  vect[ini+13]=tens_old(1,1);
  vect[ini+14]=tens_old(1,2);
  vect[ini+15]=tens_old(2,0);
  vect[ini+16]=tens_old(2,1);
  vect[ini+17]=tens_old(2,2);
}


void RedbackMechMaterialHO::recupSigmaNew(RankTwoTensor & tens_new, Real* vect, int  ini){
  for (unsigned i = 0; i < 3; ++i){
  tens_new(i,i) = vect[ini + i];
  }

  tens_new(1,2) = vect[ini + 3];
  tens_new(0,2) = vect[ini + 4];
  tens_new(0,1) = vect[ini + 5];
  tens_new(2,1) = vect[ini + 6];
  tens_new(2,0) = vect[ini + 7];
  tens_new(1,0) = vect[ini + 8];

}


void RedbackMechMaterialHO::recupMomentNew(RankTwoTensor & tens_new, Real* vect, int  ini){
  tens_new(0,0)=vect[ini + 9];
  tens_new(0,1)=vect[ini + 10];
  tens_new(0,2)=vect[ini + 11];
  tens_new(1,0)=vect[ini + 12];
  tens_new(1,1)=vect[ini + 13];
  tens_new(1,2)=vect[ini + 14];
  tens_new(2,0)=vect[ini + 15];
  tens_new(2,1)=vect[ini + 16];
  tens_new(2,2)=vect[ini + 17];
}



void RedbackMechMaterialHO::computeQpElasticityTensor()
{
//  RedbackMechMaterial::computeQpElasticityTensor();

//  _elastic_flexural_rigidity_tensor[_qp] = _Bijkl;
//  _Jacobian_mult_couple[_qp] = _Bijkl;
}

/*
 * STARTING PSEUDO-LOOP IN PLASTICITY FUNCTIONS. NEED TO BE OVERWRITTEN WHEN IN PLASTICITY
 */

void
RedbackMechMaterialHO::returnMap(const RankTwoTensor & sig_old,
                                      const RankTwoTensor & delta_d,
                                      const RankFourTensor & E_ijkl,
                                      RankTwoTensor & dp,
                                      RankTwoTensor & sig,
                                      Real & p_y,
                                      Real & q_y)
{
  //  _stress[_qp] = _elasticity_tensor[_qp] * _elastic_strain[_qp];

   sig = _elasticity_tensor[_qp] * _elastic_strain[_qp];
   //std::cout << std::setprecision(12) << std::fixed;

  //sig = sig_old + E_ijkl * delta_d;
  dp = RankTwoTensor(); // Plastic rate of deformation tensor in unrotated configuration
}

/**
 * Get unitary flow tensor in deviatoric direction
 */
void
RedbackMechMaterialHO::getFlowTensor(const RankTwoTensor & sig,
                                     Real /*q*/,
                                     Real p,
                                     Real /*q_y*/,
                                     Real /*p_y*/,
                                     Real yield_stress,
                                     RankTwoTensor & flow_tensor)
{
  flow_tensor.zero(); // TODO: not implemented yet
}

/**
 * Compute the flow incrmement for modified Cam-clay
 * pc ... pre-consolidation pressure (pc = -getYieldStress(eqvpstrain))
 */
Real
RedbackMechMaterialHO::getFlowIncrement(Real sig_eqv, Real pressure, Real q_yield_stress, Real p_yield_stress, Real pc)
{
  return 0; // TODO: not implemented yet
}

void
RedbackMechMaterialHO::getJac(const RankTwoTensor & sig,
                              const RankFourTensor & E_ijkl,
                              Real flow_incr,
                              Real sig_eqv,
                              Real pressure,
                              Real p_yield_stress,
                              Real q_yield_stress,
                              Real pc,
                              RankFourTensor & dresid_dsig)
{
  dresid_dsig.zero();
}

void
RedbackMechMaterialHO::get_py_qy(Real /*p*/, Real /*q*/, Real & p_y, Real & q_y, Real /*yield_stress*/, bool & /*is_plastic*/)
{
  // TODO: not implemented yet
  p_y = 0;
  q_y = 0;
}

void
RedbackMechMaterialHO::computeRedbackTerms(RankTwoTensor & sig, Real q_y, Real p_y)
{
  // Update the mechanical dissipation
  RankTwoTensor instantaneous_strain_rate;
  RankTwoTensor instantaneous_curvature_rate;

  instantaneous_strain_rate = (_plastic_strain[ _qp ] - _plastic_strain_old[ _qp ]) / _dt;
  instantaneous_curvature_rate = (_plastic_curvature[ _qp ] - _plastic_curvature_old[ _qp ]) / _dt;

  instantaneous_strain_rate *= _plastic_potential_multiplier;
  instantaneous_curvature_rate *= _plastic_potential_multiplier;

  _mechanical_dissipation_mech[ _qp ] = _stress[_qp].doubleContraction(instantaneous_strain_rate) +
                                      _stress_couple[_qp].doubleContraction(instantaneous_curvature_rate) ;
  _mechanical_dissipation_mech[ _qp ] *= _gr[ _qp ];

  _mechanical_dissipation_jac_mech[ _qp ] = 0;
  /*
  if (_has_T)
  {
   _mechanical_dissipation_jac_mech[ _qp ] = _mechanical_dissipation_mech[ _qp ] / (1 + _delta[ _qp ] * _T[ _qp ]) / (1 + _delta[ _qp ] * _T[ _qp ]);
  }
  */
  _mechanical_dissipation_tot[ _qp ] =  _mechanical_dissipation_tot_old[ _qp ] + _mechanical_dissipation_mech[ _qp ]* _dt;

  if (_plastic_strain[_qp] == _plastic_strain_old[_qp])
    // no plastic deformation, so _elasticity_tensor = _Jacobian_mult
      _dplastic_heat_dstrain[_qp] = RankTwoTensor();
  else
  {
    _dplastic_heat_dstrain[_qp] =
      (_plastic_strain[_qp] - _plastic_strain_old[_qp]).initialContraction(_Jacobian_mult[_qp]);
      _dplastic_heat_dstrain[_qp] += _stress[_qp];
      _dplastic_heat_dstrain[_qp] -= _stress[_qp].initialContraction(_Cijkl.invSymm() * _Jacobian_mult[_qp]);
      _dplastic_heat_dstrain[_qp] /= _dt;
      _dplastic_heat_dstrain[_qp] *= _gr[ _qp ];
   }
   //_dplastic_heat_dstrain[_qp] = RankTwoTensor();

     if (_plastic_curvature[_qp] == _plastic_curvature_old[_qp])
    // no plastic deformation, so _elasticity_tensor = _Jacobian_mult
      _dplastic_heat_dcurvature[_qp] = RankTwoTensor();
  else
  {
    _dplastic_heat_dcurvature[_qp] =
      (_plastic_curvature[_qp] - _plastic_curvature_old[_qp]).initialContraction(_Jacobian_mult_couple[_qp]);
      _dplastic_heat_dcurvature[_qp] += _stress_couple[_qp];
      _dplastic_heat_dcurvature[_qp] -= _stress_couple[_qp].initialContraction(_Bijkl.invSymm() * _Jacobian_mult_couple[_qp]);
      _dplastic_heat_dcurvature[_qp] /= _dt;
      _dplastic_heat_dcurvature[_qp] *= _gr[ _qp ];
   }
   //_dplastic_heat_dcurvature[_qp] = RankTwoTensor();

  // Update the mechanical porosity
  Real delta_phi_mech_el, delta_phi_mech_pl;
  delta_phi_mech_el =
    (1.0 - _total_porosity[ _qp ]) * (_solid_compressibility[ _qp ] * (_pore_pres[ _qp ] - _P0_param) -
                                      _solid_thermal_expansion[ _qp ] * (_T[ _qp ] - _T0_param) +
                                      (_elastic_strain[ _qp ] - _elastic_strain_old[ _qp ]).trace());
  delta_phi_mech_pl = (1.0 - _total_porosity[ _qp ]) * (_plastic_strain[ _qp ] - _plastic_strain_old[ _qp ]).trace();

  _mechanical_porosity[ _qp ] = delta_phi_mech_el + delta_phi_mech_pl;

  //Real _beta_star_init = 0.0;

  //Compute terms for the RedbackPoroHO
  //if ( _stress[_qp](0,0) > -65 ){
  //  _beta_star_init = _beta_star * 10000000 ;}
  //else {
  //  _beta_star_init = _beta_star ;}

  Real instantaneous_vol_strain_rate;
  instantaneous_vol_strain_rate = (_total_strain[ _qp ].trace() - _total_strain_old[ _qp ].trace()) / _dt;
  //instantaneous_vol_strain_rate = (_plastic_strain[ _qp ].trace() - _plastic_strain_old[ _qp ].trace()) / _dt;
 _poromech_kernel[ _qp ] = instantaneous_vol_strain_rate / _beta_star;
 //_poromech_jac[ _qp ] = (1 / (1 + _delta[ _qp ] * _T[ _qp ]) / (1 + _delta[ _qp ] * _T[ _qp ]));
 _poromech_jac[ _qp ] = 1 / ( _beta_star * _dt);
}


//#endif // COSSERAT_DYNLIB_EXISTS
