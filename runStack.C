#include "MacroCommon.hh"
#include "TString.h"

#include <vector>
#include <utility>

typedef std::pair<TString,Bool_t> SamplePair;
typedef std::vector<SamplePair> SamplePairVec;
typedef SamplePairVec::iterator SamplePairVecIter;

void runStack(){
  setupcpp11();
  gROOT->LoadMacro("StackPlots.cpp++g");

  // define inputs in pairs

  SamplePairVec Samples;
  Samples.push_back(SamplePair("doublemu",false));
  Samples.push_back(SamplePair("zmumu",true));
  Samples.push_back(SamplePair("ttbar",true));

  StackPlots * stacker = new StackPlots(Samples,"stack","png");
  stacker->DoStacks();
  delete stacker;
}
