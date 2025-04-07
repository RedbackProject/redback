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

#include "Function.h"
#include "ImageProcessing.h"
#include <fstream>

registerMooseObject("RedbackApp", ImageProcessing);

InputParameters
ImageProcessing::validParams()
{
  InputParameters params = Material::validParams();
  params.addRequiredParam<FunctionName>("function", "Name of the function");
  return params;
}

ImageProcessing::ImageProcessing(const InputParameters & parameters)
  : Material(parameters), _func(getParam<FunctionName>("function"))
{
  _function.resize(1);
  _function[0] = &getFunctionByName(_func);
  idFile = fopen("idfile.txt", "w");
  fputs("", idFile);
  fclose(idFile);
}

void
ImageProcessing::initQpStatefulProperties()
{
  computeQpFunctions();
}

void
ImageProcessing::computeQpProperties()
{
  computeQpFunctions();
}

void
ImageProcessing::computeQpFunctions()
{
  if (_t_step == 1 && (*_function[0]).value(_t, _q_point[_qp]) == 0)
  {
    idFile = fopen("idfile.txt", "a");
    std::ostringstream convert; // stream used for the conversion
    convert << _current_elem->id();
    fputs(convert.str().c_str(), idFile);
    fputs(" ", idFile);
    fclose(idFile);
  }
}
