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

#include "Ellipse.h"
#include "RedbackMechMaterialCCanisotropic.h"
#include <cmath> //used for fabs

registerMooseObject("RedbackApp", RedbackMechMaterialCCanisotropic);

InputParameters
RedbackMechMaterialCCanisotropic::validParams()
{
  InputParameters params = RedbackMechMaterialCC::validParams();
  params.addParam<Real>(
      "initial_anisotropy_param", 0, "Initial anisotropy coefficient (from Dafalias 2013)");

  return params;
}

RedbackMechMaterialCCanisotropic::RedbackMechMaterialCCanisotropic(
    const InputParameters & parameters)
  : RedbackMechMaterialCC(parameters),
    _initial_anisotropy_param(getParam<Real>("initial_anisotropy_param")),
    _anisotropy_coeff(declareProperty<Real>("anisotropy_coeff"))
{
}

void
RedbackMechMaterialCCanisotropic::stepInitQpProperties()
{
  RedbackMechMaterialCC::stepInitQpProperties();
  // Variable initialisation (called at each step)
  if (_t == 0)
    _anisotropy_coeff[_qp] = _initial_anisotropy_param; // TODO: implement \dot{alpha}
  else
    _anisotropy_coeff[_qp] =
        _anisotropy_coeff[_qp] + 0 * (_slope_yield_surface - _anisotropy_coeff[_qp]) *
                                     _dt; // TODO:check the sign of the anisotropy coefficient
}

/**
 * Get unitary flow tensor in deviatoric direction, modified Cam-Clay
 */
void
RedbackMechMaterialCCanisotropic::getFlowTensor(const RankTwoTensor & sig,
                                                Real q,
                                                Real p,
                                                Real /*q_y*/,
                                                Real /*p_y*/,
                                                Real pc,
                                                RankTwoTensor & flow_tensor)
{
  if (pc > 0)
    pc *= -1;

  Real M_squared = _slope_yield_surface * _slope_yield_surface;
  Real alpha_squared = _anisotropy_coeff[_qp] * _anisotropy_coeff[_qp];
  if (q != 0)
    flow_tensor = 3.0 * ((q - _anisotropy_coeff[_qp] * p) / q) * sig.deviatoric() /
                  (M_squared - alpha_squared);
  else
    flow_tensor.zero(); // TODO: is that the right thing to do?
  flow_tensor.addIa(2.0 * p - pc -
                    2.0 * _anisotropy_coeff[_qp] * (q - _anisotropy_coeff[_qp] * p) /
                        (M_squared - alpha_squared) / 3.0); //(p > 0 ? 1:-1)
  // TODO: do we need to normalise? If so, do we need the sqrt(3/2) factor?
  // flow_tensor /= std::pow(2.0/3.0,0.5)*flow_tensor.L2norm();
}

/**
 * Compute the flow incrmement for modified Cam-clay
 * pc ... pre-consolidation pressure (pc = -getYieldStress(eqvpstrain))
 */
Real
RedbackMechMaterialCCanisotropic::getFlowIncrement(
    Real sig_eqv, Real pressure, Real q_yield_stress, Real p_yield_stress, Real pc)
{
  pc *= -1;
  if (Ellipse::isPointOutsideOfRotatedEllipse(/*m=*/_slope_yield_surface,
                                              /*p_0=*/pc,
                                              /*alpha=*/_anisotropy_coeff[_qp],
                                              /*x=*/pressure,
                                              /*y=*/sig_eqv))
  {
    Real flow_incr_vol = _ref_pe_rate * _dt *
                         std::pow(std::fabs(pressure - p_yield_stress), _exponent) * _exponential;
    Real flow_incr_dev = _ref_pe_rate * _dt *
                         std::pow(std::fabs(sig_eqv - q_yield_stress), _exponent) * _exponential;
    return std::pow(flow_incr_vol * flow_incr_vol + flow_incr_dev * flow_incr_dev, 0.5);
  }
  else
    return 0;
}

Real
RedbackMechMaterialCCanisotropic::getDerivativeFlowIncrement(const RankTwoTensor & /*sig*/,
                                                             Real pressure,
                                                             Real sig_eqv,
                                                             Real pc,
                                                             Real q_yield_stress,
                                                             Real p_yield_stress)
{
  if (Ellipse::isPointOutsideOfRotatedEllipse(/*m=*/_slope_yield_surface,
                                              /*p_0=*/pc,
                                              /*alpha=*/_anisotropy_coeff[_qp],
                                              /*x=*/pressure,
                                              /*y=*/sig_eqv))
  {
    Real delta_lambda_p = _ref_pe_rate * _dt *
                          std::pow(std::fabs(pressure - p_yield_stress), _exponent) * _exponential;
    Real delta_lambda_q = _ref_pe_rate * _dt *
                          std::pow(std::fabs(sig_eqv - q_yield_stress), _exponent) * _exponential;
    Real delta_lambda =
        (std::pow(delta_lambda_p * delta_lambda_p + delta_lambda_q * delta_lambda_q, 0.5));
    Real der_flow_incr_dev = _ref_pe_rate * _dt * _exponent *
                             std::pow(std::fabs(sig_eqv - q_yield_stress), _exponent - 1.0) *
                             _exponential;
    Real der_flow_incr_vol = _ref_pe_rate * _dt * _exponent *
                             std::pow(std::fabs(pressure - p_yield_stress), _exponent - 1.0) *
                             _exponential;
    return (delta_lambda_q * der_flow_incr_dev + delta_lambda_p * der_flow_incr_vol) / delta_lambda;
  }
  else
    return 0;
}

