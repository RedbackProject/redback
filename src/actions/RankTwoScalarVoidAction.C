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
#include "RankTwoScalarVoidAction.h"
#include "RankTwoScalarTools.h"

registerMooseAction("RedbackApp", RankTwoScalarVoidAction, "add_postprocessor");

InputParameters
RankTwoScalarVoidAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addRequiredParam<MultiMooseEnum>(
      "scalar_type",
      MultiMooseEnum("VonMisesStress EffectiveStrain Hydrostatic L2norm "
                     "VolumetricStrain FirstInvariant SecondInvariant "
                     "ThirdInvariant TriaxialityStress"),
      "Type of scalar outputs");
  params.addParam<std::vector<BoundaryName>>(
      "boundary", "The list of boundary IDs from the mesh that this object will be applied");

  return params;
}

RankTwoScalarVoidAction::RankTwoScalarVoidAction(const InputParameters & params) : Action(params) {}

void
RankTwoScalarVoidAction::act()
{
  const MultiMooseEnum scalar_types = getParam<MultiMooseEnum>("scalar_type");

  for (int j = 0; j < LIBMESH_DIM; j++)
  {
    for (int k = 0; k < LIBMESH_DIM; k++)
    {
      InputParameters pp_params = _factory.getValidParams("SideIntegralVoidPostprocessor");
      pp_params.set<std::vector<VariableName>>("variable") = {"disp_x"};
      pp_params.set<std::vector<VariableName>>("disp_x") = {"disp_x"};
      pp_params.set<std::vector<VariableName>>("disp_y") = {"disp_y"};
      pp_params.set<std::vector<VariableName>>("disp_z") = {"disp_z"};
      pp_params.set<unsigned int>("index_i") = j;
      pp_params.set<unsigned int>("index_j") = k;
      pp_params.set<std::vector<OutputName>>("outputs") = {"none"};
      pp_params.set<std::vector<BoundaryName> >("boundary") = getParam<std::vector<BoundaryName> >("boundary");
      _problem->addPostprocessor("SideIntegralVoidPostprocessor",
                                std::string("RankTwoScalarVoidAction_") +
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
            std::string("RankTwoScalarVoidAction_") + std::to_string(j) + std::to_string(k);
      }
    }
    pp_params.set<MooseEnum>("scalar_type") = scalar_types[j];
    _problem->addPostprocessor("RankTwoScalarPostprocessor",
                               std::string("void_strain_") +
                                   std::string(scalar_types[j]),
                               pp_params);
  }

  // InputParameters pp_params = _factory.getValidParams("VolumePostprocessor");
  // if (isParamValid("block"))
  // {
  //   pp_params.set<std::vector<SubdomainName> >("block") = getParam<std::vector<SubdomainName>
  //   >("block");
  // }
  // _problem->addPostprocessor("VolumePostprocessor", "Volume", pp_params);
}
