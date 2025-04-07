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

#include "FEProblem.h"
#include "Factory.h"
#include "Parser.h"
#include "RankTwoScalarAction.h"
#include "RankTwoScalarTools.h"

registerMooseAction("RedbackApp", RankTwoScalarAction, "add_postprocessor");

InputParameters
RankTwoScalarAction::validParams()
{
  // action to compute the homogenised invariant of a tensorial material property over a block or boundary
  // as an integral (need to divide manually by volume)
  InputParameters params = Action::validParams();
  params.addRequiredParam<std::vector<MaterialPropertyName>>("rank_two_tensor",
                                                             "The rank two material tensors name");
  params.addRequiredParam<MultiMooseEnum>(
      "scalar_type",
      MultiMooseEnum("VonMisesStress EffectiveStrain Hydrostatic L2norm "
                     "VolumetricStrain FirstInvariant SecondInvariant "
                     "ThirdInvariant TriaxialityStress"),
      "Type of scalar outputs");
  params.addParam<std::vector<SubdomainName>>(
      "block", "The list of block ids (SubdomainID) that this object will be applied");
  params.addParam<bool>(
    "compute_on_boundary", false, "Allows invariant to be computed on boundaries instead of blocks");
  params.addParam<std::vector<BoundaryName> >(
    "boundary", "The list of boundary IDs from the mesh that this object will be applied");
// TODO: create function to divide the integral by the volume
  return params;
}

RankTwoScalarAction::RankTwoScalarAction(const InputParameters & params) : Action(params) {}

void
RankTwoScalarAction::act()
{
  std::vector<MaterialPropertyName> tensors =
      getParam<std::vector<MaterialPropertyName>>("rank_two_tensor");
  const MultiMooseEnum scalar_types = getParam<MultiMooseEnum>("scalar_type");
  std::string postprocessor = "MaterialTensorIntegral";
  if (getParam<bool>("compute_on_boundary"))
    postprocessor = "MaterialTensorSideIntegral";

  for (unsigned int i = 0; i < tensors.size(); i++)
  {
    for (int j = 0; j < LIBMESH_DIM; j++)
    {
      for (int k = 0; k < LIBMESH_DIM; k++)
      {
        InputParameters pp_params = _factory.getValidParams(postprocessor);
        pp_params.set<MaterialPropertyName>("rank_two_tensor") = tensors[i];
        pp_params.set<unsigned int>("index_i") = j;
        pp_params.set<unsigned int>("index_j") = k;
        pp_params.set<std::vector<OutputName>>("outputs") = {"none"};
        if (getParam<bool>("compute_on_boundary") && isParamValid("boundary"))
          pp_params.set<std::vector<BoundaryName> >("boundary") = getParam<std::vector<BoundaryName> >("boundary");
        else if (isParamValid("block"))
          pp_params.set<std::vector<SubdomainName> >("block") = getParam<std::vector<SubdomainName> >("block");
        _problem->addPostprocessor(postprocessor,
                                  std::string("RankTwoScalarAction_") + std::string(tensors[ i ]) +
                                    std::to_string(j) + std::to_string(k), pp_params);
      }
    }

    for (unsigned int j = 0; j < scalar_types.size(); j++)
    {
      InputParameters pp_params = _factory.getValidParams("RankTwoScalarPostprocessor");
      for (int j = 0; j < LIBMESH_DIM; j++)
      {
        for (int k = 0; k < LIBMESH_DIM; k++)
        {
          pp_params.set<PostprocessorName>(std::string("index") + std::to_string(j) +
                                           std::to_string(k)) =
              std::string("RankTwoScalarAction_") + std::string(tensors[ i ]) + std::to_string(j) + std::to_string(k);
        }
      }
      pp_params.set<MooseEnum>("scalar_type") = scalar_types[j];
      _problem->addPostprocessor("RankTwoScalarPostprocessor",
                                 std::string(tensors[i]) + std::string("_") +
                                     std::string(scalar_types[j]),
                                 pp_params);
    }
  }

  // TODO: create function to divide the integral by the volume
  // InputParameters pp_params = _factory.getValidParams("VolumePostprocessor");
  // if (isParamValid("block"))
  // {
  //   pp_params.set<std::vector<SubdomainName> >("block") = getParam<std::vector<SubdomainName>
  //   >("block");
  // }
  // _problem->addPostprocessor("VolumePostprocessor", "Volume", pp_params);
}
