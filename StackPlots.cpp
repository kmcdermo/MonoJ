#include "StackPlots.hh"
#include "Common.hh"

StackPlots::StackPlots(SamplePairVec Samples, const TString outname, const TString outtype){
  // input data members
  for (SamplePairVecIter iter = Samples.begin(); iter != Samples.end(); ++iter) {
    if ((*iter).second) { // isMC == true
      fMCNames.push_back((*iter).first);
    }
    else { // data
      fDataNames.push_back((*iter).first);
    }
  }

  // use the plots already stored in Analysis.cpp --> doubles
  fDHistNames.push_back("zmass");
  fDHistNames.push_back("zpt");
  fDHistNames.push_back("pfmet");
  fDHistNames.push_back("signaljetpt");
  fDHistNames.push_back("signaljeteta");
  fDHistNames.push_back("signaljetCHfrac");
  fDHistNames.push_back("signaljetNHfrac");
  fDHistNames.push_back("signaljetEMfrac");
  fDHistNames.push_back("signaljetCEMfrac");

  // use plots stored in Analysis.cpp --> ints
  fIHistNames.push_back("njets");
  fIHistNames.push_back("nvtx");

  // store these too
  fNData   = fDataNames.size();
  fNMC     = fMCNames.size();
  fNDHists = fDHistNames.size();
  fNIHists = fIHistNames.size();

  StackPlots::OpenInputFiles();
  StackPlots::InitInputPlots();
  StackPlots::InitOutputPlots();
  StackPlots::InitOutputLegends();
  StackPlots::InitRatioPlots();
  StackPlots::InitOutputCanvPads();

  // output data members
  fOutName = outname; // where to put output plots 
  MakeOutDirectory(fOutName); // make output directory --> same name as outname
  fOutFile = new TFile(Form("%s/%s_canvs.root",fOutName.Data(),fOutName.Data()),"RECREATE"); // make output tfile --> store canvas images here too, for quick editting
  fOutType = outtype; // allow user to pick png, pdf, gif, etc for stacked plots

  // define color map + title map
  fSampleTitleMap["zmumu"] = "Z #rightarrow #mu^{+} #mu^{-}";
  fSampleTitleMap["ttbar"] = "t#bar{t}";
}

StackPlots::~StackPlots(){
}

void StackPlots::OpenInputFiles() {
  // open input files into TFileVec --> data 
  fDataFiles.resize(fNData);
  for (UInt_t dt = 0; dt < fNData; dt++) {
    TString datafile = Form("%s_data/%s_data_plots.root",fDataNames[dt].Data(),fDataNames[dt].Data());
    fDataFiles[dt] = TFile::Open(datafile.Data());
    StackPlots::CheckValidFile(fDataFiles[dt],datafile);
  }

  // open input files into TFileVec --> mc 
  fMCFiles.resize(fNMC);
  for (UInt_t mc = 0; mc < fNMC; mc++) {
    TString mcfile = Form("%s_MC/%s_MC_plots.root",fMCNames[mc].Data(),fMCNames[mc].Data());
    fMCFiles[mc] = TFile::Open(mcfile.Data());
    StackPlots::CheckValidFile(fMCFiles[mc],mcfile);
  }
}

void StackPlots::CheckValidFile(TFile *& file, const TString fname){
  if (file == (TFile*) NULL) { // check if valid file
    std::cout << "Input file is bad pointer: " << fname.Data() << " ...exiting..." << std::endl;
    exit(1);
  }
  else {
    std::cout << "Successfully opened file: " << fname.Data() << std::endl;
  }
}

