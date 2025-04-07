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
#include "RedbackMechMaterialHOelastic.h"
#include "PermutationTensor.h"

/**
 * RedbackMechMaterialHOelastic handles a high order material.
 */
InputParameters
RedbackMechMaterialHOelastic::validParams()
{
  InputParameters params = RedbackMechMaterial::validParams();
  params.addCoupledVar("wc_x", 0, "Cosserat rotation around x axis");
  params.addCoupledVar("wc_y", 0, "Cosserat rotation around y axis");
  params.addCoupledVar("wc_z", 0, "Cosserat rotation around z axis");
  params.addRequiredParam<std::vector<Real> >("B_ijkl", "Flexural bending rigidity tensor.  Should have 9 entries.");

  MooseEnum fm = RankFourTensor::fillMethodEnum();
  fm = "general_isotropic";
  params.addParam<MooseEnum>("fill_method_bending", fm, "The fill method for the 'bending' tensor.");

  return params;
}

RedbackMechMaterialHOelastic::RedbackMechMaterialHOelastic(const InputParameters & parameters) :
    RedbackMechMaterial(parameters),
    _symmetric_strain(declareProperty<RankTwoTensor>("symmetric_strain")),
    _antisymmetric_strain(declareProperty<RankTwoTensor>("antisymmetric_strain")),
    _symmetric_plastic_strain(declareProperty<RankTwoTensor>("symmetric_strain")),
    _antisymmetric_plastic_strain(declareProperty<RankTwoTensor>("antisymmetric_strain")),
    _curvature(declareProperty<RankTwoTensor>("curvature")),
    _elastic_curvature(declareProperty<RankTwoTensor>("elastic_curvature")),
    //_elastic_curvature_old(getMaterialPropertyOld<RankTwoTensor>("_elastic_curvature")),
    _total_curvature(declareProperty<RankTwoTensor>("total_curvature")),
    //_total_curvature_old(getMaterialPropertyOld<RankTwoTensor>("total_curvature")),
    _symmetric_stress(declareProperty<RankTwoTensor>("symmetric_stress")),
    _antisymmetric_stress(declareProperty<RankTwoTensor>("antisymmetric_stress")),
    _stress_couple(declareProperty<RankTwoTensor>("coupled_stress")),
    _stress_trace(declareProperty<Real>("stress_trace")),
    _macro_rotation(declareProperty<RankTwoTensor>("macro_rotation")),
    _elastic_flexural_rigidity_tensor(declareProperty<RankFourTensor>("elastic_flexural_rigidity_tensor")),
    _Jacobian_mult_couple(declareProperty<RankFourTensor>("coupled_Jacobian_mult")),
    _Jacobian_offdiag_bc(declareProperty<RankFourTensor>("coupled_Jacobian_off")),
    _Jacobian_offdiag_cb(declareProperty<RankFourTensor>("Jacobian_off")),
    _Bijkl_vector(getParam<std::vector<Real> >("B_ijkl")),
    _Bijkl(),
    _curvature_increment(declareProperty<RankTwoTensor>("curvature_increment")),
    _plastic_curvature(declareProperty<RankTwoTensor>("plastic_curvature")),
    _stress_old(getMaterialPropertyOld<RankTwoTensor>("stress")),
    _stress_older(getMaterialPropertyOlder<RankTwoTensor>("stress")),
    _stress_couple_old(getMaterialPropertyOld<RankTwoTensor>("coupled_stress")),
    _stress_couple_older(getMaterialPropertyOlder<RankTwoTensor>("coupled_stress")),
    _wc_x(coupledValue("wc_x")),
    _wc_y(coupledValue("wc_y")),
    _wc_z(coupledValue("wc_z")),
    _grad_wc_x(coupledGradient("wc_x")),
    _grad_wc_y(coupledGradient("wc_y")),
    _grad_wc_z(coupledGradient("wc_z")),
    _grad_wc_x_old(_fe_problem.isTransient() ? coupledGradientOld("wc_x") : _grad_zero),
    _grad_wc_y_old(_fe_problem.isTransient() ? coupledGradientOld("wc_y") : _grad_zero),
    _grad_wc_z_old(_fe_problem.isTransient() ? coupledGradientOld("wc_z") : _grad_zero),
    _fill_method_bending(getParam<MooseEnum>("fill_method_bending"))
{
  _Bijkl.fillFromInputVector(_Bijkl_vector, (RankFourTensor::FillMethod)(int)_fill_method_bending);
}


