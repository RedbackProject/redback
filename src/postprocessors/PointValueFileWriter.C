//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PointValueFileWriter.h"
#include <fstream>

// MOOSE includes
#include "Function.h"
#include "MooseMesh.h"
#include "MooseVariable.h"
#include "SubProblem.h"

#include "libmesh/system.h"

registerMooseObject("RedbackApp", PointValueFileWriter);

InputParameters
PointValueFileWriter::validParams()
{
  // File that does homogeneous erosion of a binary CT-scan file and write resulting microstructure in another filename
  // The amount of erosion is given by a PointValue Postprocessor
  InputParameters params = GeneralPostprocessor::validParams();
  params.addRequiredParam<VariableName>(
      "variable", "The name of the variable that this postprocessor operates on.");
  params.addRequiredParam<Point>("point",
                                 "The physical point where the solution will be evaluated.");
  params.addRequiredParam<FileName>("file", "Name of the CTscan data file");
  params.addRequiredParam<FileName>("working_file", "Name of the data file to be created");
  params.addRequiredParam<FileName>("multiapp_file",
                                    "Name of the file to know if multiapp should be run");
  params.addRequiredParam<std::string>("pore_value", "Value of the pore (string)");
  params.addRequiredParam<std::string>("grain_value", "Value of the grain (string)");
  params.addParam<unsigned int>(
      "refinement_level", 1, "Number of elements wanted per pixel of the image");
  params.addParam<int>("size_x", 1, "Size in x");
  params.addParam<int>("size_y", 1, "Size in y");
  params.addParam<int>("size_z", 1, "Size in z. z=1 if 2D");
  params.addRequiredParam<int>("nx", "Resolution in x");
  params.addRequiredParam<int>("ny", "Resolution in y");
  params.addRequiredParam<int>("nz", "Resolution in z. z=1 if 2D");
  MooseEnum neighbours = MooseEnum("direct diag2D diag3D", "diag2D");
  params.addParam<MooseEnum>("neighbours", neighbours, "type of neighbours detection");

  return params;
}

PointValueFileWriter::PointValueFileWriter(const InputParameters & parameters)
  : GeneralPostprocessor(parameters),
    _value(0),
    _var_number(_subproblem.getVariable(_tid, parameters.get<VariableName>("variable")).number()),
    _system(_subproblem.getSystem(getParam<VariableName>("variable"))),
    _point(getParam<Point>("point")),
    _old_poro_value(0),
    _file_name(getParam<FileName>("file")),
    _working_file_name(getParam<FileName>("working_file")),
    _multiapp_file_name(getParam<FileName>("multiapp_file")),
    _value_pore(getParam<std::string>("pore_value")),
    _value_grain(getParam<std::string>("grain_value")),
    _refinement(getParam<unsigned int>("refinement_level")),
    _size_x(getParam<int>("size_x")),
    _size_y(getParam<int>("size_y")),
    _size_z(getParam<int>("size_z")),
    _nx(getParam<int>("nx")),
    _ny(getParam<int>("ny")),
    _nz(getParam<int>("nz")),
    _neighbours(getParam<MooseEnum>("neighbours"))
{
  // read the CTscan txt file and extract the information
  std::string line;
  std::ifstream myfile(_file_name);
  _data.clear();
  if (myfile.is_open())
  {
    while (myfile.good())
    {
      getline(myfile, line);
      std::stringstream ss(line);
      std::istream_iterator<std::string> begin(ss);
      std::istream_iterator<std::string> end;
      std::vector<std::string> vstrings(begin, end);
      _data.push_back(vstrings);
    }
    myfile.close();
  }
  else
    mooseError("Unable to open file");

  // keeping only necessary data
  _data.erase(_data.end() - 1);
  _data.erase(_data.begin(), _data.begin() + (_nz > 1 ? 7 : 5));

  // assert size match the user's
  mooseAssert(_nx == _data[0].size(),
              "x resolution provided does not match the x resolution of the file.");
  mooseAssert(_ny * _nz == _data.size(),
              "y*z resolution provided does not match the y*z resolution of the file.");

  std::vector<std::vector<std::string>> data_refined;
  if (_refinement > 1)
  {
    // don't refine in z if 2D
    int refinement_z = (_nz > 1 ? _refinement : 1);
    // uniform refinement
    data_refined.resize(_ny * _refinement * _nz * refinement_z);
    for (int z = 0; z < _nz; ++z)
    {
      for (int y = 0; y < _ny; ++y)
      {
        for (int x = 0; x < _nx; ++x)
          for (int i = 0; i < _refinement; ++i)
            data_refined[z * refinement_z * _ny * _refinement + _refinement * y].push_back(
                _data[z * _ny + y][x]);
        for (int j = 1; j < _refinement; ++j)
          data_refined[z * refinement_z * _ny * _refinement + _refinement * y + j] =
              data_refined[z * refinement_z * _ny * _refinement + _refinement * y];
      }
      for (int k = 1; k < refinement_z; ++k)
        for (int y = 0; y < _refinement * _ny; ++y)
          data_refined[(z * refinement_z + k) * _ny * _refinement + y] =
              data_refined[z * refinement_z * _ny * _refinement + y];
    }
    _data = data_refined;
    // reevaluating _size_ after refinement
    _nx = _data[0].size();
    _ny *= _refinement;
    if (_nz > 1)
      _nz *= _refinement;
    _console << "_nx = " << _nx << std::endl;
    _console << "_ny = " << _ny << std::endl;
    _console << "_nz = " << _nz << std::endl;
  }

  // calculating the pore_volume
  for (int z = 0; z < _nz; ++z)
    for (int y = 0; y < _ny; ++y)
      for (int x = 0; x < _nx; ++x)
        if (_data[z * _ny + y][x] == _value_pore)
          _pore_volume += 1 / (float)(_nx * _ny * _nz);
  _console << "_pore_volume = " << _pore_volume << std::endl;

  // init value
  _poro_value = _pore_volume;

  // init _upper_layer_bound
  _boundary_element_value = _value_grain;
  BoundaryElements();
  Real layer_volume = 1 / (float)(_nx * _ny * _nz) * _boundary_elements.size();
  _upper_layer_bound = _pore_volume + layer_volume;
  _console << "_upper_layer_bound = " << _upper_layer_bound << std::endl;

  // init _lower_layer_bound
  _lower_layer_bound = _pore_volume;
  _console << "_lower_layer_bound = " << _lower_layer_bound << std::endl;

  std::stringstream ss;
  // position along x/y/z axis for writing data file
  for (int i = 0; i < _nx; ++i)
    ss << i * _size_x / (float)_nx << " ";
  _data_x_axis_text = ss.str();

  ss.str(std::string());
  for (int i = 0; i < _ny; ++i)
    ss << i * _size_y / (float)_ny << " ";
  _data_y_axis_text = ss.str();

  ss.str(std::string());
  for (int i = 0; i < _nz; ++i)
    ss << i * _size_z / (float)_nz << " ";
  _data_z_axis_text = ss.str();

  // write file
  FileWriter();

  _console << "done with constructor" << std::endl;
}