void StackPlots::InitInputPlots() {
  // init input double hists
  fInDataDHists.resize(fNDHists);
  fInMCDHists.resize(fNDHists);
  for (UInt_t dh = 0; dh < fNDHists; dh++){ // loop over double hists
    // data first
    fInDataDHists[dh].resize(fNData); 
    for (UInt_t dt = 0; dt < fNData; dt++) { // init data double hists
      fInDataDHists[dh][dt] = (TH1D*)fDataFiles[dt]->Get(Form("%s",fDHistNames[dh].Data()));
      StackPlots::CheckValidTH1D(fInDataDHists[dh][dt],fDHistNames[dh],fDataFiles[dt]->GetName());
    }

    // mc second
    fInMCDHists[dh].resize(fNMC); 
    for (UInt_t mc = 0; mc < fNMC; mc++) { // init mc double hists
      fInMCDHists[dh][mc] = (TH1D*)fMCFiles[mc]->Get(Form("%s",fDHistNames[dh].Data()));
      StackPlots::CheckValidTH1D(fInMCDHists[dh][mc],fDHistNames[dh],fMCFiles[mc]->GetName());
    }
  }

  // init input int hists
  fInDataIHists.resize(fNIHists);
  fInMCIHists.resize(fNIHists);
  for (UInt_t ih = 0; ih < fNIHists; ih++){ // loop over int hists
    // data first
    fInDataIHists[ih].resize(fNData); 
    for (UInt_t dt = 0; dt < fNData; dt++) { // init data int hists
      fInDataIHists[ih][dt] = (TH1I*)fDataFiles[dt]->Get(Form("%s",fIHistNames[ih].Data()));
      StackPlots::CheckValidTH1I(fInDataIHists[ih][dt],fDHistNames[ih],fDataFiles[dt]->GetName());
    }

    // mc second
    fInMCIHists[ih].resize(fNMC); 
    for (UInt_t mc = 0; mc < fNMC; mc++) { // init mc int hists
      fInMCIHists[ih][mc] = (TH1I*)fMCFiles[mc]->Get(Form("%s",fIHistNames[ih].Data()));
      StackPlots::CheckValidTH1I(fInMCIHists[ih][mc],fIHistNames[ih],fMCFiles[mc]->GetName());
    }
  }
}

void StackPlots::CheckValidTH1D(TH1D*& plot, const TString pname, const TString fname){
  if (plot == (TH1D*) NULL) { // check if valid plot
    std::cout << "Input TH1D is bad pointer: " << pname.Data() << " in input file: " << fname.Data() << " ...exiting..." << std::endl;
    exit(1);
  }
  else {
    std::cout << "Successfully initialized plot: " << pname.Data() << " in input file: " << fname.Data() << std::endl;
  }
}

void StackPlots::CheckValidTH1I(TH1I*& plot, const TString pname, const TString fname){
  if (plot == (TH1I*) NULL) { // check if valid plot
    std::cout << "Input TH1I is bad pointer: " << pname.Data() << " in input file: " << fname.Data() << " ...exiting..." << std::endl;
    exit(1);
  }
  else {
    std::cout << "Successfully initialized plot: " << pname.Data() << " in input file: " << fname.Data() << std::endl;
  }
}

void StackPlots::InitOutputPlots() {
  // init output hists and sumw2 for Data + MC -- init output stacks for MC

  // double hists
  fOutDataDHists.resize(fNDHists); // make enough space for data double hists
  fOutMCDHists.resize(fNDHists); // make enough space for MC double hists
  fOutMCDStacks.resize(fNDHists); // same with stack 
  for (UInt_t dh = 0; dh < fNDHists; dh++){
    fOutMCDStacks[dh] = new THStack("","");
  }
  
  // int hists
  fOutDataIHists.resize(fNIHists); // make enough space for data int hists
  fOutMCIHists.resize(fNIHists); // make enough space for MC int hists
  fOutMCIStacks.resize(fNIHists); // same with stack 
  for (UInt_t ih = 0; ih < fNIHists; ih++){
    fOutMCIStacks[ih] = new THStack("","");
  }
}

void StackPlots::InitOutputLegends() {
  // init legends

  // double
  fDLegend.resize(fNDHists);
  for (UInt_t dh = 0; dh < fNDHists; dh++){
    fDLegend[dh] = new TLegend(0.75,0.75,0.89,0.89);
  }

  // int
  fILegend.resize(fNIHists);
  for (UInt_t ih = 0; ih < fNIHists; ih++){
    fILegend[ih] = new TLegend(0.8,0.8,0.97,0.97);
  }
}

void StackPlots::InitRatioPlots() {
  // init ratios 

  // double
  fOutRatioDHists.resize(fNDHists);

  // int
  fOutRatioIHists.resize(fNIHists);
}

void StackPlots::InitOutputCanvPads() {
  // init canvases + pads

  // double
  fOutDCanvas.resize(fNDHists);
  fOutDStackPad.resize(fNDHists);
  fOutDRatioPad.resize(fNDHists);
  for (UInt_t dh = 0; dh < fNDHists; dh++){
    fOutDCanvas[dh] = new TCanvas();
    fOutDCanvas[dh]->cd();
    
    fOutDStackPad[dh] = new TPad("", "", 0, 0.3, 1.0, 1.0);
    fOutDStackPad[dh]->SetBottomMargin(0); // Upper and lower plot are joined
    
    fOutDRatioPad[dh] = new TPad("", "", 0, 0.05, 1.0, 0.3);
    fOutDRatioPad[dh]->SetTopMargin(0);
    fOutDRatioPad[dh]->SetBottomMargin(0.2);
  }

  // int
  fOutICanvas.resize(fNIHists);
  fOutIStackPad.resize(fNIHists);
  fOutIRatioPad.resize(fNIHists);
  for (UInt_t ih = 0; ih < fNIHists; ih++){
    fOutICanvas[ih] = new TCanvas();
    fOutICanvas[ih]->cd();

    fOutIStackPad[ih] = new TPad("", "", 0, 0.3, 1, 1.0);
    fOutIStackPad[ih]->SetBottomMargin(0); // Upper and lower plot are joined

    fOutIRatioPad[ih] = new TPad("", "", 0, 0.05, 1, 0.3);
    fOutIRatioPad[ih]->SetTopMargin(0);
    fOutIRatioPad[ih]->SetBottomMargin(0.2);
  }
}