void
RedbackMechMaterialHOelastic::initQpStatefulProperties()
{
  RedbackMechMaterial::stepInitQpProperties();
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
  _elastic_flexural_rigidity_tensor[ _qp ].zero();
  _Jacobian_mult_couple[ _qp ].zero();
  _Jacobian_offdiag_bc[ _qp ].zero();
  _Jacobian_offdiag_cb[ _qp ].zero();
  _curvature_increment[ _qp ].zero();
  _plastic_curvature[ _qp ].zero();

}


void
RedbackMechMaterialHOelastic::computeQpStrain(const RankTwoTensor & Fhat)
{
  //strain = (grad_disp + grad_disp^T)/2
  RankTwoTensor grad_tensor(_grad_disp_x[_qp], _grad_disp_y[_qp], _grad_disp_z[_qp]);
  RealVectorValue wc_vector(_wc_x[_qp], _wc_y[_qp], _wc_z[_qp]);

  for (unsigned i = 0; i < LIBMESH_DIM; ++i)
    for (unsigned j = 0; j < LIBMESH_DIM; ++j)
      for (unsigned k = 0; k < LIBMESH_DIM; ++k)
        {
        grad_tensor(i, j) += PermutationTensor::eps(i, j, k) * wc_vector(k);
        //Real tmp = grad_tensor(i, j);
        //std::cout << "grad_tensor(" << i <<", " << j << " = " << tmp << std::endl;
      }
  _symmetric_strain[_qp] = (grad_tensor + grad_tensor.transpose()) / 2.0;
  _antisymmetric_strain[_qp] = (grad_tensor - grad_tensor.transpose()) / 2.0;
  _elastic_strain[_qp] = grad_tensor;
/*
  RankTwoTensor old_deformation(_grad_disp_x_old[_qp],
                    _grad_disp_y_old[_qp],
                     _grad_disp_z_old[_qp]); // Old Deformation gradient

  _strain_increment[_qp]= grad_tensor - old_deformation;
*/
  /* Setting up a macro-rotation (antisymmetric part of the strain) tensor to be used in Cosserat BCs*/
  RankTwoTensor mgrad_tensor(_grad_disp_x[_qp], _grad_disp_y[_qp], _grad_disp_z[_qp]);
  _macro_rotation[_qp] = (mgrad_tensor - mgrad_tensor.transpose()) / 2.0;

  /*Real tmp = _antisymmetric_strain[_qp](0,1) ;
  if (std::abs(tmp) > 1e-50)
    std::cout << "_antisymmetric_strain[" << _qp<<"](0,1) = " << tmp << std::endl;
*/
  //_strain_increment[_qp].addIa(-_solid_thermal_expansion[_qp] * (_T[_qp] - _T_old[_qp])); TODO
  _rotation_increment[_qp].zero();
  _rotation_increment[_qp].addIa(1);

  RankTwoTensor wc_grad_tensor(_grad_wc_x[_qp], _grad_wc_y[_qp], _grad_wc_z[_qp]);
  _curvature[_qp] = wc_grad_tensor;
/*
  RankTwoTensor old_curvature(_grad_wc_x_old[_qp],
                     _grad_wc_y_old[_qp],
                     _grad_wc_z_old[_qp]); // Old Deformation gradient

_curvature_increment[_qp] = wc_grad_tensor - old_curvature;
*/
}