void
RedbackMechMaterialCCanisotropic::getJac(const RankTwoTensor & sig,
                                         const RankFourTensor & E_ijkl,
                                         Real flow_incr,
                                         Real sig_eqv,
                                         Real pressure,
                                         Real p_yield_stress,
                                         Real q_yield_stress,
                                         Real pc,
                                         RankFourTensor & dresid_dsig)
{
  unsigned i, j, k, l;
  RankTwoTensor sig_dev, flow_dirn_vol, flow_dirn_dev, fij, flow_dirn, flow_tensor;
  RankTwoTensor dfi_dft;
  RankFourTensor dfd_dsig, dfi_dsig;
  Real f1, f2, f3, f4, f5;
  Real dfi_dseqv;

  pc *= -1; // TODO: check sign at very beginning, only once, and stick to
            // whatever convention!

  sig_dev = sig.deviatoric();

  dfi_dseqv =
      getDerivativeFlowIncrement(sig, pressure, sig_eqv, pc, q_yield_stress, p_yield_stress);
  getFlowTensor(sig, sig_eqv, pressure, q_yield_stress, p_yield_stress, pc, flow_dirn);

  /* The following calculates the tensorial derivative (Jacobian) of the
   * residual with respect to stress, dr_dsig
   * It consists of two terms: The first is
   * dr_dsig = (dfi_dseqv_dev*flow_dirn_dev(k,l)) * flow_dirn_dev(i,j)
   * which is the tensorial product of the flow increment tensor times the flow
   * direction tensor
   *
   * The second is the product of the flow increment tensor times the derivative
   * of the flow direction tensor
   * with respect to the stress tensor. See also REDBACK's documentation
   * */

  // This loop calculates the first term
  for (i = 0; i < 3; ++i)
    for (j = 0; j < 3; ++j)
      for (k = 0; k < 3; ++k)
        for (l = 0; l < 3; ++l)
          dfi_dsig(i, j, k, l) = flow_dirn(i, j) * flow_dirn(k, l) * dfi_dseqv;

  // Real flow_tensor_norm = flow_dirn.L2norm();

  // This loop calculates the second term. Read REDBACK's documentation
  // (same as J2 plasticity case)
  f1 = 0.0;
  f2 = 0.0;
  if (sig_eqv > 1e-8)
  {
    f1 = 3.0 / (_slope_yield_surface * _slope_yield_surface -
                _anisotropy_coeff[_qp] * _anisotropy_coeff[_qp]);
    f2 = f1 * (1.0 - _anisotropy_coeff[_qp] * p_yield_stress / q_yield_stress);
    f3 = -f1 *
         (1.0 - _anisotropy_coeff[_qp] * p_yield_stress / q_yield_stress -
          2 * _slope_yield_surface * _slope_yield_surface / 9.0) /
         3.0;
    f4 = -f1 * (_anisotropy_coeff[_qp] / (3.0 * q_yield_stress));
    f5 = f1 *
         (3.0 * _anisotropy_coeff[_qp] * p_yield_stress / (2.0 * q_yield_stress * q_yield_stress));
  }
  for (i = 0; i < 3; ++i)
    for (j = 0; j < 3; ++j)
      for (k = 0; k < 3; ++k)
        for (l = 0; l < 3; ++l)
          dfd_dsig(i, j, k, l) =
              f2 * deltaFunc(i, k) * deltaFunc(j, l) + f3 * deltaFunc(i, j) * deltaFunc(k, l) +
              f4 * sig_dev(i, j) * deltaFunc(k, l) + f5 * sig_dev(i, j) * sig_dev(k, l) +
              f4 * deltaFunc(i, j) * sig_dev(k, l); // d_flow_dirn/d_sig - 2nd part

  // dfd_dsig = dft_dsig1/flow_tensor_norm - 3.0 * dft_dsig2 /
  // (2*sig_eqv*flow_tensor_norm*flow_tensor_norm*flow_tensor_norm);
  // //d_flow_dirn/d_sig
  // TODO: check if the previous two lines (i.e normalizing the flow vector)
  // should be activated or not. Currently we are using the non-unitary flow
  // vector

  dresid_dsig = E_ijkl.invSymm() + dfd_dsig * flow_incr + dfi_dsig; // Jacobian
}

void
RedbackMechMaterialCCanisotropic::get_py_qy(
    Real p, Real q, Real & p_y, Real & q_y, Real yield_stress, bool & is_plastic)
{
  Ellipse::distanceCCanisotropic(
      _slope_yield_surface, -yield_stress, _anisotropy_coeff[_qp], p, q, p_y, q_y);
  Real pc = -yield_stress;
  is_plastic = Ellipse::isPointOutsideOfRotatedEllipse(
      /*m=*/_slope_yield_surface,
      /*p_0=*/pc,
      /*alpha=*/_anisotropy_coeff[_qp],
      /*x=*/p,
      /*y=*/q);
}
