//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PointValueFile.h"
#include <fstream>

// MOOSE includes
#include "Function.h"
#include "MooseMesh.h"
#include "MooseVariable.h"
#include "SubProblem.h"

#include "libmesh/system.h"

registerMooseObject("RedbackApp", PointValueFile);

InputParameters
PointValueFile::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  params.addRequiredParam<VariableName>(
      "variable", "The name of the variable that this postprocessor operates on.");
  params.addRequiredParam<Point>("point",
                                 "The physical point where the solution will be evaluated.");
  params.addRequiredParam<FileName>("file", "Name of the txt file to write the point value");

  return params;
}

PointValueFile::PointValueFile(const InputParameters & parameters)
  : GeneralPostprocessor(parameters),
    _var_number(_subproblem.getVariable(_tid, parameters.get<VariableName>("variable")).number()),
    _system(_subproblem.getSystem(getParam<VariableName>("variable"))),
    _point(getParam<Point>("point")),
    _value(0),
    _file_name(getParam<FileName>("file"))
{
}

void
PointValueFile::execute()
{
  _value = _system.point_value(_var_number, _point, false);

  /**
   * If we get exactly zero, we don't know if the locator couldn't find an element, or
   * if the solution is truly zero, more checking is needed.
   */
  if (MooseUtils::absoluteFuzzyEqual(_value, 0.0))
  {
    auto pl = _subproblem.mesh().getPointLocator();
    pl->enable_out_of_mesh_mode();

    auto * elem = (*pl)(_point);
    auto elem_id = elem ? elem->id() : DofObject::invalid_id;
    gatherMin(elem_id);

    if (elem_id == DofObject::invalid_id)
      mooseError(
          "No element located at ", _point, " in PointValueFile Postprocessor named: ", name());
  }
}

Real
PointValueFile::getValue() const
{
  // write text
  FILE * output_file = fopen(_file_name.c_str(), "w");
  fputs(std::to_string(_value).c_str(), output_file);
  fclose(output_file);

  return _value;
}
