//* This file is part of the MOOSE framework
//* https://www.mooseframework.org

#include "RedbackFullSolveMultiApp.h"
#include "FullSolveMultiApp.h"

#include <fstream>
#include <sstream>
#include <iterator>
#include <cstdio>

registerMooseObject("RedbackApp", RedbackFullSolveMultiApp);

InputParameters
RedbackFullSolveMultiApp::validParams()
{
  InputParameters params = FullSolveMultiApp::validParams();

  params.addRequiredParam<FileName>(
      "file",
      "Name of the txt file with the porosity change");

  params.addParam<FileName>(
      "times_file",
      "",
      "Name of the txt file to write the time values");

  params.addRequiredParam<FileName>(
      "upper_layer_file",
      "Name of the txt file with the next layer threshold");

  params.addRequiredParam<FileName>(
      "lower_layer_file",
      "Name of the txt file with the last layer threshold");

  return params;
}

RedbackFullSolveMultiApp::RedbackFullSolveMultiApp(const InputParameters & parameters)
  : FullSolveMultiApp(parameters),
    _times_file(getParam<FileName>("times_file")),
    _porosity_change(0.0),
    _porosity_change_old(0.0)
{
  // initialize times file if requested
  if (!_times_file.empty())
  {
    FILE * f = fopen(_times_file.c_str(), "w");
    if (f)
    {
      fputs("", f);
      fclose(f);
    }
  }
}

bool
RedbackFullSolveMultiApp::solveStep(Real dt, Real target_time, bool auto_advance)
{
  // -----------------------------
  // Read driving scalar values
  // -----------------------------
  _porosity_change = readFile(getParam<FileName>("file"));

  const Real upper_layer_threshold =
      readFile(getParam<FileName>("upper_layer_file"));

  const Real lower_layer_threshold =
      readFile(getParam<FileName>("lower_layer_file"));

  // -----------------------------
  // Decide whether to skip solve
  // -----------------------------
  const Real delta = _porosity_change - _porosity_change_old;

  if (delta >= 0 && std::abs(_porosity_change) < upper_layer_threshold)
    return true;

  if (delta <= 0 && std::abs(_porosity_change) > lower_layer_threshold)
    return true;

  // accept update
  _porosity_change_old = _porosity_change;

  // -----------------------------
  // Log time if requested
  // -----------------------------
  if (!_times_file.empty())
  {
    FILE * f = fopen(_times_file.c_str(), "a");
    if (f)
    {
      fputs(" ", f);
      fputs(std::to_string(target_time).c_str(), f);
      fclose(f);
    }
  }

  // -----------------------------
  // IMPORTANT: delegate to base class
  // -----------------------------
  return FullSolveMultiApp::solveStep(dt, target_time, auto_advance);
}

Real
RedbackFullSolveMultiApp::readFile(const FileName & file_name)
{
  std::ifstream file(file_name.c_str());
  if (!file.is_open())
    return 0.0;

  std::string line;
  Real value = 0.0;

  while (std::getline(file, line))
  {
    std::stringstream ss(line);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;

    std::vector<std::string> tokens(begin, end);

    if (tokens.empty())
      continue;

    try
    {
      value = std::stod(tokens[0]);
    }
    catch (...)
    {
      mooseWarning("Could not parse value in file: " + file_name);
    }

    break;
  }

  return value;
}