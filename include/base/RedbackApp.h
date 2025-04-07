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

#ifndef REDBACKAPP_H
#define REDBACKAPP_H

#include "MooseApp.h"

class RedbackApp : public MooseApp
{
public:
  RedbackApp(InputParameters parameters);
  static InputParameters validParams();

  virtual ~RedbackApp();

  static void registerApps();
  static void registerAll(Factory & f, ActionFactory & af, Syntax & s);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* REDBACKAPP_H */
