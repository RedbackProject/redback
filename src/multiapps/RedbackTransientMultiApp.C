#include "RedbackTransientMultiApp.h"

#include <chrono>
#include <thread>
#include <fstream>

registerMooseObject("RedbackApp", RedbackTransientMultiApp);

InputParameters
RedbackTransientMultiApp::validParams()
{
  InputParameters params = TransientMultiApp::validParams();

  params.addRequiredParam<FileName>(
      "multiapp_file",
      "Name of the file to know if multiapp should be run");

  return params;
}

RedbackTransientMultiApp::RedbackTransientMultiApp(
    const InputParameters & parameters)
  : TransientMultiApp(parameters),
    _multiapp_file(getParam<FileName>("multiapp_file"))
{
}

bool
RedbackTransientMultiApp::solveStep(
    Real dt,
    Real target_time,
    bool auto_advance)
{
  using namespace std::this_thread;
  using namespace std::chrono;

  sleep_for(milliseconds(10));

  Real run_multiapp = ReadFile(_multiapp_file);

  if (run_multiapp == 0)
    return true;

  return TransientMultiApp::solveStep(
      dt,
      target_time,
      auto_advance);
}

Real
RedbackTransientMultiApp::ReadFile(const FileName & file_name)
{
  Real value = 0;
  std::string line;
  std::ifstream myfile(file_name.c_str());
  if (myfile.is_open())
  {
    while (myfile.good())
    {
      getline(myfile, line);
      std::stringstream ss(line);
      std::istream_iterator<std::string> begin(ss);
      std::istream_iterator<std::string> end;
      std::vector<std::string> vstrings(begin, end);
      if (vstrings.size() == 0)
      {
        mooseWarning("file is empty");
        break;
      }
      value = std::stod(vstrings[0].c_str());
      break;
    }
    myfile.close();
  }
  return value;
}