void
PointValueFileWriter::execute()
{
  _old_poro_value = _poro_value;
  _poro_value = _system.point_value(_var_number, _point, false);

  _console << "_old_poro_value = " << _old_poro_value << std::endl;
  _console << "_poro_value = " << _poro_value << std::endl;

  /**
   * If we get exactly zero, we don't know if the locator couldn't find an element, or
   * if the solution is truly zero, more checking is needed.
   */
  if (MooseUtils::absoluteFuzzyEqual(_poro_value, 0.0))
  {
    auto pl = _subproblem.mesh().getPointLocator();
    pl->enable_out_of_mesh_mode();

    auto * elem = (*pl)(_point);
    auto elem_id = elem ? elem->id() : DofObject::invalid_id;
    gatherMin(elem_id);

    if (elem_id == DofObject::invalid_id)
      mooseError("No element located at ",
                 _point,
                 " in PointValueFileWriter Postprocessor named: ",
                 name());
  }
}

void
PointValueFileWriter::finalize()
{
  // define precipitation
  bool precip = false;
  if (_poro_value - _old_poro_value < 0)
    precip = true;

  _console << "precip = " << precip << std::endl;
  _console << "_poro_value = " << _poro_value << std::endl;
  _console << "_lower_layer_bound = " << _lower_layer_bound << std::endl;
  _console << "_upper_layer_bound = " << _upper_layer_bound << std::endl;

  // write to run multiapp or not
  FILE * output_file = fopen(_multiapp_file_name.c_str(), "w");
  // don't run erosion if change of porosity is too low
  if (_poro_value > _lower_layer_bound && _poro_value < _upper_layer_bound)
  {
    fputs(std::to_string(0).c_str(), output_file);
    fclose(output_file);
    _value = _pore_volume;
  }
  fputs(std::to_string(1).c_str(), output_file);
  fclose(output_file);

  _console << "running code for erosion..." << std::endl;

  // read the CTscan txt file and extract the information
  std::string line;
  std::ifstream myfile(_working_file_name);
  _data.clear();
  if (myfile.is_open())
  {
    while (myfile.good())
    {
      getline(myfile, line);
      std::stringstream ss(line);
      std::istream_iterator<std::string> begin(ss);
      std::istream_iterator<std::string> end;
      std::vector<std::string> vstrings(begin, end);
      _data.push_back(vstrings);
    }
    myfile.close();
  }
  else
    mooseError("Unable to open file");

  // keeping only necessary data
  _data.erase(_data.end() - 1);
  _data.erase(_data.begin(), _data.begin() + (_nz > 1 ? 7 : 5));

  // how to check which elements are boundary, precip or disso
  _boundary_element_value = _value_grain;
  std::string new_value = _value_pore;
  if (precip == true)
  {
    _boundary_element_value = _value_pore;
    new_value = _value_grain;
  }

  Real layer_volume;
  bool first_loop = true;
  while (_pore_volume < 1. - 1e-10 && _pore_volume > 0. + 1e-10)
  {
    if (precip || !first_loop)
      BoundaryElements();
    first_loop = false;

    if (_boundary_elements.size() == 0)
    {
      _upper_layer_bound = _pore_volume;
      _console << "_upper_layer_bound = " << _upper_layer_bound << std::endl;
      _lower_layer_bound = _pore_volume;
      _console << "_lower_layer_bound = " << _lower_layer_bound << std::endl;
      break;
    }

    // Calculating the volume of the boundary elements layer
    layer_volume = 1 / (float)(_nx * _ny * _nz) * _boundary_elements.size();
    _console << "layer_volume = " << layer_volume << std::endl;

    // break of loop if layer is too big to remove
    if (precip == false && _pore_volume + layer_volume > _poro_value)
    {
      _upper_layer_bound = _pore_volume + layer_volume;
      _console << "_upper_layer_bound = " << _upper_layer_bound << std::endl;
      _lower_layer_bound = _pore_volume;
      _console << "_lower_layer_bound = " << _lower_layer_bound << std::endl;
      break;
    }

    // assign new value to the boundary elements
    for (int i = 0; i < _boundary_elements.size(); ++i)
      _data[_boundary_elements[i].first][_boundary_elements[i].second] = new_value;

    // updating _pore_volume
    if (precip == true)
    {
      _upper_layer_bound = _pore_volume;
      _pore_volume -= layer_volume;
      if (_poro_value > _pore_volume)
      {
        _console << "_upper_layer_bound = " << _upper_layer_bound << std::endl;
        _lower_layer_bound = _pore_volume;
        _console << "_lower_layer_bound = " << _lower_layer_bound << std::endl;
        break;
      }
    }
    else
      _pore_volume += layer_volume;
    _console << "_pore_volume = " << _pore_volume << std::endl;
  }

  // write file
  FileWriter();

  _console << "... erosion done" << std::endl;

  _value = _pore_volume;
}

