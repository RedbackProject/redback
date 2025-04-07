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
/****************************************************************/
/* Dirichlet boundary condition to impose displacement in a     */
/* direction transversal to element normal vector, by taking a  */
/* vectorial product of the normal with a given vector          */
/****************************************************************/

#include "Function.h"
#include "FunctionDirichletTransverseBC.h"

registerMooseObject("RedbackApp", FunctionDirichletTransverseBC);

InputParameters
FunctionDirichletTransverseBC::validParams()
{
  InputParameters params = DirichletBCBase::validParams();
  params.addRequiredParam<FunctionName>("function", "The forcing function.");
  params.addRequiredParam<RealVectorValue>(
      "center", "Center point to calculate transversal direction for boundary point.");
  params.addRequiredParam<RealVectorValue>(
      "axis",
      "Axis of rotation, vector to calculate transversal direction for "
      "boundary point.");
  params.addParam<Real>("angular_velocity", 1., "Angular velocity");
  params.addRequiredParam<unsigned int>("dir_index", "Direction index (0 for X, 1 for Y, 2 for Z)");
  return params;
}

FunctionDirichletTransverseBC::FunctionDirichletTransverseBC(const InputParameters & parameters)
  : DirichletBCBase(parameters),
    _u_old(valueOld()),
    _func(getFunction("function")),
    _center(getParam<RealVectorValue>("center")),
    _axis(getParam<RealVectorValue>("axis")),
    _angular_velocity(getParam<Real>("angular_velocity")),
    _dir_index(getParam<unsigned int>("dir_index"))
{
}

Real
FunctionDirichletTransverseBC::computeQpValue()
{
  /*TypeVector<Real> vector1 = TypeVector<Real>(_axis);
  TypeVector<Real> vector2 = *static_cast<const TypeVector<Real> *>(_current_node) -
  TypeVector<Real>(_center); TypeVector<Real> vector3 = vector1.cross(vector2); vector3 /=
  vector3.norm(); return vector3(_dir_index) * _func.value(_t, *_current_node);*/

  Real x = (*_current_node)(0);
  Real y = (*_current_node)(1);
  Real z = (*_current_node)(2);
  Real a = _axis(0);
  Real b = _axis(1);
  Real c = _axis(2);
  Real d = std::sqrt(b * b + c * c);
  Real Ct = std::cos(_angular_velocity * _func.value(_t, *_current_node) * _t);
  Real St = std::sin(_angular_velocity * _func.value(_t, *_current_node) * _t);
  /* Construct Transformation Matrix */
  Real M11 = a * a + d * d * Ct;
  Real M12 = a * (b - b * Ct) - c * St;
  Real M13 = a * (c - c * Ct) + b * St;
  Real M14 =
      (1 - a * a - d * d * Ct) * _center(0) +
      St * (c * _center(1) - b * _center(2) + a * (Ct - 1) * (b * _center(1) + c * _center(2)));
  Real M21 = a * (b - b * Ct) + c * St;
  Real M22 = b * b * (1 + a * a * b * b / (d * d) * Ct) + c * c * Ct / (d * d);
  Real M23 = b * c * ((a * a - 1) * Ct / (d * d) + 1) - a * (b * b + c * c) * St / (d * d);
  Real M24 = -(a * b * (1 - Ct) * d * d * _center(0) + c * d * d * St * _center(0) +
               c * c * Ct * _center(1) + (b * b - 1) * d * d * _center(1) +
               b * c * (d * d - Ct) * _center(2) - a * (b * b + c * c) * St * _center(2) +
               a * a * b * Ct * (b * _center(1) + c * _center(2))) /
             (d * d);
  Real M31 = a * (c - c * Ct) - b * St;
  Real M32 = b * c * ((a * a - 1) * Ct / (d * d) + 1) + a * (b * b + c * c) * St / (d * d);
  Real M33 = (b * b * Ct + c * c * (a * a * Ct + d * d)) / (d * d);
  Real M34 = -(a * c * (1 - Ct) * d * d * _center(0) - b * d * d * St * _center(0) +
               b * c * (-Ct + d * d) * _center(1) + a * (b * b + c * c) * St * _center(1) +
               b * b * Ct * _center(2) + (c * c - 1) * d * d * _center(2) +
               a * a * c * Ct * (b * _center(1) + c * _center(2))) /
             (d * d);
  if (_dir_index == 0)
  {
    return M11 * x + M12 * y + M13 * z + M14 - x;
  }
  else if (_dir_index == 1)
  {
    return M21 * x + M22 * y + M23 * z + M24 - y;
  }
  else if (_dir_index == 2)
  {
    return M31 * x + M32 * y + M33 * z + M34 - z;
  }
  else
    mooseError("Case not implemented yet...");
  return 0;
}
