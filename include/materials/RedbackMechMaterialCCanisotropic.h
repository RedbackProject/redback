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

#ifndef REDBACKMECHMATERIALCCANISOTROPIC_H
#define REDBACKMECHMATERIALCCANISOTROPIC_H

#include "RedbackMechMaterialCC.h"

class RedbackMechMaterialCCanisotropic : public RedbackMechMaterialCC
{
public:
  RedbackMechMaterialCCanisotropic(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  Real _initial_anisotropy_param; // Initial coefficient alpha of anisotropy
                                  // (Dafalias 2013)

  MaterialProperty<Real> & _anisotropy_coeff; // current coefficient alpha of
                                              // anisotropy (Dafalias 2013)

  virtual void stepInitQpProperties() override;
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
  Real getDerivativeFlowIncrement(const RankTwoTensor &, Real, Real, Real, Real, Real);
};

#endif // REDBACKMECHMATERIALCCANISOTROPIC_H
