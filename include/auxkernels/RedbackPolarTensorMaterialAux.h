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

#ifndef REDBACKPOLARTENSORMATERIALAUX_H
#define REDBACKPOLARTENSORMATERIALAUX_H

#include "AuxKernel.h"
#include "RankTwoTensor.h"

class RedbackPolarTensorMaterialAux : public AuxKernel
{
public:
  RedbackPolarTensorMaterialAux(const InputParameters & parameters);
  static InputParameters validParams();

  virtual ~RedbackPolarTensorMaterialAux() {}

protected:
  virtual Real computeValue();

private:
  const MaterialProperty<RankTwoTensor> & _tensor;
  const unsigned int _i;
  const unsigned int _j;
};

#endif // REDBACKPOLARTENSORMATERIALAUX_H
