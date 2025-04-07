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

#ifndef REDBACKMECHMATERIALEXPCC_H
#define REDBACKMECHMATERIALEXPCC_H

#include "RedbackMechMaterial.h"

class RedbackMechMaterialExpCC : public RedbackMechMaterial
{
public:
  RedbackMechMaterialExpCC(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  Real _slope_yield_surface; // coefficient for yield surface

  virtual void getJac(const RankTwoTensor &,
                      const RankFourTensor &,
                      Real,
                      Real,
                      Real,
                      Real,
                      Real,
                      Real,
                      RankFourTensor &) override;
  virtual void
  getFlowTensor(const RankTwoTensor &, Real, Real, Real, Real, Real, RankTwoTensor &) override;
  virtual Real getFlowIncrement(Real, Real, Real, Real, Real) override;
  virtual void get_py_qy(Real, Real, Real &, Real &, Real, bool &) override;
  virtual void form_damage_kernels(Real) override;
  void
  getDerivativeFlowIncrement(Real &, Real &, const RankTwoTensor &, Real, Real, Real, Real, Real);
};

#endif // REDBACKMECHMATERIALEXPCC_H
