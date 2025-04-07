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

#ifndef REDBACKMECHMATERIALELASTIC_H
#define REDBACKMECHMATERIALELASTIC_H

#include "RedbackMechMaterial.h"

class RedbackMechMaterialElastic : public RedbackMechMaterial
{
public:
  RedbackMechMaterialElastic(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual void returnMap(const RankTwoTensor &,
                         const RankTwoTensor &,
                         const RankFourTensor &,
                         RankTwoTensor &,
                         RankTwoTensor &,
                         Real &,
                         Real &) override;
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
};

#endif // REDBACKMECHMATERIALELASTIC_H
