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
#include "RankTwoContractionAction.h"

registerMooseAction("RedbackApp", RankTwoContractionAction, "add_postprocessor");

InputParameters
RankTwoContractionAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addRequiredParam<std::vector<MaterialPropertyName> >("rank_two_tensor", "The rank two material tensors name");
  params.addParam<bool>(
    "compute_on_boundary", false, "Allows macro dissipation to be computed on boundaries instead of blocks");
  params.addParam<std::string>("base_name", "Material property base name");
  params.addParam<std::vector<SubdomainName> >("block",
                                               "The list of block ids (SubdomainID) that this object will be applied");
  params.addParam<std::vector<BoundaryName> >(
    "boundary", "The list of boundary IDs from the mesh that this object will be applied");
  return params;
}

RankTwoContractionAction::RankTwoContractionAction(const InputParameters & params) :
    Action(params), _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : "")
{
}

void
RankTwoContractionAction::act()
{
  std::vector<MaterialPropertyName> tensors = getParam<std::vector<MaterialPropertyName> >("rank_two_tensor");
  std::string postprocessor = "MaterialTensorIntegral";
  if (getParam<bool>("compute_on_boundary"))
    postprocessor = "MaterialTensorSideIntegral";

  for (int i = 0; i < 1; i++)
  {
    for (int j = 0; j < LIBMESH_DIM; j++)
    {
      for (int k = 0; k < LIBMESH_DIM; k++)
      {
        InputParameters pp_params = _factory.getValidParams("MaterialTensorIntegral");
        pp_params.set<MaterialPropertyName>("rank_two_tensor") = tensors[ i ];
        pp_params.set<unsigned int>("index_i") = j;
        pp_params.set<unsigned int>("index_j") = k;
        pp_params.set<std::vector<OutputName> >("outputs") = { "none" };
        pp_params.set<std::vector<SubdomainName> >("block") = getParam<std::vector<SubdomainName> >("block");
        _problem->addPostprocessor("MaterialTensorIntegral",
                                   _base_name + std::string("RankTwoContractionAction_") + std::string(tensors[ i ]) +
                                     std::to_string(j) + std::to_string(k),
                                   pp_params);
      }
    }
  }

  for (int j = 0; j < LIBMESH_DIM; j++)
  {
    for (int k = 0; k < LIBMESH_DIM; k++)
    {
      InputParameters pp_params = _factory.getValidParams(postprocessor);
      pp_params.set<MaterialPropertyName>("rank_two_tensor") = tensors[ 1 ];
      pp_params.set<unsigned int>("index_i") = j;
      pp_params.set<unsigned int>("index_j") = k;
      pp_params.set<std::vector<OutputName> >("outputs") = { "none" };
      if (getParam<bool>("compute_on_boundary") && isParamValid("boundary"))
        pp_params.set<std::vector<BoundaryName> >("boundary") = getParam<std::vector<BoundaryName> >("boundary");
      else if (isParamValid("block"))
        pp_params.set<std::vector<SubdomainName> >("block") = getParam<std::vector<SubdomainName> >("block");
      _problem->addPostprocessor(postprocessor,
                                 _base_name + std::string("RankTwoContractionAction_") + std::string(tensors[ 1 ]) +
                                   std::to_string(j) + std::to_string(k),
                                 pp_params);
    }
  }

  InputParameters pp_params = _factory.getValidParams("RankTwoContractionPostprocessor");
  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < LIBMESH_DIM; j++)
    {
      for (int k = 0; k < LIBMESH_DIM; k++)
      {
        pp_params.set<PostprocessorName>(std::to_string(i + 1) + std::string("_index") + std::to_string(j) +
                                         std::to_string(k)) = _base_name + std::string("RankTwoContractionAction_") +
                                                              std::string(tensors[ i ]) + std::to_string(j) +
                                                              std::to_string(k);
      }
    }
  }
  _problem->addPostprocessor("RankTwoContractionPostprocessor",
                             _base_name + std::string(tensors[ 0 ]) + std::string("-") + std::string(tensors[ 1 ]),
                             pp_params);
}
