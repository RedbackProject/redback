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

#ifndef ELEMENTFILESUBDOMAIN_H
#define ELEMENTFILESUBDOMAIN_H

// MOOSE includes
#include "MeshGenerator.h"

/**
 * MeshModifier for assigning subdomain IDs of all elements
 */
class ElementFileSubdomain : public MeshGenerator
{
public:
  /**
   * Class constructor
   * @param parameters The input parameters
   */
  ElementFileSubdomain(const InputParameters & parameters);

  static InputParameters validParams();

  /**
   * Class destructor
   */
  virtual ~ElementFileSubdomain();

  /**
   * Perform the actual element subdomain ID assignment
   */
  std::unique_ptr<MeshBase> generate() override;

protected:
  std::unique_ptr<MeshBase> & _input;

};

#endif // ELEMENTFILESUBDOMAIN_H
