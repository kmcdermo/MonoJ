#include "MacroCommon.hh"
#include "TString.h"
#include <vector>

typedef std::vector<TString> TStrVec;

void runStack(){
  setupcpp11();
  gROOT->LoadMacro("StackPlots.cpp++g");

  // define inputs in pairs

  TStrVec data;
  data.push_back("doublemu");
  TStrVec mc;
  mc.push_back("zmumu");
  TStrVec dplots;
  dplots.push_back("zmass");
  TStrVec iplots;      

  StackPlots * stacker = new StackPlots(data,mc,dplots,iplots,"stack","png");
  stacker->DoStacks();
  delete stacker;
}
