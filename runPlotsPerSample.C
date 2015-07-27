#include "MacroCommon.hh"

#include <vector>
#include <utility>

typedef std::pair<TString,Bool_t> SamplePair;
typedef std::vector<SamplePair> SamplePairVec;
typedef SamplePairVec::iterator SamplePairVecIter;

void runPlotsPerSample(){
  setupcpp11();

  gROOT->LoadMacro("Analysis.cpp++g");
  // First is channel
  // Second is MC/Data, if MC, isMC == 1, else isMC == 0

  // Third is output name of directory/rootfile/file plots
  // Fourth is output type of plots if SaveAs
  
  SamplePairVec Samples;
  Samples.push_back(SamplePair("doublemu",false));
  Samples.push_back(SamplePair("zmumu",true));
  Samples.push_back(SamplePair("ttbar",true));

  for (SamplePairVecIter iter = Samples.begin(); iter != Samples.end(); ++iter) {
    std::cout << "Processing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
    Analysis sample((*iter).first,(*iter).second,"png");
    sample.DoAnalysis();
  }

}
