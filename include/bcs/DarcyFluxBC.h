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

#ifndef DARCYFLUXBC_H
#define DARCYFLUXBC_H

#include "FluxBC.h"

/**
 * A FluxBC which is consistent with the boundary terms arising from
 * the Diffusion Kernel. The flux vector in this case is simply
 * grad(u) and the residual contribution is:
 *
 * \f$ F(u) = - \int_{\Gamma} \nabla u * \hat n * \phi d\Gamma \f$
 *
 * In contrast to e.g. VectorNeumannBC, the user does not provide a
 * specified value of the flux when using this class, instead the
 * residual contribution corresponding to the current value of grad(u)
 * is computed and accumulated into the residual vector.
 */
class DarcyFluxBC : public FluxBC
{
public:
  DarcyFluxBC(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual RealGradient computeQpFluxResidual() override;
  virtual RealGradient computeQpFluxJacobian() override;

  const MaterialProperty<Real> & _Le;
};

#endif /* DARCYFLUXBC_H */
