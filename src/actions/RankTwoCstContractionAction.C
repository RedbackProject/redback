/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "FEProblem.h"
#include "Factory.h"
#include "Parser.h"
#include "RankTwoCstContractionAction.h"

registerMooseAction("RedbackApp", RankTwoCstContractionAction, "add_postprocessor");

InputParameters
RankTwoCstContractionAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addRequiredParam<MaterialPropertyName>("rank_two_tensor", "The rank two material tensor name");
  params.addParam<FunctionName>("D00", "Boundary deformation rate Dxx");
  params.addParam<FunctionName>("D11", "Boundary deformation rate Dyy");
  params.addParam<FunctionName>("D22", "Boundary deformation rate Dzz");
  params.addParam<bool>(
    "compute_on_boundary", false, "Allows macro dissipation to be computed on boundaries instead of blocks");
  params.addParam<std::string>("base_name", "Material property base name");
  params.addParam<std::vector<SubdomainName> >("block",
                                               "The list of block ids (SubdomainID) that this object will be applied");
  params.addParam<std::vector<BoundaryName> >(
    "boundary", "The list of boundary IDs from the mesh that this object will be applied");
  return params;
}

RankTwoCstContractionAction::RankTwoCstContractionAction(const InputParameters & params) :
    Action(params), _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : "")
{
}

void
RankTwoCstContractionAction::act()
{
  MaterialPropertyName tensor = getParam<MaterialPropertyName>("rank_two_tensor");
  std::string postprocessor = "MaterialTensorIntegral";
  if (getParam<bool>("compute_on_boundary"))
    postprocessor = "MaterialTensorSideIntegral";

  for (int j = 0; j < LIBMESH_DIM; j++)
  {
    for (int k = 0; k < LIBMESH_DIM; k++)
    {
      InputParameters pp_params = _factory.getValidParams(postprocessor);
      pp_params.set<MaterialPropertyName>("rank_two_tensor") = tensor;
      pp_params.set<unsigned int>("index_i") = j;
      pp_params.set<unsigned int>("index_j") = k;
      pp_params.set<std::vector<OutputName> >("outputs") = { "none" };
      if (getParam<bool>("compute_on_boundary") && isParamValid("boundary"))
        pp_params.set<std::vector<BoundaryName> >("boundary") = getParam<std::vector<BoundaryName> >("boundary");
      else if (isParamValid("block"))
        pp_params.set<std::vector<SubdomainName> >("block") = getParam<std::vector<SubdomainName> >("block");
      _problem->addPostprocessor(postprocessor,
                                 _base_name + std::string("RankTwoCstContractionAction_") + std::string(tensor) +
                                   std::to_string(j) + std::to_string(k),
                                 pp_params);
    }
  }

  InputParameters pp_params = _factory.getValidParams("EmptyPostprocessor");
  pp_params.set<std::vector<OutputName> >("outputs") = { "none" };
  _problem->addPostprocessor("EmptyPostprocessor", _base_name + std::string("empty"), pp_params);
  for (int i = 0; i < 3; i++)
  {
    InputParameters pp_params = _factory.getValidParams("FunctionValuePostprocessor");
    pp_params.set<std::vector<OutputName> >("outputs") = { "none" };
    pp_params.set<FunctionName>("function") =
      getParam<FunctionName>(std::string("D") + std::to_string(i) + std::to_string(i));
    _problem->addPostprocessor(
      "FunctionValuePostprocessor", _base_name + std::string("D") + std::to_string(i) + std::to_string(i), pp_params);
  }

  pp_params = _factory.getValidParams("RankTwoContractionPostprocessor");
  for (int j = 0; j < LIBMESH_DIM; j++)
  {
    for (int k = 0; k < LIBMESH_DIM; k++)
    {
      pp_params.set<PostprocessorName>(std::to_string(1) + std::string("_index") + std::to_string(j) +
                                       std::to_string(k)) = _base_name + std::string("RankTwoCstContractionAction_") +
                                                            std::string(tensor) + std::to_string(j) +
                                                            std::to_string(k);
      pp_params.set<PostprocessorName>(std::to_string(2) + std::string("_index") + std::to_string(j) +
                                       std::to_string(k)) = _base_name + std::string("empty");
    }
  }
  for (int i = 0; i < 3; i++)
  {
    pp_params.set<PostprocessorName>(std::to_string(2) + std::string("_index") + std::to_string(i) +
                                     std::to_string(i)) =
      _base_name + std::string("D") + std::to_string(i) + std::to_string(i);
  }
  _problem->addPostprocessor(
    "RankTwoContractionPostprocessor", _base_name + std::string(tensor) + std::string("-Dij"), pp_params);
}