void
RedbackMechMaterialHOelastic::computeQpStress()
{
  //RedbackMechMaterial::computeQpStress();

  _stress[_qp] = _elasticity_tensor[_qp] * _elastic_strain[_qp];

  // Additional outputs for post-processing
  _symmetric_stress[_qp] = (_stress[_qp] + _stress[_qp].transpose()) / 2.0;
  _antisymmetric_stress[_qp] = (_stress[_qp] - _stress[_qp].transpose()) / 2.0;
  _stress_couple[_qp] = _elastic_flexural_rigidity_tensor[_qp] * _curvature[_qp];

  // Compute the energy dissipation and the properties declared
  computeRedbackTerms(_stress[_qp], 0, 0);

  //int num = 4;
  //int num2 = display_(&num);
  //std::cout << "le resultat de l'addition vaut " << num2 << std::endl;
  //myLib::
  //SayHello("world");
 //_stress_trace[_qp] =_stress[_qp].trace();
//traceaff(_stress_trace[0]);
//std::cout << "la trace plus un du tenseur des contraintes vaut " << traceplus(_stress_trace[0]) << std::endl;
/*
int NPROPS = 6;
int NSTR = 18;
int NILL = 1;
int NSVARSGP = 75;

Real STRESSF[NSTR];
Real DEFORT[NSTR];
Real DSDE[NSTR*NSTR];
Real SVARSGP[NSVARSGP];
Real PROPS[NPROPS];

Real Kbulk = 10;
Real Gshear = 10;
Real GCshear = 20/2; // on divise par 2 car facteur 4 au lieu d'un facteur 2 dans la loi d'elasticite de Ioannis
Real Radius = 0.05;
Real mufor = 1; // essai avec le critere le plus simple possible
Real cfor = 10;
Real g_1 = 8/5;
Real g_2 = 2/5;
Real g_3 = 8/5;
Real g_4 = 2/5;

PROPS[0]=Kbulk;
PROPS[1]=Gshear;
PROPS[2]=GCshear;
PROPS[3]=Radius;
PROPS[4]=mufor;
PROPS[5]=cfor;

DEFORT[0]=_strain_increment[_qp](0,0);
DEFORT[1]=_strain_increment[_qp](1,1);
DEFORT[2]=_strain_increment[_qp](2,2);
DEFORT[3]= 0.5*(_strain_increment[_qp](1,2)+_strain_increment[_qp](2,1));
DEFORT[4]= 0.5*(_strain_increment[_qp](0,2)+_strain_increment[_qp](2,0));
DEFORT[5]=0.5*(_strain_increment[_qp](0,1)+_strain_increment[_qp](1,0));
DEFORT[6]=0.5*(_strain_increment[_qp](1,2)-_strain_increment[_qp](2,1));
DEFORT[7]=0.5*(_strain_increment[_qp](0,2)-_strain_increment[_qp](2,0));
DEFORT[8]=0.5*(_strain_increment[_qp](0,2)-_strain_increment[_qp](2,0));
DEFORT[9]=_curvature_increment[_qp](0,0);
DEFORT[10]=_curvature_increment[_qp](0,1);
DEFORT[11]=_curvature_increment[_qp](0,2);
DEFORT[12]=_curvature_increment[_qp](1,0);
DEFORT[13]=_curvature_increment[_qp](1,1);
DEFORT[14]=_curvature_increment[_qp](1,2);
DEFORT[15]=_curvature_increment[_qp](2,0);
DEFORT[16]=_curvature_increment[_qp](2,1);
DEFORT[17]=_curvature_increment[_qp](2,2);

STRESSF[0]=_stress[_qp](0,0);
STRESSF[1]=_stress[_qp](1,1);
STRESSF[2]=_stress[_qp](2,2);
STRESSF[3]=_symmetric_stress[_qp](1,2);
STRESSF[4]=_symmetric_stress[_qp](0,2);
STRESSF[5]=_symmetric_stress[_qp](0,1);
STRESSF[6]=_antisymmetric_stress[_qp](1,2);
STRESSF[7]=_antisymmetric_stress[_qp](0,2);
STRESSF[8]=_antisymmetric_stress[_qp](0,1);
STRESSF[9]=_stress_couple[_qp](0,0);
STRESSF[10]=_stress_couple[_qp](0,1);
STRESSF[11]=_stress_couple[_qp](0,2);
STRESSF[12]=_stress_couple[_qp](1,0);
STRESSF[13]=_stress_couple[_qp](1,1);
STRESSF[14]=_stress_couple[_qp](1,2);
STRESSF[15]=_stress_couple[_qp](2,0);
STRESSF[16]=_stress_couple[_qp](2,1);
STRESSF[17]=_stress_couple[_qp](2,2);

_returnmap_iter[_qp] = 0;

for (unsigned i = 0; i < NSTR; ++i){
SVARSGP[i] = STRESSF[i];
}
for (unsigned i = 0; i < NSTR; ++i){
SVARSGP[i+NSTR] = DEFORT[i];
}

for (unsigned i = 0; i < 3; ++i){
  SVARSGP[2*NSTR+i]=_plastic_strain[_qp](i,i);
}
SVARSGP[2*NSTR+3]=0.5*(_plastic_strain[_qp](1,2)+_plastic_strain[_qp](2,1));
SVARSGP[2*NSTR+4]=0.5*(_plastic_strain[_qp](0,2)+_plastic_strain[_qp](2,0));
SVARSGP[2*NSTR+5]=0.5*(_plastic_strain[_qp](0,1)+_plastic_strain[_qp](1,0));
SVARSGP[2*NSTR+6]=0.5*(_plastic_strain[_qp](1,2)-_plastic_strain[_qp](2,1));
SVARSGP[2*NSTR+7]=0.5*(_plastic_strain[_qp](0,2)-_plastic_strain[_qp](2,0));
SVARSGP[2*NSTR+8]=0.5*(_plastic_strain[_qp](0,2)-_plastic_strain[_qp](2,0));

//usermat_(&STRESSF[NSTR],&DEFORT[NSTR],&DSDE[NSTR][NSTR],&NSTR,&PROPS[NPROPS],&NPROPS,&SVARSGP[NSVARSGP],&NSVARSGP,&NILL,&Kbulk);
usermat_(STRESSF,DEFORT,DSDE,&NSTR,PROPS,&NPROPS,SVARSGP,&NSVARSGP,&NILL);

//usermat_();

std::cout << " fortran sigma (11)= " << STRESSF[0] << std::endl;
std::cout << " Moose sigma (11)= " << _stress[_qp](0,0) << std::endl;

std::cout << " fortran sigma (22)= " << STRESSF[1] << std::endl;
std::cout << " Moose sigma (22)= " << _stress[_qp](1,1) << std::endl;

std::cout << " fortran sigma (33)= " << STRESSF[2] << std::endl;
std::cout << " Moose sigma (33)= " << _stress[_qp](2,2) << std::endl;

std::cout << " fortran sigma (12) sym = " << STRESSF[3] << std::endl;
std::cout << " Moose sigma (12) sym = " << _symmetric_stress[_qp](0,1) << std::endl;

std::cout << " fortran sigma (12) antisym = " << STRESSF[4] << std::endl;
std::cout << " Moose sigma (12) antisym = " << _antisymmetric_stress[_qp](0,1) << std::endl;

std::cout << " fortran moment (31) = " << STRESSF[5] << std::endl;
std::cout << " Moose moment (31) = " << _stress_couple[_qp](2,0) << std::endl;

std::cout << " fortran moment (32)  = " << STRESSF[6] << std::endl;
std::cout << " Moose moment (32)  = " << _stress_couple[_qp](2,1) << std::endl;


_stress[_qp](0,0)=STRESSF[0];
_stress[_qp](1,1)=STRESSF[1];
_stress[_qp](2,2)=STRESSF[2];
_stress[_qp](1,2)=STRESSF[3]+STRESSF[6];
_stress[_qp](0,2)=STRESSF[4]+STRESSF[7];
_stress[_qp](0,1)=STRESSF[5]+STRESSF[8];
_stress[_qp](2,1)=STRESSF[3]-STRESSF[6];
_stress[_qp](2,0)=STRESSF[4]-STRESSF[7];
_stress[_qp](1,0)=STRESSF[5]-STRESSF[8];
_stress_couple[_qp](0,0)=STRESSF[9];
_stress_couple[_qp](0,1)=STRESSF[10];
_stress_couple[_qp](0,2)=STRESSF[11];
_stress_couple[_qp](1,0)=STRESSF[12];
_stress_couple[_qp](1,1)=STRESSF[13];
_stress_couple[_qp](1,2)=STRESSF[14];
_stress_couple[_qp](2,0)=STRESSF[15];
_stress_couple[_qp](2,1)=STRESSF[16];
_stress_couple[_qp](2,2)=STRESSF[17];

_returnmap_iter[_qp] = SVARSGP[3*NSTR];

for (unsigned i = 0; i < 3; ++i){
_plastic_strain[_qp](i,i) = SVARSGP[2*NSTR+i];
}
_plastic_strain[_qp](1,2) = SVARSGP[2*NSTR+3]+SVARSGP[2*NSTR+6];
_plastic_strain[_qp](0,2) = SVARSGP[2*NSTR+4]+SVARSGP[2*NSTR+7];
_plastic_strain[_qp](0,1) = SVARSGP[2*NSTR+5]+SVARSGP[2*NSTR+8];
_plastic_strain[_qp](2,1) = SVARSGP[2*NSTR+3]-SVARSGP[2*NSTR+6];
_plastic_strain[_qp](2,0) = SVARSGP[2*NSTR+4]-SVARSGP[2*NSTR+7];
_plastic_strain[_qp](1,0) = SVARSGP[2*NSTR+5]-SVARSGP[2*NSTR+8];
_plastic_curvature[_qp](0,0)=SVARSGP[2*NSTR+9];
_plastic_curvature[_qp](0,1)=SVARSGP[2*NSTR+10];
_plastic_curvature[_qp](0,2)=SVARSGP[2*NSTR+11];
_plastic_curvature[_qp](1,0)=SVARSGP[2*NSTR+12];
_plastic_curvature[_qp](1,1)=SVARSGP[2*NSTR+13];
_plastic_curvature[_qp](1,2)=SVARSGP[2*NSTR+14];
_plastic_curvature[_qp](2,0)=SVARSGP[2*NSTR+15];
_plastic_curvature[_qp](2,1)=SVARSGP[2*NSTR+16];
_plastic_curvature[_qp](2,2)=SVARSGP[2*NSTR+17];

_symmetric_plastic_strain[_qp] = (_plastic_strain[_qp] + _plastic_strain[_qp].transpose()) / 2.0;
_antisymmetric_plastic_strain[_qp] = (_plastic_strain[_qp] - _plastic_strain[_qp].transpose()) / 2.0;

Real normL2;

for (unsigned int i = 0; i < 3; ++i)
  for (unsigned int j = 0; j < 3; ++j)
    normL2 += g_1 * _antisymmetric_plastic_strain[_qp](i,j) * _antisymmetric_plastic_strain[_qp](i,j) + g_2 * _antisymmetric_plastic_strain[_qp](i,j) * _antisymmetric_plastic_strain[_qp](j,i) + g_3 * _plastic_curvature[_qp](i,j) * _plastic_curvature[_qp](i,j) + g_4 * _plastic_curvature[_qp](i,j) * _plastic_curvature[_qp](j,i);

_eqv_plastic_strain[_qp] = std::pow(normL2, 0.5);

_volumetric_strain[_qp] = _plastic_strain[_qp].trace(); // PLASTIC vol strain


_elastic_strain[_qp](0,0)=SVARSGP[3*NSTR+3+0];
_elastic_strain[_qp](1,1)=SVARSGP[3*NSTR+3+1];
_elastic_strain[_qp](2,2)=SVARSGP[3*NSTR+3+2];
_elastic_strain[_qp](1,2)=SVARSGP[3*NSTR+3+3]+SVARSGP[3*NSTR+3+6];
_elastic_strain[_qp](0,2)=SVARSGP[3*NSTR+3+4]+SVARSGP[3*NSTR+3+7];
_elastic_strain[_qp](0,1)=SVARSGP[3*NSTR+3+5]+SVARSGP[3*NSTR+3+8];
_elastic_strain[_qp](2,1)=SVARSGP[3*NSTR+3+3]-SVARSGP[3*NSTR+3+6];
_elastic_strain[_qp](2,0)=SVARSGP[3*NSTR+3+4]-SVARSGP[3*NSTR+3+7];
_elastic_strain[_qp](1,0)=SVARSGP[3*NSTR+3+5]-SVARSGP[3*NSTR+3+8];
_elastic_curvature[_qp](0,0)=SVARSGP[3*NSTR+3+9];
_elastic_curvature[_qp](0,1)=SVARSGP[3*NSTR+3+10];
_elastic_curvature[_qp](0,2)=SVARSGP[3*NSTR+3+11];
_elastic_curvature[_qp](1,0)=SVARSGP[3*NSTR+3+12];
_elastic_curvature[_qp](1,1)=SVARSGP[3*NSTR+3+13];
_elastic_curvature[_qp](1,2)=SVARSGP[3*NSTR+3+14];
_elastic_curvature[_qp](2,0)=SVARSGP[3*NSTR+3+15];
_elastic_curvature[_qp](2,1)=SVARSGP[3*NSTR+3+16];
_elastic_curvature[_qp](2,2)=SVARSGP[3*NSTR+3+17];

_total_strain[_qp] = _total_strain_old[_qp] + _strain_increment[_qp];
_total_curvature[_qp] = _total_curvature_old[_qp] + _curvature_increment[_qp];

_total_volumetric_strain[_qp] = _total_strain[_qp].trace();

//std::cout << " fortran DEFORT (1)= " << DEFORT[0] << std::endl;
//std::cout << " fortran DEFORT (2)= " << DEFORT[1] << std::endl;
//std::cout << " fortran DEFORT (3)  = " << DEFORT[2] << std::endl;
//std::cout << " fortran DEFORT (4)  = " << DEFORT[3] << std::endl;
//std::cout << " fortran DEFORT (5) = " << DEFORT[4] << std::endl;
//std::cout << " fortran DEFORT (6)  = " << DEFORT[5] << std::endl;

//std::cout << " fortran PROPS (1)= " << PROPS[0] << std::endl;
//std::cout << " fortran PROPS (2)= " << PROPS[1] << std::endl;
//std::cout << " fortran PROPS (3)  = " << PROPS[2] << std::endl;
//std::cout << " fortran PROPS (4)  = " << PROPS[3] << std::endl;

//std::cout << " fortran DSDE (1,1)= " << DSDE[0] << std::endl;
*/

}



