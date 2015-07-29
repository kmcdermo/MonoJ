#include "Analysis.hh"
#include "StackPlots.hh"
#include "Common.hh"
#include "macros/PUReWeight.C"

#include "TROOT.h"

#include <iostream>

int main(){
  // set TDR Style
  gROOT->ProcessLine(".x ~/.rootlogon.C"); // hack way to set root logon macro ... could put this into Typedef.hh 
  
  // Variables needed in all functions for plotting and the like so it is universal
  
  // Color for MC Stacks
  ColorMap colorMap;
  colorMap["zmumu"] = kCyan;
  colorMap["zelel"] = kBlue+1;
  colorMap["wmunu"] = kGreen+3;
  colorMap["welnu"] = kGreen-7;
  colorMap["ttbar"] = kRed+2;

  // Total Integrated Luminosity
  Double_t lumi = 0.04024; // int lumi 

  // Selection we want (zpeak with muons, zpeak with electrons, etc)
  TString selection = "zmumu";

  // Samples to process 
  SamplePairVec Samples;
  Samples.push_back(SamplePair("doublemu",false));
  Samples.push_back(SamplePair("zmumu",true));
  Samples.push_back(SamplePair("ttbar",true));

  // Allow user to set outtype for plots
  TString outtype = "png";

  // Allow user to set output directory for whole project--> if running only stacking... will need to specify inputs in .cc file
  TString outdir = "firststudies_4024pb";

  //++++++++++++++++++++++++++++++++++++++++// Analysis begins here
  // First do PU reweight calculation ... will build separate object for it.  for now just use vector from macro
  std::cout << "Do PU reweighting with z -> mu mu peak" << std::endl;

  gROOT->ProcessLine(".L PUReWeight.C");
  DblVec puweights = PUReWeight();

  std::cout << "Finished PU reweighting, now begin Analysis" << std::endl;

  // First make total output directory ... sub directories made inside objects
  MakeOutDirectory(outdir);

  for (SamplePairVecIter iter = Samples.begin(); iter != Samples.end(); ++iter) {
    std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
    Analysis sample((*iter),selection,puweights,lumi,colorMap,outdir,outtype);
    sample.DoAnalysis();
  }
  
  std::cout << "Done with Analysis ... now make stack plots" << std::endl;

  StackPlots * stacker = new StackPlots(Samples,selection,lumi,colorMap,outdir,outtype);
  stacker->DoStacks();
  delete stacker;
}


