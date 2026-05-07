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

#pragma once

// MOOSE includes
#include "MeshGenerator.h"

/**
 * MeshModifier for assigning new subdomain IDs of all elements in list of file
 */
class ElementFileSubdomainGenerator : public MeshGenerator
{
public:
  /**
   * Class constructor
   * @param parameters The input parameters
   */
  ElementFileSubdomainGenerator(const InputParameters & parameters);

  static InputParameters validParams();

  /**
   * Perform the actual element subdomain ID assignment
   */
  std::unique_ptr<MeshBase> generate() override;

protected:
  std::unique_ptr<MeshBase> & _input;

};