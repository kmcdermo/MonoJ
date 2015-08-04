#include "../interface/Common.hh"

#include "../interface/Analysis.hh"
#include "../interface/StackPlots.hh"
#include "../interface/PUReweight.hh"

#include "TROOT.h"

#include <utility>
#include <vector>
#include <algorithm>

#include <iostream>
#include <fstream>

typedef std::pair<TString,Double_t>  SampleYieldPair;
typedef std::vector<SampleYieldPair> SampleYieldPairVec;

static bool sortByYield(const SampleYieldPair& mcpair1, const SampleYieldPair& mcpair2) {
  return mcpair1.second<=mcpair2.second;
}

int main(){
  // set TDR Style
  gROOT->ProcessLine(".x ~/.rootlogon.C"); // hack way to set root logon macro ... could put this into Typedef.hh 
  
  // Variables needed in all functions for plotting and the like so it is universal
  // Color for MC Stacks
  ColorMap colorMap;
  colorMap["zll"]     = kCyan;
  colorMap["wln"]     = kRed;
  colorMap["diboson"] = kBlue;
  colorMap["top"]     = kMagenta;
  colorMap["qcd"]     = kYellow;
  colorMap["gamma"]   = kGreen;

  // nPV needs nBins set for PU reweights and actual plots
  const Int_t nBins_vtx = 50;

  // Allow user to set output directory for whole project--> if running only stacking... will need to specify inputs in .cc file
  const TString outdir = "fullbatch_doublemu";

  // Allow user to set outtype for plots
  const TString outtype = "png";

  // First make total output directory ... sub directories made inside objects
  MakeOutDirectory(outdir);

  //++++++++++++++++++++++++++++++++++++++++// 
  //                                        //
  //                 Analysis               //
  //                                        //
  //++++++++++++++++++++++++++++++++++++++++// 

  // Important variables to set for analysis

  // Total Integrated Luminosity
  const Double_t lumi = 0.04003; // int lumi in fb^-1

  // Selection we want (zmumu = zpeak with muons, zelel = zpeak with electrons, singlemu, singlephoton)
  const TString selection = "zmumu";

  // Njets selection (==1, ==2) ... -1 = no selection
  const Int_t njetsselection = 2;

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

  // add info about reweighting technique to yields
  yields << "PU Reweighting Info" << std::endl;
  yields << "-------------------" << std::endl << std::endl;

  const Bool_t doReWeight = true; // false if no actual reweighting to be performed
  DblVec puweights;
  if (doReWeight) {
    const TString PURWselection = "zmumu";
    const Int_t   PURWnjetsselection = -1;
    std::cout << Form("Do PU reweighting first with %s selection, njets selection: %d!",PURWselection.Data(),PURWnjetsselection) << std::endl;
    
    SamplePairVec PURWSamples;
    PURWSamples.push_back(SamplePair("doublemu",false));
    PURWSamples.push_back(SamplePair("zll",true));
    
    PUReweight * reweight = new PUReweight(PURWSamples,PURWselection,PURWnjetsselection,lumi,nBins_vtx,outdir,outtype);
    puweights = reweight->GetPUWeights(); 
  
    delete reweight;

    yields << "Used PU Reweighting with selection: " << PURWselection.Data() << " njets cut: " << ((PURWnjetsselection != -1) ? Form("%i",PURWnjetsselection) : "NONE") << std::endl;
    // include samples used for reweighting, to be safe
    yields << "Samples used in PURW:" << std::endl;
    for (SamplePairVecIter iter = PURWSamples.begin(); iter != PURWSamples.end(); ++iter) {
      yields << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
    }
  } // end if for doReWeight

  else { // no reweight!  so just push back puweights  = 1.0 for all nvtx bins
    for (Int_t i = 1; i <= nBins_vtx; i++ ){ // could do i = 0; i < nBins_vtx ... just do this to match bin indexing in ROOT and look like function inside PUReweight.cc
      puweights.push_back(1.0);
    }
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
  std::cout << Form("Analysis %s selection, njets selection: %d!, with luminosity: %2.2f pb^-1",selection.Data(),njetsselection,lumi*1000) << std::endl;
  
  // -------------------------------------- //
  // Z/W peak analysis
  std::cout << "Starting data, Zll, Wln Analysis" << std::endl;

  SamplePairVec Samples; // this vector is one that will also be used for stack plots
  Samples.push_back(SamplePair("doublemu",false));
  //  Samples.push_back(SamplePair("doubleel",false));
  //  Samples.push_back(SamplePair("singlemu",false));
  //  Samples.push_back(SamplePair("singlephoton",false));
  Samples.push_back(SamplePair("zll",true));
  Samples.push_back(SamplePair("wln",true));
  
  for (SamplePairVecIter iter = Samples.begin(); iter != Samples.end(); ++iter) {
    std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
    Analysis sample((*iter),selection,njetsselection,puweights,lumi,nBins_vtx,outdir,colorMap,outtype);
    sample.DoAnalysis(yields);
  } 
  std::cout << "Done with Data, Zll, Wln Analysis" << std::endl;  
  
  // -------------------------------------- //
  // diboson analysis
  std::cout << "Starting diboson Analysis" << std::endl;

  SamplePairVec DBSamples;
  DBSamples.push_back(SamplePair("ww",true));
  DBSamples.push_back(SamplePair("zz",true));
  DBSamples.push_back(SamplePair("wz",true));

  for (SamplePairVecIter iter = DBSamples.begin(); iter != DBSamples.end(); ++iter) {
  std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
  Analysis sample((*iter),selection,njetsselection,puweights,lumi,nBins_vtx,outdir,colorMap,outtype);
  sample.DoAnalysis(yields);
  }
  std::cout << "Done with Diboson Analysis ... now Hadd Diboson" << std::endl;  
  
  Hadd(DBSamples,outdir,selection,njetsselection,"diboson");

  Samples.push_back(SamplePair("diboson",true)); // add hadded file to total samples for stacking
  std::cout << "Done with Diboson Hadd" << std::endl;  

  // -------------------------------------- //
  // top backgrounds
  std::cout << "Starting top Analysis" << std::endl;

  SamplePairVec TopSamples;
  TopSamples.push_back(SamplePair("ttbar",true)); 
  TopSamples.push_back(SamplePair("singlett",true)); 
  TopSamples.push_back(SamplePair("singletbart",true)); 
  TopSamples.push_back(SamplePair("singletw",true)); 
  TopSamples.push_back(SamplePair("singletbarw",true)); 
  
  for (SamplePairVecIter iter = TopSamples.begin(); iter != TopSamples.end(); ++iter) {
  std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
  Analysis sample((*iter),selection,njetsselection,puweights,lumi,nBins_vtx,outdir,colorMap,outtype);
  sample.DoAnalysis(yields);
  }
  std::cout << "Done with Top Analysis ... now Hadd Top" << std::endl;  

  Hadd(TopSamples,outdir,selection,njetsselection,"top");

  Samples.push_back(SamplePair("top",true)); // add hadded file to total samples for stacking
  std::cout << "Done with Top Hadd" << std::endl;  

  // -------------------------------------- //
  // Photon backgrounds
  /*
  std::cout << "Starting photon Analysis" << std::endl;

  SamplePairVec GammaSamples;
  GammaSamples.push_back(SamplePair("gamma100to200",true)); 
  GammaSamples.push_back(SamplePair("gamma200to400",true)); 
  GammaSamples.push_back(SamplePair("gamma400to600",true)); 
  GammaSamples.push_back(SamplePair("gamma600toinf",true)); 

  for (SamplePairVecIter iter = GammaSamples.begin(); iter != GammaSamples.end(); ++iter) {
    std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
    Analysis sample((*iter),selection,njetsselection,puweights,lumi,nBins_vtx,outdir,colorMap,outtype);
    sample.DoAnalysis(yields);
  }

  std::cout << "Done with Gamma Analysis ... now hadd Gamma" << std::endl;  

  Hadd(GammaSamples,outdir,selection,njetsselection,"gamma");
  
  Samples.push_back(SamplePair("gamma",true));  //add hadded file to total samples for stacking   
  std::cout << "Done with Gamma Hadd" << std::endl;  
  */
  // -------------------------------------- //
  // QCD backgrounds

  std::cout << "Starting QCD Analysis" << std::endl;
  
  SamplePairVec QCDSamples;
  QCDSamples.push_back(SamplePair("qcd15to30",true)); 
  QCDSamples.push_back(SamplePair("qcd30to50",true)); 
  QCDSamples.push_back(SamplePair("qcd50to80",true)); 
  QCDSamples.push_back(SamplePair("qcd80to120",true)); 
  QCDSamples.push_back(SamplePair("qcd120to170",true)); 
  QCDSamples.push_back(SamplePair("qcd170to300",true)); 
  QCDSamples.push_back(SamplePair("qcd300to470",true)); 
  QCDSamples.push_back(SamplePair("qcd470to600",true)); 
  QCDSamples.push_back(SamplePair("qcd600to800",true));
  QCDSamples.push_back(SamplePair("qcd800to1000",true));
  QCDSamples.push_back(SamplePair("qcd1000to1400",true)); 
  QCDSamples.push_back(SamplePair("qcd1400to1800",true));
  QCDSamples.push_back(SamplePair("qcd1800to2400",true));
  QCDSamples.push_back(SamplePair("qcd2400to3200",true));
  QCDSamples.push_back(SamplePair("qcd3200toinf",true)); 

  for (SamplePairVecIter iter = QCDSamples.begin(); iter != QCDSamples.end(); ++iter) {
    std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
    Analysis sample((*iter),selection,njetsselection,puweights,lumi,nBins_vtx,outdir,colorMap,outtype);
    sample.DoAnalysis(yields);
  }

  std::cout << "Done with QCD Analysis ... now hadd QCD" << std::endl;  

  Hadd(QCDSamples,outdir,selection,njetsselection,"qcd");
  
  Samples.push_back(SamplePair("qcd",true)); // add hadded file to total samples for stacking 
  std::cout << "Done with QCD Hadd" << std::endl;

  yields << "--------------------------------------------" << std::endl << std::endl;

  //========================================// 
  //        Stack Plots Production          //
  //========================================// 
  
  UInt_t tmp_nmc   = 0;  
  UInt_t tmp_ndata = 0;  

  std::cout << "Performing checks on mc and data samples" << std::endl;

  for (SamplePairVecIter iter = Samples.begin(); iter != Samples.end(); ++iter) {
    if ((*iter).second) {tmp_nmc++;}
    else {tmp_ndata++;}
  }

  if ((tmp_ndata != 0) && (tmp_nmc != 0)) { // check to make sure enough samples can make stacks
    const UInt_t tmp_nsamples = tmp_nmc+tmp_ndata;

    std::cout << "Sorting MC Samples by yields" << std::endl;

    TString nJetsStr = "";
    if (njetsselection != -1){
      nJetsStr = Form("_nj%i",njetsselection);
    }

    SamplePairVec MCSamples;
    SamplePairVec DataSamples;
    for (UInt_t isample = 0; isample < tmp_nsamples; isample++) {
      if (Samples[isample].second) {
	MCSamples.push_back(Samples[isample]);
      }
      else {
	DataSamples.push_back(Samples[isample]);
      }
    }

    // push back mc yields in tmp vector
    SampleYieldPairVec tmp_mcyields;
    for (UInt_t mc = 0; mc < tmp_nmc; mc++) {

      // open mc file first
      const TString tmp_mcfilename = Form("%s/%s%s/%s_MC/plots.root",outdir.Data(),selection.Data(),nJetsStr.Data(),MCSamples[mc].first.Data());
      TFile * tmp_mcfile = TFile::Open(tmp_mcfilename.Data());
      CheckValidFile(tmp_mcfile,tmp_mcfilename);

      // open nvtx plot
      TH1D * tmp_nvtx = (TH1D*)tmp_mcfile->Get("nvtx");
      CheckValidTH1D(tmp_nvtx,"nvtx",tmp_mcfilename);
    
      // get yield and push back with corresponding sample name
      tmp_mcyields.push_back(SampleYieldPair(MCSamples[mc].first,tmp_nvtx->Integral()));
      
      delete tmp_nvtx;
      delete tmp_mcfile;
    }

    // sort temp yields vector
    std::sort(tmp_mcyields.begin(),tmp_mcyields.end(),sortByYield);
    std::cout << "Finished sorting MC, now put samples in right order to be processed" << std::endl;

    // clear MCSamples, and push back in order of tmp_yields now sorted
    MCSamples.clear();
    for (UInt_t mc = 0; mc < tmp_nmc; mc++) { // init mc double hists
      if (tmp_mcyields[mc].second > 0.05) { // skip backgrounds that contribute less than a 20th of an event
	MCSamples.push_back(SamplePair(tmp_mcyields[mc].first,"true"));
      }
    }
  
    // clear original samples vector and push back data and mc samples
    Samples.clear();
    for (UInt_t data = 0; data < tmp_ndata; data++ ) {
      Samples.push_back(DataSamples[data]);
    }
    for (UInt_t mc = 0; mc < MCSamples.size(); mc++ ) {
      Samples.push_back(MCSamples[mc]);
    }

    yields << "Total Yields taken from nvtx in StackPlots" << std::endl;
    yields << "------------------------------------------" << std::endl << std::endl;

    std::cout << "Make the stacks!" << std::endl;

    StackPlots * stacker = new StackPlots(Samples,selection,njetsselection,lumi,outdir,colorMap,outtype);
    stacker->DoStacks(yields);
    delete stacker;
  }
  
  yields.close(); // close yields txt
}

