#include <vector>

typedef std::vector<TString> TStrVec;

void setupcpp11();

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


  //  StackPlots stacker(data,mc,dplots,iplots,"stack","png");
  //  stacker.DoStacks();
  StackPlots * stacker = new StackPlots(data,mc,dplots,iplots,"stack","png");
  stacker->DoStacks();
  delete stacker;
}

void setupcpp11(){ // customize ACLiC's behavior ...
  TString o;
  // customize MakeSharedLib
  o = TString(gSystem->GetMakeSharedLib());
  o = o.ReplaceAll(" -c ", " -std=c++0x -c ");
  gSystem->SetMakeSharedLib(o.Data());
  // customize MakeExe
  o = TString(gSystem->GetMakeExe());
  o = o.ReplaceAll(" -c ", " -std=c++0x -c ");
  gSystem->SetMakeExe(o.Data());
}
