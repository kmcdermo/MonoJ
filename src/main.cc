#include "../interface/Common.hh"

#include "../interface/Analysis.hh"
#include "../interface/StackPlots.hh"
#include "../interface/PUReweight.hh"

#include "TROOT.h"

#include <iostream>

int main(){
  // set TDR Style
  gROOT->ProcessLine(".x ~/.rootlogon.C"); // hack way to set root logon macro ... could put this into Typedef.hh 
  
  // Variables needed in all functions for plotting and the like so it is universal
  // Color for MC Stacks
  ColorMap colorMap;
  colorMap["zll"] = kCyan;
  colorMap["wln"] = kGreen+3;
  colorMap["ww"]  = kYellow-7;
  colorMap["zz"]  = kPink-4;
  colorMap["wz"]  = kMagenta+2;
  colorMap["top"] = kRed+2;
  colorMap["qcd"] = kOrange+7;

  // nPV needs nBins set for PU reweights and actual plots
  Int_t nBins_vtx = 50;

  // Allow user to set output directory for whole project--> if running only stacking... will need to specify inputs in .cc file
  TString outdir = "justdata";

  // Allow user to set outtype for plots
  TString outtype = "png";

  // First make total output directory ... sub directories made inside objects
  MakeOutDirectory(outdir);

  //++++++++++++++++++++++++++++++++++++++++// 
  //                                        //
  //                 Analysis               //
  //                                        //
  //++++++++++++++++++++++++++++++++++++++++// 

  // Important variables to set for analysis

  // Total Integrated Luminosity
  Double_t lumi = 0.04024; // int lumi 

  // Selection we want (zmumu = zpeak with muons, zelel = zpeak with electrons, singleel)
  TString selection = "zmumu";

  //========================================// 
  //             PU Reweighting             //
  //========================================// 

  // currently set for z->mumu peak matchng selection in Analysis.cc, could make this a compiled code eventually

  TString PURWselection = "zmumu";
  std::cout << Form("Do PU reweighting first with %s!",PURWselection.Data()) << std::endl;
  
  SamplePairVec PURWSamples;
  PURWSamples.push_back(SamplePair("doublemu",false));
  PURWSamples.push_back(SamplePair("zll",true));

  PUReweight * reweight = new PUReweight(PURWSamples,PURWselection,lumi,nBins_vtx,outdir,outtype);

  Bool_t doReWeight = true; // false if no actual reweighting to be performed
  DblVec puweights = reweight->GetPUWeights(doReWeight); 

  delete reweight;
  std::cout << "Finished PU reweighting, now begin Analysis" << std::endl;

  //========================================// 
  //      Produce Plots Per Sample          //
  //========================================// 

  // run over data + MC samples (dibosons and the like) first, then Top samples, then QCD
  // -------------------------------------- //
  // Largest samples to process 
  SamplePairVec Samples;
  Samples.push_back(SamplePair("doublemu",false));
  Samples.push_back(SamplePair("zll",true));
  Samples.push_back(SamplePair("wln",true));
  Samples.push_back(SamplePair("ww",true));
  Samples.push_back(SamplePair("zz",true));
  Samples.push_back(SamplePair("wz",true));

  for (SamplePairVecIter iter = Samples.begin(); iter != Samples.end(); ++iter) {
    std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
    Analysis sample((*iter),selection,puweights,lumi,nBins_vtx,colorMap,outdir,outtype);
    sample.DoAnalysis();
  }

  std::cout << "Done with Analysis ... now do Top Analysis" << std::endl;  

  // -------------------------------------- //
  // top backgrounds
  SamplePairVec TopSamples;
  TopSamples.push_back(SamplePair("ttbar",true)); 
  TopSamples.push_back(SamplePair("singlett",true)); 
  TopSamples.push_back(SamplePair("singletbart",true)); 
  TopSamples.push_back(SamplePair("singletw",true)); 
  TopSamples.push_back(SamplePair("singletbarw",true)); 

  for (SamplePairVecIter iter = TopSamples.begin(); iter != TopSamples.end(); ++iter) {
    std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
    Analysis sample((*iter),selection,puweights,lumi,nBins_vtx,colorMap,outdir,outtype);
    sample.DoAnalysis();
  }

  std::cout << "Done with Top Analysis ... now Hadd Top" << std::endl;  
  Hadd(TopSamples,outdir,selection,"top");
  Samples.push_back(SamplePair("top",true)); // add hadded file to total samples for stacking
  std::cout << "Done with Top Hadd ... now do QCD Analysis" << std::endl;  

  // -------------------------------------- //
  // QCD backgrounds
  SamplePairVec QCDSamples;
  QCDSamples.push_back(SamplePair("qcd15to30",true)); 
  QCDSamples.push_back(SamplePair("qcd30to50",true)); 
  QCDSamples.push_back(SamplePair("qcd50to80",true)); 
  QCDSamples.push_back(SamplePair("qcd80to120",true)); 
  QCDSamples.push_back(SamplePair("qcd120to170",true)); 
  QCDSamples.push_back(SamplePair("qcd170to300",true)); 
  QCDSamples.push_back(SamplePair("qcd300to470",true)); 
  QCDSamples.push_back(SamplePair("qc470to600",true)); 
  QCDSamples.push_back(SamplePair("qcd600to800",true));
  QCDSamples.push_back(SamplePair("qcd800to1000",true));
  QCDSamples.push_back(SamplePair("qcd1000t1400",true)); 
  QCDSamples.push_back(SamplePair("qcd1400to1800",true));
  QCDSamples.push_back(SamplePair("qcd1800to2400",true));
  QCDSamples.push_back(SamplePair("qcd2400to3200",true));
  QCDSamples.push_back(SamplePair("qcd3200toinf",true)); 

  for (SamplePairVecIter iter = QCDSamples.begin(); iter != QCDSamples.end(); ++iter) {
    std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
    Analysis sample((*iter),selection,puweights,lumi,nBins_vtx,colorMap,outdir,outtype);
    sample.DoAnalysis();
  }

  std::cout << "Done with QCD Analysis ... now hadd QCD" << std::endl;  
  Hadd(QCDSamples,outdir,selection,"qcd");
  Samples.push_back(SamplePair("qcd",true)); // add hadded file to total samples for stacking
  std::cout << "Done with QCD Hadd ... now make all stack plots" << std::endl;  

  //========================================// 
  //        Stack Plots Production          //
  //========================================// 

  StackPlots * stacker = new StackPlots(Samples,selection,lumi,colorMap,outdir,outtype);
  stacker->DoStacks();
  delete stacker;
}


