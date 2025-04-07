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

#ifndef REDBACKACTION_H
#define REDBACKACTION_H

#include "Action.h"

class RedbackAction : public Action
{
public:
  RedbackAction(InputParameters params);

  static InputParameters validParams();

  virtual void act();

private:
  const std::string _disp_x;
  const std::string _disp_y;
  const std::string _disp_z;
  const std::string _temp;
  const std::string _pore_pres;
};

#endif // REDBACKACTION_H
