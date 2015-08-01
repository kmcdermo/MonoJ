#include "../interface/Common.hh"

#include "../interface/Analysis.hh"
#include "../interface/StackPlots.hh"
#include "../interface/PUReweight.hh"

#include "TROOT.h"

#include <iostream>
#include <fstream>


int main(){
  // set TDR Style
  gROOT->ProcessLine(".x ~/.rootlogon.C"); // hack way to set root logon macro ... could put this into Typedef.hh 
  
  // Variables needed in all functions for plotting and the like so it is universal
  // Color for MC Stacks
  ColorMap colorMap;
  colorMap["zll"]     = kCyan;
  colorMap["wln"]     = kGreen+3;
  colorMap["diboson"] = kMagenta+2;
  colorMap["top"]     = kRed+2;
  colorMap["qcd"]     = kOrange+7;
  colorMap["gamma"]   = kBlue-4;
  
  // nPV needs nBins set for PU reweights and actual plots
  Int_t nBins_vtx = 50;

  // Allow user to set output directory for whole project--> if running only stacking... will need to specify inputs in .cc file
  TString outdir = "test_zmumu";

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
  Double_t lumi = 0.04003; // int lumi in fb^-1

  // Selection we want (zmumu = zpeak with muons, zelel = zpeak with electrons, singlemu, singlephoton)
  TString selection = "zmumu";

  // Njets selection (==1, ==2) ... -1 = no selection
  Int_t njetsselection = -1;

  // Second make selection inside total directory
  TString njetsstr = "";
  if (njetsselection != -1){
    njetsstr = Form("_nj%i",njetsselection);
  }
  MakeOutDirectory(Form("%s/%s%s",outdir.Data(),selection.Data(),njetsstr.Data()));

  // now make ofstream object to store yield info
  ofstream yields(Form("%s/%s%s/yields.txt",outdir.Data(),selection.Data(),njetsstr.Data()),std::ios_base::app);
  yields << "Yields with selection: " << selection.Data() << " njets cut: " << ((njetsselection != -1) ? Form("%i",njetsselection) : "NONE") << std::endl;
  yields << "=============================================================" << std::endl << std::endl;
    
  //========================================// 
  //             PU Reweighting             //
  //========================================// 

  // currently set for z->mumu peak matchng selection in Analysis.cc, could make this a compiled code eventually
  
  TString PURWselection = "zmumu";
  Int_t   PURWnjetsselection = -1;
  std::cout << Form("Do PU reweighting first with %s selection, njets selection: %d!",PURWselection.Data(),PURWnjetsselection) << std::endl;
  
  SamplePairVec PURWSamples;
  PURWSamples.push_back(SamplePair("doublemu",false));
  PURWSamples.push_back(SamplePair("zll",true));
  //  PURWSamples.push_back(SamplePair("ttbar",true));

  PUReweight * reweight = new PUReweight(PURWSamples,PURWselection,PURWnjetsselection,lumi,nBins_vtx,outdir,outtype);

  Bool_t doReWeight = true; // false if no actual reweighting to be performed
  DblVec puweights = reweight->GetPUWeights(doReWeight); 
  
  delete reweight;
  
  // add info about reweighting technique to yields
  yields << "PU Reweighting Info" << std::endl;
  yields << "-------------------" << std::endl << std::endl;
  if (doReWeight) {
    yields << "Used PU Reweighting with selection: " << PURWselection.Data() << " njets cut: " << ((PURWnjetsselection != -1) ? Form("%i",PURWnjetsselection) : "NONE") << std::endl;
    // include samples used for reweighting, to be safe
    yields << "Samples used in PURW:" << std::endl;
    for (SamplePairVecIter iter = PURWSamples.begin(); iter != PURWSamples.end(); ++iter) {
      yields << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
    }
  }
  else {
    yields << "No PU reweighting applied!" << std::endl;
  }
  yields << "-------------------" << std::endl << std::endl;

  std::cout << "Finished PU reweighting, now begin Analysis" << std::endl;
  
  //========================================// 
  //      Produce Plots Per Sample          //
  //========================================// 
  
  yields << "Yields from individual Samples from Analysis" << std::endl;
  yields << "--------------------------------------------" << std::endl << std::endl;
  
  // run over data + MC samples (dibosons and the like) first, then Top samples, then QCD
  // -------------------------------------- //
  // Largest samples to process 
  SamplePairVec Samples;
  Samples.push_back(SamplePair("doublemu",false));
  Samples.push_back(SamplePair("zll",true));
  //  Samples.push_back(SamplePair("wln",true));
  
  for (SamplePairVecIter iter = Samples.begin(); iter != Samples.end(); ++iter) {
    std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
    Analysis sample((*iter),selection,njetsselection,puweights,lumi,nBins_vtx,colorMap,outdir,outtype);
    sample.DoAnalysis(yields);
  }

  std::cout << "Done with Data, Zll,Wln Analysis ... now do Diboson Analysis" << std::endl;  
  
  
  // -------------------------------------- //
  // diboson analysis
  SamplePairVec DBSamples;
  DBSamples.push_back(SamplePair("ww",true));
  DBSamples.push_back(SamplePair("zz",true));
  DBSamples.push_back(SamplePair("wz",true));

  for (SamplePairVecIter iter = DBSamples.begin(); iter != DBSamples.end(); ++iter) {
    std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
    Analysis sample((*iter),selection,njetsselection,puweights,lumi,nBins_vtx,colorMap,outdir,outtype);
    sample.DoAnalysis(yields);
  }
 
  std::cout << "Done with Diboson Analysis ... now Hadd Diboson" << std::endl;  
  Hadd(DBSamples,outdir,selection,njetsselection,"diboson");
  Samples.push_back(SamplePair("diboson",true)); // add hadded file to total samples for stacking
  std::cout << "Done with Diboson Hadd ... now do Top Analysis" << std::endl;  
  
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
    Analysis sample((*iter),selection,njetsselection,puweights,lumi,nBins_vtx,colorMap,outdir,outtype);
    sample.DoAnalysis(yields);
  }

  std::cout << "Done with Top Analysis ... now Hadd Top" << std::endl;  
  Hadd(TopSamples,outdir,selection,njetsselection,"top");
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
    Analysis sample((*iter),selection,njetsselection,puweights,lumi,nBins_vtx,colorMap,outdir,outtype);
    sample.DoAnalysis(yields);
  }

  std::cout << "Done with QCD Analysis ... now hadd QCD" << std::endl;  
  Hadd(QCDSamples,outdir,selection,njetsselection,"qcd");
  Samples.push_back(SamplePair("qcd",true)); // add hadded file to total samples for stacking 
  std::cout << "Done with QCD Hadd ... now do Gamma Analysis" << std::endl;  

  // -------------------------------------- //
  /*
  // Photon backgrounds
  SamplePairVec GammaSamples;
  GammaSamples.push_back(SamplePair("gamma100to200",true)); 
  GammaSamples.push_back(SamplePair("gamma200to400",true)); 
  GammaSamples.push_back(SamplePair("gamma400to600",true)); 
  GammaSamples.push_back(SamplePair("gamma600toinf",true)); 

  for (SamplePairVecIter iter = GammaSamples.begin(); iter != GammaSamples.end(); ++iter) {
    std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
    Analysis sample((*iter),selection,njetsselection,puweights,lumi,nBins_vtx,colorMap,outdir,outtype);
    sample.DoAnalysis(yields);
  }

  std::cout << "Done with Gamma Analysis ... now hadd Gamma" << std::endl;  
  Hadd(QCDSamples,outdir,selection,njetsselection,"gamma");
  Samples.push_back(SamplePair("gamma",true)); // add hadded file to total samples for stacking 
  std::cout << "Done with Gamma Hadd ... now make all stack plots" << std::endl;  
  */
  yields << "--------------------------------------------" << std::endl << std::endl;

  //========================================// 
  //        Stack Plots Production          //
  //========================================// 

  yields << "Total Yields taken from nvtx in StackPlots" << std::endl;
  yields << "------------------------------------------" << std::endl << std::endl;

  StackPlots * stacker = new StackPlots(Samples,selection,njetsselection,lumi,colorMap,outdir,outtype);
  stacker->DoStacks(yields);
  delete stacker;

  yields.close(); // close yields txt
}
