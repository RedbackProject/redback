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
#include "RedbackMechMaterialCC.h"
#include <cmath> //used for fabs

registerMooseObject("RedbackApp", RedbackMechMaterialCC);

InputParameters
RedbackMechMaterialCC::validParams()
{
  InputParameters params = RedbackMechMaterial::validParams();
  // TODO: Check sign of slope_yield_surface
  //  if (_slope_yield_surface == 0)
  //    mooseError("modified Cam-Clay cannot deal with 0 CSL slope
  //    ('slope_yield_surface')");
  //  if (getYieldStress(0) <= 0)
  //    mooseError("modified Cam-Clay cannot deal with negative
  //    pre-consolidation stress ('yield_stress')");
  params.addParam<Real>(
      "slope_yield_surface", 0, "Slope of yield surface (positive, see documentation)");
  params.addParam<Real>("shift_ellipse",
                        0,
                        "Horizontal shift of the ellipse in normalised stress values (positive to "
                        "give the material some cohesion)");
  return params;
}

RedbackMechMaterialCC::RedbackMechMaterialCC(const InputParameters & parameters)
  : RedbackMechMaterial(parameters),
    _slope_yield_surface(std::fabs(getParam<Real>("slope_yield_surface"))),
    _shift_ellipse(getParam<Real>("shift_ellipse"))
{
}

/**
 * Get flow tensor in deviatoric direction, modified Cam-Clay
 */
void
RedbackMechMaterialCC::getFlowTensor(const RankTwoTensor & sig,
                                     Real /*q*/,
                                     Real p,
                                     Real /*q_y*/,
                                     Real /*p_y*/,
                                     Real yield_stress,
                                     RankTwoTensor & flow_tensor)
{
  Real pc = -yield_stress;

  flow_tensor = 3.0 * sig.deviatoric() / (_slope_yield_surface * _slope_yield_surface);
  flow_tensor.addIa((2.0 * p - pc - 2 * _shift_ellipse) / 3.0);
}

/**
 * Compute the flow incrmement for modified Cam-clay
 * pc ... pre-consolidation pressure (pc = -getYieldStress(eqvpstrain))
 */
Real
RedbackMechMaterialCC::getFlowIncrement(
    Real sig_eqv, Real pressure, Real q_yield_stress, Real p_yield_stress, Real yield_stress)
{
  Real pc = -yield_stress;
  Real sigma_0 = std::fabs(pc);
  Real flow_incr_vol = _ref_pe_rate * _dt *
                       std::pow((pressure - p_yield_stress) / sigma_0, _exponent) * _exponential;
  Real flow_incr_dev =
      _ref_pe_rate * _dt * std::pow((sig_eqv - q_yield_stress) / sigma_0, _exponent) * _exponential;
  return std::sqrt(flow_incr_vol * flow_incr_vol + flow_incr_dev * flow_incr_dev);
}

void
RedbackMechMaterialCC::getDerivativeFlowIncrement(Real & dfi_dp,
                                                  Real & dfi_dq,
                                                  const RankTwoTensor & /*sig*/,
                                                  Real pressure,
                                                  Real sig_eqv,
                                                  Real pc,
                                                  Real q_yield_stress,
                                                  Real p_yield_stress)
{
  Real sigma_0 = std::fabs(pc);
  Real p_term = (pressure - p_yield_stress) / sigma_0;
  Real q_term = (sig_eqv - q_yield_stress) / sigma_0;
  Real factor = _ref_pe_rate * _dt * _exponent * _exponential / sigma_0 /
                std::sqrt(std::pow(p_term, 2 * _exponent) + std::pow(q_term, 2 * _exponent));
  dfi_dp = factor * std::pow(p_term, 2 * _exponent - 1);
  dfi_dq = factor * std::pow(q_term, 2 * _exponent - 1);
}

void
RedbackMechMaterialCC::getJac(const RankTwoTensor & sig,
                              const RankFourTensor & E_ijkl,
                              Real flow_incr,
                              Real sig_eqv,
                              Real pressure,
                              Real p_yield_stress,
                              Real q_yield_stress,
                              Real yield_stress,
                              RankFourTensor & dresid_dsig)
{
  unsigned i, j, k, l;
  RankTwoTensor sig_dev, flow_dirn_vol, flow_dirn_dev, fij, flow_dirn, flow_tensor;
  RankTwoTensor dfi_dft;
  RankFourTensor dfd_dsig, dfi_dsig;
  Real f1, f2, f3;
  Real dfi_dp, dfi_dseqv;

  Real pc = -yield_stress;
  sig_dev = sig.deviatoric();

  getDerivativeFlowIncrement(
      dfi_dp, dfi_dseqv, sig, pressure, sig_eqv, pc, q_yield_stress, p_yield_stress);
  getFlowTensor(sig, sig_eqv, pressure, q_yield_stress, p_yield_stress, yield_stress, flow_dirn);

  /* The following calculates the tensorial derivative (Jacobian) of the
   *residual with respect to stress, dr_dsig
   * It consists of two terms: The first is
   * dr_dsig = (dfi_dseqv_dev*flow_dirn_dev(k,l)) * flow_dirn_dev(i,j)
   * which is the tensorial product of the flow increment tensor times the flow
   *direction tensor
   *
   * The second is the product of the flow increment tensor times the derivative
   *of the flow direction tensor
   * with respect to the stress tensor. See also REDBACK's documentation
   * */

  f1 = 0.0;
  f2 = 0.0;
  f3 = 0.0;
  if (sig_eqv > 1e-10)
  {
    f1 = 3.0 / (_slope_yield_surface * _slope_yield_surface);
    f2 = 2.0 / 9.0 - 1.0 / (_slope_yield_surface * _slope_yield_surface);
    f3 = 3.0 / (2.0 * sig_eqv);
  }
  // This loop calculates the first term
  for (i = 0; i < 3; ++i)
    for (j = 0; j < 3; ++j)
      for (k = 0; k < 3; ++k)
        for (l = 0; l < 3; ++l)
          dfi_dsig(i, j, k, l) =
              flow_dirn(i, j) * (f3 * sig_dev(k, l) * dfi_dseqv + dfi_dp * deltaFunc(k, l) / 3.0);

  // This loop calculates the second term.
  for (i = 0; i < 3; ++i)
    for (j = 0; j < 3; ++j)
      for (k = 0; k < 3; ++k)
        for (l = 0; l < 3; ++l)
          dfd_dsig(i, j, k, l) =
              f1 * deltaFunc(i, k) * deltaFunc(j, l) + f2 * deltaFunc(i, j) * deltaFunc(k, l);

  dresid_dsig = E_ijkl.invSymm() + dfd_dsig * flow_incr + dfi_dsig; // Jacobian
}

void
RedbackMechMaterialCC::get_py_qy(
    Real p, Real q, Real & p_y, Real & q_y, Real yield_stress, bool & is_plastic)
{
  Real M = _slope_yield_surface;
  Real pc = -yield_stress;
  // Check if outside the ellipse
  Real potential =
      std::pow(q / M, 2) + p * (p - pc) + _shift_ellipse * (_shift_ellipse - 2 * p + pc);
  is_plastic = (potential >= 0); // compute yield coords regardless

  // get yield point in any case (even if elastic)
  Ellipse::distanceCC(_slope_yield_surface, -yield_stress, p, q, p_y, q_y, _shift_ellipse);
}

void RedbackMechMaterialCC::form_damage_kernels(Real /*q_y*/) {}