Real
PointValueFileWriter::getValue() const
{
  return _value;
}

void
PointValueFileWriter::FileWriter()
{
  // convert data to text
  std::vector<std::string> data_text;
  for (size_t z = 0; z < _nz; ++z)
  {
    for (size_t y = 0; y < _ny; ++y)
    {
      std::stringstream ss;
      for (size_t x = 0; x < _nx; ++x)
      {
        ss << _data[z * _ny + y][x] << " ";
      }
      data_text.push_back(ss.str());
    }
  }

  // write text
  FILE * output_file = fopen(_working_file_name.c_str(), "w");
  fputs("AXIS X\n", output_file);
  fputs(_data_x_axis_text.c_str(), output_file);
  fputs("\nAXIS Y\n", output_file);
  fputs(_data_y_axis_text.c_str(), output_file);
  if (_nz > 1)
  {
    fputs("\nAXIS Z\n", output_file);
    fputs(_data_z_axis_text.c_str(), output_file);
  }
  fputs("\nDATA\n", output_file);
  for (size_t z = 0; z < _nz; ++z)
  {
    for (size_t y = 0; y < _ny; ++y)
    {
      fputs(data_text[z * _ny + y].c_str(), output_file);
      fputs("\n", output_file);
    }
  }
  fclose(output_file);
}

void
PointValueFileWriter::BoundaryElements()
{
  _boundary_elements.clear();
  for (int z = 0; z < _nz; ++z)
    for (int y = 0; y < _ny; ++y)
      for (int x = 0; x < _nx; ++x)
        if (_data[z * _ny + y][x] == _boundary_element_value)
          Neighbours(z, y, x);
  _console << "_boundary_elements.size() = " << _boundary_elements.size() << std::endl;
}

