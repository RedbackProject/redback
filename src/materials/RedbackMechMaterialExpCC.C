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
#include "RedbackMechMaterialExpCC.h"
#include <cmath> //used for fabs

registerMooseObject("RedbackApp", RedbackMechMaterialExpCC);

InputParameters
RedbackMechMaterialExpCC::validParams()
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
  return params;
}

RedbackMechMaterialExpCC::RedbackMechMaterialExpCC(const InputParameters & parameters)
  : RedbackMechMaterial(parameters), _slope_yield_surface(getParam<Real>("slope_yield_surface"))
{
}

/**
 * Get unitary flow tensor in deviatoric direction, modified Cam-Clay
 */
void
RedbackMechMaterialExpCC::getFlowTensor(const RankTwoTensor & sig,
                                        Real /*q*/,
                                        Real p,
                                        Real /*q_y*/,
                                        Real /*p_y*/,
                                        Real pc,
                                        RankTwoTensor & flow_tensor)
{
  if (pc > 0)
    pc *= -1;

  flow_tensor = 3.0 * sig.deviatoric() / (_slope_yield_surface * _slope_yield_surface);
  flow_tensor.addIa((2.0 * p - pc) / 3.0); //(p > 0 ? 1:-1)
  // TODO: do we need to normalise? If so, do we need the sqrt(3/2) factor?
  // flow_tensor /= std::pow(2.0/3.0,0.5)*flow_tensor.L2norm();
}

/**
 * Compute the flow incrmement for modified Cam-clay
 * pc ... pre-consolidation pressure (pc = -getYieldStress(eqvpstrain))
 */
Real
RedbackMechMaterialExpCC::getFlowIncrement(
    Real sig_eqv, Real pressure, Real /*q_yield_stress*/, Real /*p_yield_stress*/, Real pc)
{
  pc *= -1;
  if (Ellipse::isPointOutsideOfEllipse(/*m=*/_slope_yield_surface,
                                       /*p_c=*/pc,
                                       /*x=*/pressure,
                                       /*y=*/sig_eqv))
  {
    Real flow_incr_vol = _ref_pe_rate * _dt * _exponential;
    Real flow_incr_dev = _ref_pe_rate * _dt * _exponential;
    // Real flow_incr_vol = _ref_pe_rate * _dt * std::pow(std::fabs(pressure - p_yield_stress),
    // _exponent) * _exponential; Real flow_incr_dev = _ref_pe_rate * _dt *
    // std::pow(std::fabs(sig_eqv - q_yield_stress), _exponent) * _exponential;
    return std::pow(flow_incr_vol * flow_incr_vol + flow_incr_dev * flow_incr_dev, 0.5);
  }
  else
    return 0;
}

void
RedbackMechMaterialExpCC::getDerivativeFlowIncrement(Real & dfi_dp,
                                                     Real & dfi_dq,
                                                     const RankTwoTensor & /*sig*/,
                                                     Real /*pressure*/,
                                                     Real /*sig_eqv*/,
                                                     Real /*pc*/,
                                                     Real /*q_yield_stress*/,
                                                     Real /*p_yield_stress*/)
{
  dfi_dp = 0; // TODO
  dfi_dq = 0; // TODO
}

void
RedbackMechMaterialExpCC::getJac(const RankTwoTensor & sig,
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
  getFlowTensor(sig, sig_eqv, pressure, q_yield_stress, p_yield_stress, pc, flow_dirn);

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
RedbackMechMaterialExpCC::get_py_qy(
    Real p, Real q, Real & p_y, Real & q_y, Real yield_stress, bool & /*is_plastic*/)
{
  Ellipse::distanceCC(_slope_yield_surface, -yield_stress, p, q, p_y, q_y, 0.);
}

void RedbackMechMaterialExpCC::form_damage_kernels(Real /*q_y*/) {}
