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

#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include "Material.h"

/**
 * This material automatically declares as material properties whatever is
 *passed to it
 * through the parameters 'prop_names' and uses the Functions from 'prop_values'
 *as the values
 * for those properties.
 *
 * This is not meant to be used in a production capacity... and instead is meant
 *to be used
 * during development phases for ultimate flexibility.
 */
class ImageProcessing : public Material
{
public:
  ImageProcessing(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual void initQpStatefulProperties() override;
  virtual void computeQpProperties() override;

  FunctionName _func;
  std::vector<const Function *> _function;
  FILE * idFile;

private:
  /**
   * A helper method for evaluating the functions
   */
  void computeQpFunctions();
};

#endif // IMAGEPROCESSING_H
