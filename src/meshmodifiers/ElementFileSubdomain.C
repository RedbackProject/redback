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

#include "ElementFileSubdomain.h"
#include "MooseMesh.h"
#include "CastUniquePointer.h"
#include "libmesh/elem.h"
#include <fstream>

registerMooseObject("RedbackApp", ElementFileSubdomain);

InputParameters
ElementFileSubdomain::validParams()
{
  InputParameters params = MeshGenerator::validParams();
  params.addRequiredParam<std::vector<SubdomainID>>("subdomain_ids",
                                                    "New subdomain IDs of all elements");
  params.addParam<std::vector<dof_id_type>>("element_ids", "New subdomain IDs of all elements");
  params.addParam<FileName>("file", "Name of the txt file with the elements");
  params.addRequiredParam<MeshGeneratorName>("input", "The mesh to modify");
  return params;
}

ElementFileSubdomain::ElementFileSubdomain(const InputParameters & parameters)
    : MeshGenerator(parameters),
      _input(getMesh("input"))

{
}

ElementFileSubdomain::~ElementFileSubdomain() {}

std::unique_ptr<MeshBase>
ElementFileSubdomain::generate()
{
  // Reference the the libMesh::MeshBase
  std::unique_ptr<MeshBase> mesh = std::move(_input);

  std::vector<SubdomainID> bids = getParam<std::vector<SubdomainID>>("subdomain_ids");

  // Generate a list of elements to which new subdomain IDs are to be assigned
  std::vector<Elem *> elements;
  if (isParamValid("element_ids") || isParamValid("file"))
  {
    FileName file = getParam<FileName>("file");
    std::vector<dof_id_type> elemids;
    std::string line;
    std::ifstream myfile(file.c_str());
    if (myfile.is_open())
    {
      while (myfile.good())
      {
        getline(myfile, line);
        std::stringstream ss(line);
        std::istream_iterator<std::string> begin(ss);
        std::istream_iterator<std::string> end;
        std::vector<std::string> vstrings(begin, end);

        for (unsigned int i = 0; i < vstrings.size(); ++i)
        {
          elemids.push_back((dof_id_type)atoi(vstrings[i].c_str()));
        }
      }
      myfile.close();
    }
    else
    {
      mooseWarning("Unable to open file");
      elemids = getParam<std::vector<dof_id_type>>("element_ids");
    }
    // std::vector<dof_id_type> elemids = getParam<std::vector<dof_id_type>
    // >("element_ids");
    for (dof_id_type i = 0; i < elemids.size(); ++i)
    {
      Elem * elem = mesh->query_elem_ptr(elemids[i]);
      if (!elem)
        mooseError("invalid element ID is in element_ids");
      else
        elements.push_back(elem);
    }
  }
  else
  {
    bool has_warned_remapping = false;
    MeshBase::const_element_iterator el = mesh->elements_begin();
    const MeshBase::const_element_iterator end_el = mesh->elements_end();
    for (dof_id_type e = 0; el != end_el; ++el, ++e)
    {
      Elem * elem = *el;
      if (elem->id() != e && (!has_warned_remapping))
      {
        mooseWarning("ElementFileSubdomain will ignore the element remapping");
        has_warned_remapping = true;
      }
      elements.push_back(elem);
    }
  }

  if (bids.size() != elements.size() && bids.size() != 1)
    mooseError(" Size of subdomain_ids is not consistent with the number of elements");

  // Assign new subdomain IDs and make sure elements in different types are not
  // assigned with the same subdomain ID
  std::map<ElemType, std::set<SubdomainID>> type2blocks;
  for (dof_id_type e = 0; e < elements.size(); ++e)
  {
    Elem * elem = elements[e];
    ElemType type = elem->type();
    SubdomainID newid;
    if (bids.size() == 1)
      newid = bids[0];
    else
      newid = bids[e];

    bool has_type = false;
    for (std::map<ElemType, std::set<SubdomainID>>::iterator it = type2blocks.begin();
         it != type2blocks.end();
         ++it)
    {
      if (it->first == type)
      {
        has_type = true;
        it->second.insert(newid);
      }
      else if (it->second.count(newid) > 0)
        mooseError("trying to assign elements with different types with the "
                   "same subdomain ID");
    }
    if (!has_type)
    {
      std::set<SubdomainID> blocks;
      blocks.insert(newid);
      type2blocks.insert(std::pair<ElemType, std::set<SubdomainID>>(type, blocks));
    }

    elem->subdomain_id() = newid;
  }
  return dynamic_pointer_cast<MeshBase>(mesh);
}