void
PointValueFileWriter::Neighbours(int z, int y, int x)
{
  if (_neighbours == "diag3D")
  {
    for (int dir = -1; dir < 2; ++dir)
      for (int dir2 = -1; dir2 < 2; ++dir2)
        for (int dir3 = -1; dir3 < 2; ++dir3)
          if (CheckBoundary(z + dir, y + dir2, x + dir3))
          {
            PushBoundaryElement(z, y, x);
            return;
          }
  }
  else
  {
    for (int dir = -1; dir < 2; dir += 2)
      for (int axe = 0; axe < 3; ++axe)
        if (CheckBoundary(
                z + (axe == 2 ? dir : 0), y + (axe == 1 ? dir : 0), x + (axe == 0 ? dir : 0)))
        {
          PushBoundaryElement(z, y, x);
          return;
        }
    if (_neighbours == "diag2D")
      for (int dir = -1; dir < 2; dir += 2)
        for (int dir2 = -1; dir2 < 2; dir2 += 2)
        {
          if (CheckBoundary(z, y + dir, x + dir2))
          {
            PushBoundaryElement(z, y, x);
            return;
          }
          if (CheckBoundary(z + dir, y, x + dir2))
          {
            PushBoundaryElement(z, y, x);
            return;
          }
          if (CheckBoundary(z + dir, y + dir2, x))
          {
            PushBoundaryElement(z, y, x);
            return;
          }
        }
  }
}

// void
// PointValueFileWriter::BoundaryElements()
// {
//   _boundary_elements.clear();
//   for (int z = 0; z < _nz; ++z)
//     for (int y = 0; y < _ny; ++y)
//       for (int x = 0; x < _nx; ++x)
//         if (_data[z * _ny + y][x] != _boundary_element_value)
//           for (int axe = 0; axe < 3; ++axe)
//             for (int dir = -1; dir < 2; dir += 2)
//               if (CheckBoundary(
//                       z + (axe == 2 ? dir : 0), y + (axe == 1 ? dir : 0), x + (axe == 0 ? dir :
//                       0)))
//               {
//                 // direct neighbor element is a boundary element
//                 PushBoundaryElement(
//                     z + (axe == 2 ? dir : 0), y + (axe == 1 ? dir : 0), x + (axe == 0 ? dir :
//                     0));
//                 if (_neighbours == "diag2D" || _neighbours == "diag3D")
//                   for (int axe2 = 0; axe2 < 3; ++axe2)
//                     if (axe2 != axe)
//                       for (int dir2 = -1; dir2 < 2; dir2 += 2)
//                         if (CheckBoundary(z + (axe2 == 2 ? dir2 : 0),
//                                           y + (axe2 == 1 ? dir2 : 0),
//                                           x + (axe2 == 0 ? dir2 : 0)))
//                         {
//                           // 2D diagonal neighbor element is a boundary element
//                           PushBoundaryElement(z + (axe2 == 2 ? dir2 : 0) + (axe == 2 ? dir : 0),
//                                               y + (axe2 == 1 ? dir2 : 0) + (axe == 1 ? dir : 0),
//                                               x + (axe2 == 0 ? dir2 : 0) + (axe == 0 ? dir : 0));
//                           if (_neighbours == "diag3D")
//                             for (int axe3 = 0; axe3 < 3; ++axe3)
//                               if (axe3 != axe && axe3 != axe2)
//                                 for (int dir3 = -1; dir3 < 2; dir3 += 2)
//                                   if (CheckBoundary(z + (axe3 == 2 ? dir3 : 0),
//                                                     y + (axe3 == 1 ? dir3 : 0),
//                                                     x + (axe3 == 0 ? dir3 : 0)))
//                                     // 3D diagonal  neighbor element is a boundary element
//                                     PushBoundaryElement(
//                                         z + (axe3 == 2 ? dir3 : 0) + (axe2 == 2 ? dir2 : 0) +
//                                             (axe == 2 ? dir : 0),
//                                         y + (axe3 == 1 ? dir3 : 0) + (axe2 == 1 ? dir2 : 0) +
//                                             (axe == 1 ? dir : 0),
//                                         x + (axe3 == 0 ? dir3 : 0) + (axe2 == 0 ? dir2 : 0) +
//                                             (axe == 0 ? dir : 0));
//                         }
//               }
//   _console << "_boundary_elements.size() = " << _boundary_elements.size() << std::endl;
// }

bool
PointValueFileWriter::CheckBoundary(int z, int y, int x)
{
  if (z >= 0 && z < _nz && y >= 0 && y < _ny && x >= 0 && x < _nx &&
      _data[z * _ny + y][x] != _boundary_element_value)
    // checking if neighbor is not boundary_element
    return true;
  return false;
}

void
PointValueFileWriter::PushBoundaryElement(int z, int y, int x)
{
  if (std::find(_boundary_elements.begin(),
                _boundary_elements.end(),
                std::make_pair(z * _ny + y, x)) == _boundary_elements.end())
    _boundary_elements.push_back(std::make_pair(z * _ny + y, x));
}
