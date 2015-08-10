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
  TString outdir = "fullbatch"; // append with selection

  // Allow user to set outtype for plots
  const TString outtype = "png";

  // First make total output directory ... sub directories made inside objects
  MakeOutDirectory(outdir);

  //++++++++++++++++++++++++++++++++++++++++// 
  //                                        //
  //                 Analysis               //
  //                                        //
  //++++++++++++++++++++++++++++++++++++++++// 

  // where are we running? if local, set to true!  (sets where to get files)
  const Bool_t runLocal = false;

  // produce plots per sample?
  const Bool_t doAnalysis = false;

  // do PURW?
  Bool_t doReWeight = true; // false if no actual reweighting to be performed
  if (!doAnalysis) {
    doReWeight = false;
  }

  // do stacking?
  const Bool_t doStacks = true;
  
  // Total Integrated Luminosity
  const Double_t lumi = 0.04003; // int lumi in fb^-1

  // Selection we want for ANALYSIS (zmumu = zpeak with muons, zelel = zpeak with electrons, singlemu, singlephoton)
  const TString selection = "zelel";
  outdir.Append(Form("_%s",selection.Data()));

  // what samples to use?
  const Bool_t useData         = true;
  const Bool_t useSingleBoson  = true;
  const Bool_t useDoubleBosons = true;
  const Bool_t useTop          = true;
  const Bool_t useGamma        = false;
  const Bool_t useQCD          = true;

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

  DblVec puweights; // overall vector to be used for reweighting everywhere
  if (doReWeight) {
    const TString PURWselection = "zmumu";
    const Int_t   PURWnjetsselection = -1;

    std::cout << Form("Do PU reweighting first with %s selection, njets selection: %d!",PURWselection.Data(),PURWnjetsselection) << std::endl;

    SamplePairVec PURWSamples;
    if (PURWselection.Contains("zmumu",TString::kExact)) {
      PURWSamples.push_back(SamplePair("doublemu",false));
      PURWSamples.push_back(SamplePair("zll",true));

    }
    else if (PURWselection.Contains("zelel",TString::kExact)) {
      PURWSamples.push_back(SamplePair("doubleel",false));
      PURWSamples.push_back(SamplePair("zll",true));
    }    
    else if (PURWselection.Contains("singlemu",TString::kExact)) {
      PURWSamples.push_back(SamplePair("singlemu",false));
      PURWSamples.push_back(SamplePair("wln",true));
    }    
    else if (PURWselection.Contains("singlephoton",TString::kExact)) {
      PURWSamples.push_back(SamplePair("singlephoton",false));
      PURWSamples.push_back(SamplePair("gamma100to200",true)); 
      PURWSamples.push_back(SamplePair("gamma200to400",true)); 
      PURWSamples.push_back(SamplePair("gamma400to600",true)); 
      PURWSamples.push_back(SamplePair("gamma600toinf",true)); 
    }    
    else {
      std::cout << "Not a known selection: " << PURWselection.Data() <<  " ...exiting..." << std::endl;
      exit(1);
    }

    // do the reweighting here
    PUReweight * reweight = new PUReweight(PURWSamples,PURWselection,PURWnjetsselection,lumi,nBins_vtx,outdir,outtype,runLocal);
    puweights = reweight->GetPUWeights(); 
  
    delete reweight;

    // some printouts
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

  SamplePairVec Samples; // this vector is one that will also be used for stack plots

  // -------------------------------------- //
  // Data analysis
  if (useData) {

    // selection defines data!
    SamplePairVec DataSamples;
    if (selection.Contains("zmumu",TString::kExact)) { 
      DataSamples.push_back(SamplePair("doublemu",false));
    }
    else if (selection.Contains("zelel",TString::kExact)) { 
      DataSamples.push_back(SamplePair("doubleel",false));
    }
    else if (selection.Contains("singlemu",TString::kExact)) { 
      DataSamples.push_back(SamplePair("singlemu",false));
    }
    else if (selection.Contains("singlephoton",TString::kExact)) { 
      DataSamples.push_back(SamplePair("singlephoton",false));
    }      
    else {
      std::cout << "Not a known selection for data: " << selection.Data() <<  " ...exiting..." << std::endl;
      exit(1);
    }

    if (doAnalysis) { // this block here to prevent doing analysis twice when just need sample in stack plot vector
      std::cout << "Starting data Analysis" << std::endl;
      for (SamplePairVecIter iter = DataSamples.begin(); iter != DataSamples.end(); ++iter) {
	std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
	Analysis sample((*iter),selection,njetsselection,puweights,lumi,nBins_vtx,outdir,colorMap,outtype,runLocal);
	sample.DoAnalysis(yields);
      }
      std::cout << "Done with data Analysis" << std::endl;  
    }
    
    // now just add the sample name pair to overall vector // --> could just hadd here... maybe later
    for (SamplePairVecIter iter = DataSamples.begin(); iter != DataSamples.end(); ++iter) {
      std::cout << "Adding data sample to overall samples: " << (*iter).first.Data() << std::endl;
      Samples.push_back(*iter);
    }
  }
  
  // -------------------------------------- //
  // Single Boson analysis
  if (useSingleBoson) {
    
    SamplePairVec SBLSamples; 
    SBLSamples.push_back(SamplePair("wln",true));
    SBLSamples.push_back(SamplePair("zll",true));    

    if (doAnalysis) {
      std::cout << "Starting single boson to leptons MC Analysis" << std::endl;
      for (SamplePairVecIter iter = SBLSamples.begin(); iter != SBLSamples.end(); ++iter) {
	std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
	Analysis sample((*iter),selection,njetsselection,puweights,lumi,nBins_vtx,outdir,colorMap,outtype,runLocal);
	sample.DoAnalysis(yields);
      }
      std::cout << "Done with Z -> ll, W -> lnu Analysis" << std::endl;  
    }

    // Since no hadd, just add the same sample names to the overall sample vector
    for (SamplePairVecIter iter = SBLSamples.begin(); iter != SBLSamples.end(); ++iter) {
      std::cout << "Adding single boson to leptons sample to overall samples: " << (*iter).first.Data() << std::endl;
      Samples.push_back(*iter);
    }
  }

  // -------------------------------------- //
  // diboson analysis
  if (useDoubleBosons) {

    SamplePairVec DBSamples; // outside in the strange case you want to stack individually... and symmetry with above
    DBSamples.push_back(SamplePair("ww",true));
    DBSamples.push_back(SamplePair("zz",true));
    DBSamples.push_back(SamplePair("wz",true));
   
    if (doAnalysis) {
      std::cout << "Starting diboson Analysis" << std::endl;
      for (SamplePairVecIter iter = DBSamples.begin(); iter != DBSamples.end(); ++iter) {
	std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
	Analysis sample((*iter),selection,njetsselection,puweights,lumi,nBins_vtx,outdir,colorMap,outtype,runLocal);
	sample.DoAnalysis(yields);
      }
      std::cout << "Done with Diboson Analysis ... now Hadd Diboson" << std::endl;  

      Hadd(DBSamples,outdir,selection,njetsselection,"diboson");
      std::cout << "Done with Diboson Hadd" << std::endl;  
    }

    // add only hadded file to total samples for stacking
    std::cout << "Adding hadded diboson sample to overall samples" << std::endl;
    Samples.push_back(SamplePair("diboson",true));
  }

  // -------------------------------------- //
  // top backgrounds
  if (useTop) {
        
    SamplePairVec TopSamples; // outside in the strange case you want to stack individually... and symmetry with above
    TopSamples.push_back(SamplePair("ttbar",true)); 
    TopSamples.push_back(SamplePair("singlett",true)); 
    TopSamples.push_back(SamplePair("singletbart",true)); 
    TopSamples.push_back(SamplePair("singletw",true)); 
    TopSamples.push_back(SamplePair("singletbarw",true)); 
    
    if (doAnalysis) {
      std::cout << "Starting top Analysis" << std::endl;
      for (SamplePairVecIter iter = TopSamples.begin(); iter != TopSamples.end(); ++iter) {
	std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
	Analysis sample((*iter),selection,njetsselection,puweights,lumi,nBins_vtx,outdir,colorMap,outtype,runLocal);
	sample.DoAnalysis(yields);
      }
      std::cout << "Done with Top Analysis ... now Hadd Top" << std::endl;  
      
      Hadd(TopSamples,outdir,selection,njetsselection,"top");
      std::cout << "Done with Top Hadd" << std::endl;  
    }
    
    // add hadded file to total samples for stacking
    std::cout << "Adding hadded top sample to overall samples" << std::endl;
    Samples.push_back(SamplePair("top",true)); 
  }

  // -------------------------------------- //
  // Photon backgrounds
  if (useGamma) {

    SamplePairVec GammaSamples;  // outside in the strange case you want to stack individually... and symmetry with above
    GammaSamples.push_back(SamplePair("gamma100to200",true)); 
    GammaSamples.push_back(SamplePair("gamma200to400",true)); 
    GammaSamples.push_back(SamplePair("gamma400to600",true)); 
    GammaSamples.push_back(SamplePair("gamma600toinf",true)); 
  
    if (doAnalysis) {
      std::cout << "Starting photon Analysis" << std::endl;
      for (SamplePairVecIter iter = GammaSamples.begin(); iter != GammaSamples.end(); ++iter) {
	std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
	Analysis sample((*iter),selection,njetsselection,puweights,lumi,nBins_vtx,outdir,colorMap,outtype,runLocal);
	sample.DoAnalysis(yields);
      }
      std::cout << "Done with Gamma Analysis ... now hadd Gamma" << std::endl;  

      Hadd(GammaSamples,outdir,selection,njetsselection,"gamma");
      std::cout << "Done with Gamma Hadd" << std::endl;      
    }

    //add hadded file to total samples for stacking   
    std::cout << "Adding hadded photon sample to overall samples" << std::endl;
    Samples.push_back(SamplePair("gamma",true));
  }

  // -------------------------------------- //
  // QCD backgrounds
  if (useQCD) {
  
    SamplePairVec QCDSamples;  // outside in the strange case you want to stack individually... and symmetry with above
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
    
    if (doAnalysis) {
      std::cout << "Starting QCD Analysis" << std::endl;
      for (SamplePairVecIter iter = QCDSamples.begin(); iter != QCDSamples.end(); ++iter) {
	std::cout << "Analyzing Sample: " << (*iter).first.Data() << " isMC: " << (*iter).second << std::endl;
	Analysis sample((*iter),selection,njetsselection,puweights,lumi,nBins_vtx,outdir,colorMap,outtype,runLocal);
	sample.DoAnalysis(yields);
      }
      std::cout << "Done with QCD Analysis ... now hadd QCD" << std::endl;  
      
      Hadd(QCDSamples,outdir,selection,njetsselection,"qcd");
      std::cout << "Done with QCD Hadd" << std::endl;  
    }
    
    // add hadded file to total samples for stacking 
    std::cout << "Adding hadded QCD sample to overall samples" << std::endl;
    Samples.push_back(SamplePair("qcd",true)); 
  }

  yields << "--------------------------------------------" << std::endl << std::endl;

  //========================================// 
  //        Stack Plots Production          //
  //========================================// 

  if (doStacks) {  
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
	//	if (tmp_mcyields[mc].second > 0.05) { // skip backgrounds that contribute less than a 20th of an event
	MCSamples.push_back(SamplePair(tmp_mcyields[mc].first,"true"));
	  //	}
      }
  
      // check to make sure we can proceed!  if we filtered out all mc, no stacks!
      UInt_t tmp_nfilteredmc = MCSamples.size();
      if (tmp_nfilteredmc != 0) {

	// clear original samples vector and push back data and mc samples
	Samples.clear();
	for (UInt_t data = 0; data < tmp_ndata; data++ ) {
	  Samples.push_back(DataSamples[data]);
	}
	
	
	// check first that we can still proceed!
	for (UInt_t mc = 0; mc < tmp_nfilteredmc; mc++ ) {
	  Samples.push_back(MCSamples[mc]);
	}
	
	yields << "Total Yields taken from nvtx in StackPlots" << std::endl;
	yields << "------------------------------------------" << std::endl << std::endl;
	
	std::cout << "Make the stacks!" << std::endl;
	
	StackPlots * stacker = new StackPlots(Samples,selection,njetsselection,lumi,outdir,colorMap,outtype);
	stacker->DoStacks(yields);
	delete stacker;
      }
      else { // filtered too many MC, exit!
	std::cout << "No MC to use to make stacks after cutting out MC with yield less than 0.05 ...exiting..." << std::endl;
	exit(1);
      }
    }
    else { // didn't pass for first check for making stackings
      std::cout << "Not enough samples to make stacks! Data count: " << tmp_ndata << " MC count: " << tmp_nmc << " ...exiting..." << std::endl;
      exit(1);
    }
  }
  else {
    std::cout << "Skip making the stacks and finish as doStacks: " << doStacks << std::endl;
  }
  yields.close(); // close yields txt
}

