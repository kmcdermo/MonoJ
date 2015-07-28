#include <iostream>
#include "Analysis.hh"
#include "StackPlots.hh"
#include "Typedef.hh"

int main(){
  // Variables needed in all functions for plotting and the like so it is universal
  
  // Color for MC Stacks
  ColorMap colorMap;
  colorMap["zmumu"] = kCyan;
  colorMap["ttbar"] = kRed+2;

  // Total Integrated Luminosity
  Double_t lumi = 0.03650; // int lumi 36.50 pb^-1

  // Samples to process 
  SamplePairVec Samples;
  Samples.push_back(SamplePair("doublemu",false));
  Samples.push_back(SamplePair("zmumu",true));
  Samples.push_back(SamplePair("ttbar",true));

  // Allow user to set outtype for plots
  TString outtype = "png";

  // Allow user to set final output for stacks
  TString outdir = "stack";

  for (SamplePairVecIter iter = Samples.begin(); iter != Samples.end(); ++iter) {
    std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
    Analysis sample((*iter),lumi,colorMap,outtype);
    sample.DoAnalysis();
  }

  std::cout << "Done with Analysis ... now make stack plots" << std::endl;

  StackPlots * stacker = new StackPlots(Samples,lumi,colorMap,outdir,outtype);
  stacker->DoStacks();
  delete stacker;
}


