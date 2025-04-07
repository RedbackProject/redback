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

#include "RedbackStressDivergenceTensorsNew.h"

#include "Material.h"
#include "ElasticityTensorTools.h"

registerMooseObject("RedbackApp", RedbackStressDivergenceTensorsNew);

InputParameters
RedbackStressDivergenceTensorsNew::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addRequiredParam<unsigned int>("component",
                                        "An integer corresponding to the direction the variable "
                                        "this kernel acts in. (0 for x, 1 for y, 2 for z)");
  params.addCoupledVar("disp_x", 0.0, "The x displacement");
  params.addCoupledVar("disp_y", 0.0, "The y displacement");
  params.addCoupledVar("disp_z", 0.0, "The z displacement");
  params.addCoupledVar("temp", 0.0, "The temperature");
  params.addCoupledVar("pore_pres", 0.0, "The pore fluid pressure");
  params.addParam<std::string>("base_name", "Material property base name");
  //params.addParam<std::string>(
  //  "appended_property_name", "", "Name appended to material properties to make them unique");

  // Using the displaced mesh will be set in the solid mechanics action input
  // now.
  // params.set<bool>("use_displaced_mesh") = true;

  return params;
}

RedbackStressDivergenceTensorsNew::RedbackStressDivergenceTensorsNew(const InputParameters & parameters) :
    Kernel(parameters),
    _pore_pres(coupledValue("pore_pres")),

    _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : ""),
    _stress(getMaterialPropertyByName<RankTwoTensor>(_base_name + "stress")),

    //_stress(getMaterialProperty<RankTwoTensor>("stress" + getParam<std::string>("appended_property_name"))),
    //_Jacobian_mult(
    //  getMaterialProperty<ElasticityTensorR4>("Jacobian_mult" + getParam<std::string>("appended_property_name"))),
    _Jacobian_mult(getMaterialPropertyByName<RankFourTensor>(_base_name + "Jacobian_mult")),

    // _d_stress_dT(getMaterialProperty<RankTwoTensor>("d_stress_dT"+
    // getParam<std::string>("appended_property_name"))),
    _component(getParam<unsigned int>("component")),
    _biot_coeff(getMaterialProperty<Real>("biot_coefficient")),
    _solid_thermal_expansion(getMaterialProperty<Real>("solid_thermal_expansion")),

    _xdisp_coupled(isCoupled("disp_x")),
    _ydisp_coupled(isCoupled("disp_y")),
    _zdisp_coupled(isCoupled("disp_z")),
    _temp_coupled(isCoupled("temp")),
    _pore_pres_coupled(isCoupled("pore_pres")),

    _xdisp_var(_xdisp_coupled ? coupled("disp_x") : 0),
    _ydisp_var(_ydisp_coupled ? coupled("disp_y") : 0),
    _zdisp_var(_zdisp_coupled ? coupled("disp_z") : 0),
    _temp_var(_temp_coupled ? coupled("temp") : 0),
    _porepressure_var(_pore_pres_coupled ? coupled("pore_pres") : 0),

    _gravity_term(getMaterialProperty<RealVectorValue>("mixture_gravity_term"))
{
}

Real
RedbackStressDivergenceTensorsNew::computeQpResidual()
{
  /* Handling the case where pore pressure is active. In this case pore pressure
   * is not depending on displacements (incompressible case) and the stresses of
   * TensorMechanics are now effective stresses
   * The quantity _poromech_stress is the total stress obeying to Terzaghi's
   * principle: poromech_stress(ij) = stress(ij) + biot_coefficient *
   * pore_pressure * delta(ij).
   * Jacobians are the same, since pore pressure is not depending on
   * displacements
   */

  if (_pore_pres_coupled)
  {
    _poromech_stress_row = _stress[ _qp ].row(_component);
    _poromech_stress_row(_component) -= _biot_coeff[ _qp ] * _pore_pres[ _qp ];
    // return (_poromech_stress_row - _gravity_term[_qp])* _grad_test[_i][_qp];
    return (_poromech_stress_row)*_grad_test[ _i ][ _qp ] - _gravity_term[ _qp ](_component)*_test[ _i ][ _qp ];

    // Note: 30th of October 2015: Negative signs in gravity and pore pressure
    // are being currently tested for the
    // correct sign convention.
    // In this configuration negative gravity_term because we consider positive
    // stresses in extension!
    // Thus, the gravity vector needs to be given with a negative component in Z
    // (e.g. 0 0 -9.81)
  }

  //  return (_stress[_qp].row(_component) - _gravity_term[_qp])*
  //  _grad_test[_i][_qp]; //TODO: Add the gravity kernel
  return (_stress[ _qp ].row(_component)) * _grad_test[ _i ][ _qp ] -
         _gravity_term[ _qp ](_component)*_test[ _i ][ _qp ]; // TODO: Add the gravity kernel
}

Real
RedbackStressDivergenceTensorsNew::computeQpJacobian()
{
  return ElasticityTensorTools::elasticJacobian(_Jacobian_mult[ _qp ],
    _component, _component, _grad_test[ _i ][ _qp ], _grad_phi[ _j ][ _qp ]);
  /*Real result = _Jacobian_mult[_qp].elasticJacobian(_component, _component,
  _grad_test[_i][_qp], _grad_phi[_j][_qp]);
  if (_var.number() != _porepressure_var)
    return result;
  return result + _phi[_j][_qp]*_grad_test[_i][_qp](_component);*/
}

Real
RedbackStressDivergenceTensorsNew::computeQpOffDiagJacobian(unsigned int jvar)
{
  unsigned int coupled_component = 0;
  bool active(false);
  Real porepressure_term = 0;

  if (_xdisp_coupled && jvar == _xdisp_var)
  {
    coupled_component = 0;
    active = true;
  }
  else if (_ydisp_coupled && jvar == _ydisp_var)
  {
    coupled_component = 1;
    active = true;
  }
  else if (_zdisp_coupled && jvar == _zdisp_var)
  {
    coupled_component = 2;
    active = true;
  }

  // Porepressure term (disabled for now, doesn't seem to make any
  // difference...)
   if (_pore_pres_coupled && jvar == _porepressure_var){
    return -_biot_coeff[ _qp ] * _phi[_j][_qp]*_grad_test[_i][_qp](_component);}

  if (active){
    return ElasticityTensorTools::elasticJacobian(_Jacobian_mult[ _qp ],_component,
                                                 coupled_component,
                                                 _grad_test[ _i ][ _qp ],
                                                 _grad_phi[ _j ][ _qp ]);}

  RankTwoTensor Dstress_DT;
  Dstress_DT.zero();
  Dstress_DT.addIa(-_solid_thermal_expansion[_qp]);

  if (_temp_coupled && jvar == _temp_var){
     return (( _Jacobian_mult[ _qp ] *  Dstress_DT ).row(_component)*_grad_test[_i][_qp]) * _phi[_j][_qp];}

  return 0 + porepressure_term;
}