void StackPlots::DoStacks() {
  StackPlots::MakeStackPlots();
  StackPlots::MakeRatioPlots();
  StackPlots::MakeOutputCanvas();
}

void StackPlots::MakeStackPlots(){
  // copy double plots into output hists/stacks
  for (UInt_t dh = 0; dh < fNDHists; dh++){ // double hists
    // data, copy + add only
    for (UInt_t dt = 0; dt < fNData; dt++) {
      if (dt == 0){
	fOutDataDHists[dh] = (TH1D*)fInDataDHists[dh][dt]->Clone();
      }
      else{
	fOutDataDHists[dh]->Add(fInDataDHists[dh][dt]);
      }
    } // end loop over data samples
    fDLegend[dh]->AddEntry(fOutDataDHists[dh],"Data","pl"); // add data entry to legend

    // mc, copy + add to hists, add to tstack
    for (UInt_t mc = 0; mc < fNMC; mc++) {
      if (mc == 0){ // add first for ratio
	fOutMCDHists[dh] = (TH1D*)fInMCDHists[dh][mc]->Clone();
      }
      else{ // add first for ratio
	fOutMCDHists[dh]->Add(fInMCDHists[dh][mc]);
      }
      //  just add input to stacks
      fOutMCDStacks[dh]->Add(fInMCDHists[dh][mc]);
      fDLegend[dh]->AddEntry(fInMCDHists[dh][mc],fSampleTitleMap[fMCNames[mc]],"lf");
    } // end loop over mc samples
  } // end loop over double plots

  // copy int plots into output hists/stacks
  for (UInt_t ih = 0; ih < fNIHists; ih++){ // int hists
    // data, copy + add only
    for (UInt_t dt = 0; dt < fNData; dt++) {
      if (dt == 0){
	fOutDataIHists[ih] = (TH1I*)fInDataIHists[ih][dt]->Clone();
      }
      else{
	fOutDataIHists[ih]->Add(fInDataIHists[ih][dt]);
      }
    } // end loop over data samples
    fILegend[ih]->AddEntry(fOutDataIHists[ih],"Data","l"); // add data entry to legend

    // mc, copy + add to hists, add to tstack
    for (UInt_t mc = 0; mc < fNMC; mc++) {
      if (mc == 0){ // add first for ratio
	fOutMCIHists[ih] = (TH1I*)fInMCIHists[ih][mc]->Clone();
      }
      else{ // add first for ratio
	fOutMCIHists[ih]->Add(fInMCIHists[ih][mc]);
      }
      //  just add input to stacks
      fOutMCIStacks[ih]->Add(fInMCIHists[ih][mc]);
      fILegend[ih]->AddEntry(fInMCIHists[ih][mc],fSampleTitleMap[fMCNames[mc]],"lf");
    } // end loop over mc samples
  } // end loop over int plots
}

void StackPlots::MakeRatioPlots() {
  // Define the ratio plot
  
  // double
  for (UInt_t dh = 0; dh < fNDHists; dh++){ // double hists
    fOutRatioDHists[dh] = (TH1D*)fOutDataDHists[dh]->Clone();
    fOutRatioDHists[dh]->Divide(fOutMCDHists[dh]);  
    fOutRatioDHists[dh]->SetLineColor(kBlack);
    fOutRatioDHists[dh]->SetMinimum(0.0);  // Define Y ..
    fOutRatioDHists[dh]->SetMaximum(2.0); // .. range
    fOutRatioDHists[dh]->SetStats(0);      // No statistics on lower plot
    fOutRatioDHists[dh]->GetYaxis()->SetTitle("Data/MC");
  }

  // int
  for (UInt_t ih = 0; ih < fNIHists; ih++){ // int hists
    fOutRatioIHists[ih] = (TH1I*)fOutDataIHists[ih]->Clone();
    fOutRatioIHists[ih]->Divide(fOutMCIHists[ih]);  
    fOutRatioIHists[ih]->SetLineColor(kBlack);
    fOutRatioIHists[ih]->SetMinimum(0.0);  // Define Y ..
    fOutRatioIHists[ih]->SetMaximum(2.0); // .. range
    fOutRatioIHists[ih]->SetStats(0);      // No statistics on lower plot
    fOutRatioIHists[ih]->GetYaxis()->SetTitle("Data/MC");
  }
}

