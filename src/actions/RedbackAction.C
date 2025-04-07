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

#include "RedbackAction.h"

#include "FEProblem.h"
#include "Factory.h"
#include "Parser.h"

InputParameters
RedbackAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addParam<NonlinearVariableName>("disp_x", "", "The x displacement");
  params.addParam<NonlinearVariableName>("disp_y", "", "The y displacement");
  params.addParam<NonlinearVariableName>("disp_z", "", "The z displacement");
  params.addParam<NonlinearVariableName>("temp", "", "The temperature");
  params.addParam<NonlinearVariableName>("pore_pres", "", "The pore fluid pressure");
  params.addParam<std::string>(
      "appended_property_name", "", "Name appended to material properties to make them unique");

  // changed this from true to false
  params.set<bool>("use_displaced_mesh") = true;
  // params.set<bool>("use_displaced_mesh") = false;
  return params;
}

RedbackAction::RedbackAction(InputParameters params)
  : Action(params),
    _disp_x(getParam<NonlinearVariableName>("disp_x")),
    _disp_y(getParam<NonlinearVariableName>("disp_y")),
    _disp_z(getParam<NonlinearVariableName>("disp_z")),
    _temp(getParam<NonlinearVariableName>("temp")),
    _pore_pres(getParam<NonlinearVariableName>("pore_pres"))
{
}

void
RedbackAction::act()
{
  // unsigned int dim = 1;
  std::vector<std::string> keys;
  std::vector<VariableName> vars;
  std::string type("RedbackStressDivergenceTensors");

  if (_disp_x == "")
    mooseError("disp_x must be specified");

  keys.push_back("disp_x");
  vars.push_back(_disp_x);
  if (_disp_y != "")
  {
    // ++dim;
    keys.push_back("disp_y");
    vars.push_back(_disp_y);
    if (_disp_z != "")
    {
      // ++dim;
      keys.push_back("disp_z");
      vars.push_back(_disp_z);
    }
  }

  //unsigned int num_coupled(dim);
  if (_temp != "")
  {
    //++num_coupled;
    keys.push_back("temp");
    vars.push_back(_temp);
  }
  if (_pore_pres != "")
  {
    //++num_coupled;
    keys.push_back("pore_pres");
    vars.push_back(_pore_pres);
  }

  /*
  // Create divergence objects
  std::string short_name(_name);
  // Chop off "TensorMechanics/"
  short_name.erase(0, 15);

  InputParameters params = _factory.getValidParams(type);
  for (unsigned int j = 0; j < num_coupled; ++j)
  {
    params.addCoupledVar(keys[j], "");
    params.set<std::vector<VariableName> >(keys[j]) =
  std::vector<VariableName>(1, vars[j]);
  }

  params.set<bool>("use_displaced_mesh") = getParam<bool>("use_displaced_mesh");
  params.set<std::string>("appended_property_name") =
  getParam<std::string>("appended_property_name");

  for (unsigned int i = 0; i < dim; ++i)
  {
    std::stringstream name;
    name << "Kernels/";
    name << short_name;
    name << i;

    params.set<unsigned int>("component") = i;
    params.set<NonlinearVariableName>("variable") = vars[i];

    _problem->addKernel(type, name.str(), params);
  }
  */
}