void RedbackMechMaterialHOelastic::computeQpElasticityTensor()
{
  RedbackMechMaterial::computeQpElasticityTensor();

  _elastic_flexural_rigidity_tensor[_qp] = _Bijkl;
  _Jacobian_mult_couple[_qp] = _Bijkl;
}

/*
 * STARTING PSEUDO-LOOP IN PLASTICITY FUNCTIONS. NEED TO BE OVERWRITTEN WHEN IN PLASTICITY
 */

void
RedbackMechMaterialHOelastic::returnMap(const RankTwoTensor & sig_old,
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
RedbackMechMaterialHOelastic::getFlowTensor(const RankTwoTensor & sig,
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
RedbackMechMaterialHOelastic::getFlowIncrement(Real sig_eqv, Real pressure, Real q_yield_stress, Real p_yield_stress, Real pc)
{
  return 0; // TODO: not implemented yet
}

void
RedbackMechMaterialHOelastic::getJac(const RankTwoTensor & sig,
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
RedbackMechMaterialHOelastic::get_py_qy(Real /*p*/, Real /*q*/, Real & p_y, Real & q_y, Real /*yield_stress*/, bool & /*is_plastic*/)
{
  // TODO: not implemented yet
  p_y = 0;
  q_y = 0;
}

//#endif // COSSERAT_DYNLIB_EXISTS