void StackPlots::MakeOutputCanvas() {
  // make output canvases
  
  // double
  for (UInt_t dh = 0; dh < fNDHists; dh++){ // double hists
    //upper plot is stack
    fOutDCanvas[dh]->cd();
    fOutDStackPad[dh]->Draw(); // draw upper pad   
    fOutDStackPad[dh]->cd(); // upper pad is current pad
    fOutDataDHists[dh]->SetStats(0); // No statistics on upper plot
    fOutMCDStacks[dh]->Draw("HIST");
    fOutDataDHists[dh]->Draw("PE SAME");
    fDLegend[dh]->Draw("SAME");
    
    // lower pad is ratio
    fOutDCanvas[dh]->cd();          // Go back to the main canvas before defining pad2
    fOutDRatioPad[dh]->Draw(); // draw lower pad
    fOutDRatioPad[dh]->cd(); // lower pad is current pad
    fOutRatioDHists[dh]->GetXaxis()->SetNdivisions(505);
    fOutRatioDHists[dh]->GetYaxis()->SetNdivisions(505);
    fOutRatioDHists[dh]->Draw("ep"); // lower pad is current pad

    // save the overplot+ratio with log scale
    fOutDStackPad[dh]->cd(); // upper pad is current pad
    fOutDStackPad[dh]->SetLogy(1); //  set logy on this pad
    fOutDCanvas[dh]->cd();          // Go back to the main canvas before saving
    fOutDCanvas[dh]->SaveAs(Form("%s/%s_log.%s",fOutName.Data(),fDHistNames[dh].Data(),fOutType.Data()));
    fOutFile->cd();
    fOutDCanvas[dh]->Write();

    // save the overplot+ratio without log scale
    fOutDStackPad[dh]->cd(); // upper pad is current pad
    fOutDStackPad[dh]->SetLogy(0); //  set no logy on this pad
    fOutDCanvas[dh]->cd();          // Go back to the main canvas before saving
    fOutDCanvas[dh]->SaveAs(Form("%s/%s_lin.%s",fOutName.Data(),fDHistNames[dh].Data(),fOutType.Data()));
    fOutFile->cd();
    fOutDCanvas[dh]->Write();
  }

  // int
  for (UInt_t ih = 0; ih < fNIHists; ih++){ // double hists
    //upper plot is stack
    fOutICanvas[ih]->cd();
    fOutIStackPad[ih]->Draw(); // draw upper pad
    fOutIStackPad[ih]->cd(); // upper pad is current pad
    fOutDataIHists[ih]->SetStats(0); // No statistics on upper plot
    fOutMCIStacks[ih]->Draw("HIST");
    fOutDataIHists[ih]->Draw("PE SAME");
    fDLegend[ih]->Draw("SAME");
   
    // lower pad is ratio
    fOutICanvas[ih]->cd();          // Go back to the main canvas before defining pad2
    fOutIRatioPad[ih]->Draw(); // draw lower pad
    fOutIRatioPad[ih]->cd(); // lower pad is current pad
    fOutRatioIHists[ih]->Draw("ep"); // lower pad is current pad

    // save the overplot+ratio with logy
    fOutIStackPad[ih]->cd(); // upper pad is current pad
    fOutIStackPad[ih]->SetLogy(1); //  set logy on this pad
    fOutICanvas[ih]->cd();          // Go back to the main canvas before saving
    fOutICanvas[ih]->SaveAs(Form("%s/%s_log.%s",fOutName.Data(),fIHistNames[ih].Data(),fOutType.Data()));
    fOutFile->cd();
    fOutICanvas[ih]->Write();

    // save without logy
    fOutIStackPad[ih]->cd(); // upper pad is current pad
    fOutIStackPad[ih]->SetLogy(0); //  set logy on this pad
    fOutICanvas[ih]->cd();          // Go back to the main canvas before saving
    fOutICanvas[ih]->SaveAs(Form("%s/%s_lin.%s",fOutName.Data(),fIHistNames[ih].Data(),fOutType.Data()));
    fOutFile->cd();
    fOutICanvas[ih]->Write();
  }

  // save output file
  fOutFile->Write();
}
