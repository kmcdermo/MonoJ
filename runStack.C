#include <vector>
#include <utility>

typedef std::vector<std::pair<TString,Bool_t> > SampleVec;

void setupcpp11();

void runStack(){
  setupcpp11();
  gROOT->LoadMacro("StackPlots.cpp++g");

  // define inputs in pairs
  
  SampleVec samples;
  // second in pair isMC --> set to true if MC, data false
  samples.push_back(std::make_pair("zmumu",false));
  samples.push_back(std::make_pair("zmumu",true));
  
  StackPlots * stacker = new StackPlots(samples,"stack","png");
  stacker->DoStacking();
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
