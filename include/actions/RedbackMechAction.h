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

#ifndef REDBACKMECHACTION_H
#define REDBACKMECHACTION_H

#include "Action.h"

class RedbackMechAction : public Action
{
public:
  RedbackMechAction(const InputParameters & params);

  static InputParameters validParams();

  virtual void act();

private:
  const std::string _disp_x;
  const std::string _disp_y;
  const std::string _disp_z;
  const std::string _disp_r;
  const std::string _temp;
  const std::string _pore_pres;

protected:
  /// if this vector is not empty the variables, kernels and materials are restricted to these subdomains
  std::vector<SubdomainName> _subdomain_names;
  /// indicates, if the vector of subdomain names is set (dont set block restrictions, if not)
  const bool _subdomain_names_set;
};

#endif // REDBACKMECHACTION_H
