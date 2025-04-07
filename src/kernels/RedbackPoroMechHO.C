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

#include "RedbackPoroMechHO.h"
#include "MooseMesh.h"

InputParameters
RedbackPoroMechHO::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addCoupledVar("temp", 0.0, "Temperature variable.");
  params.addCoupledVar("displacements", "The string of displacements suitable for the problem statement");
  params.addParam<Real>("time_factor", 1.0, "Time rescaling factor (global parameter!)");
  return params;
}

RedbackPoroMechHO::RedbackPoroMechHO(const InputParameters & parameters) :
    Kernel(parameters),
    _poromech_kernel(getMaterialProperty<Real>("poromechanics_kernel")),
    _poromech_jac(getMaterialProperty<Real>("poromechanics_jacobian")),
    _ndisp(coupledComponents("displacements")),
    _disp_var(_ndisp),
    _solid_thermal_expansion(getMaterialProperty<Real>("solid_thermal_expansion")),   
    _temp_coupled(isCoupled("temp")), 
    _temp_var(coupled("temp")),
    _time_factor(getParam<Real>("time_factor"))
{
  for (unsigned int i = 0; i < _ndisp; ++i)
    _disp_var[i] = coupled("displacements", i);

  // Checking for consistency between mesh size and length of the provided displacements vector
  if (_ndisp != _mesh.dimension())
    //mooseError("RedbackPoroMechHO: The number of displacement variables supplied must match the "
               //"mesh dimension.");
    mooseWarning("RedbackPoroMechHO: The number of displacement variables supplied must match the "
               "mesh dimension.");
}

RedbackPoroMechHO::~RedbackPoroMechHO()
{
}

Real
RedbackPoroMechHO::computeQpResidual()
{
  return  _time_factor *_poromech_kernel[ _qp ] * _test[ _i ][ _qp ];
  // TODO: add note in doco to tell users not to turn on term if compress=0
  // TODO: add check compress > 0
}

Real
RedbackPoroMechHO::computeQpJacobian()
{
  return 0;
}

Real
RedbackPoroMechHO::computeQpOffDiagJacobian(unsigned int jvar)
{
  for (unsigned int i = 0; i < _ndisp; ++i)
    if (jvar == _disp_var[i])
      return _poromech_jac[ _qp ] * _grad_phi[_j][_qp](i) *_test[_i][_qp];
      
  if (_temp_coupled && jvar == _temp_var){
     return -3 * _solid_thermal_expansion[_qp] *  _poromech_jac[ _qp ]* _test[_i][_qp]* _phi[_j][_qp];}


  return 0;
}
