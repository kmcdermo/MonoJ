#include "StackPlots.hh"
#include "Common.hh"

StackPlots::StackPlots(const TStrVec dtsamples, const TStrVec mcsamples, const TStrVec dhistNames, const TStrVec ihistNames, const TString outname, const TString outtype){
  // input data members
  fDataNames = dtsamples; // set samples to add
  fMCNames   = mcsamples; // set samples to stack
  fDHistNames = dhistNames; // set double plots to produce
  fIHistNames = ihistNames; // set int plots to produce

  // store these too
  fNData   = fDataNames.size();
  fNMC     = fMCNames.size();
  fNDHists = fDHistNames.size();
  fNIHists = fIHistNames.size();

  // define color map here --> will move this plus type defs to a dedicated file
  fColorMap["zmumu"] = kCyan;
  
  // define title map here --> will move
  fSampleTitleMap["zmumu"] = "Z #rightarrow #mu^{+} #mu^{-}";

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
  

  StackPlots::DoStacks();
}

StackPlots::~StackPlots(){
}

void StackPlots::OpenInputFiles() {
  // open input files into TFileVec --> data 
  fDataFiles.reserve(fNData);
  for (UInt_t dt = 0; dt < fNData; dt++) {
    fDataFiles[dt] = TFile::Open(Form("%s_data/%s_data_plots.root",fDataNames[dt].Data(),fDataNames[dt].Data()));
  }

  // open input files into TFileVec --> mc 
  fMCFiles.reserve(fNMC);
  for (UInt_t mc = 0; mc < fNMC; mc++) {
    fMCFiles[mc] = TFile::Open(Form("%s_MC/%s_MC_plots.root",fMCNames[mc].Data(),fMCNames[mc].Data()));
  }
}

void StackPlots::InitInputPlots() {
  // init input double hists
  fInDataDHists.reserve(fNDHists);
  fInMCDHists.reserve(fNDHists);
  for (UInt_t dh = 0; dh < fNDHists; dh++){ // loop over double hists
    
    // data first
    fInDataDHists[dh].reserve(fNData); 
    for (UInt_t dt = 0; dt < fNData; dt++) { // init data double hists
      fInDataDHists[dh][dt] = (TH1D*)fDataFiles[dt]->Get(Form("%s",fDHistNames[dh].Data()));
    }

    // mc second
    fInMCDHists[dh].reserve(fNMC); 
    for (UInt_t mc = 0; mc < fNMC; mc++) { // init mc double hists
      fInMCDHists[dh][mc] = (TH1D*)fMCFiles[mc]->Get(Form("%s",fDHistNames[dh].Data()));
      fInMCDHists[dh][mc]->SetFillColor(fColorMap[fMCNames[mc]]);
      fInMCDHists[dh][mc]->SetLineColor(fColorMap[fMCNames[mc]]);
      std::cout << dh << " " << mc << " " << fInMCDHists[dh][mc]->GetName() << std::endl;
    }
  }

  // init input int hists
  fInDataIHists.reserve(fNIHists);
  fInMCIHists.reserve(fNIHists);
  for (UInt_t ih = 0; ih < fNIHists; ih++){ // loop over int hists
    // data first
    fInDataIHists[ih].reserve(fNData); 
    for (UInt_t dt = 0; dt < fNData; dt++) { // init data int hists
      fInDataIHists[ih][dt] = (TH1I*)fDataFiles[dt]->Get(Form("%s",fIHistNames[ih].Data()));
    }

    // mc second
    fInMCIHists[ih].reserve(fNMC); 
    for (UInt_t mc = 0; mc < fNMC; mc++) { // init mc int hists
      fInMCIHists[ih][mc] = (TH1I*)fMCFiles[mc]->Get(Form("%s",fIHistNames[ih].Data()));
      fInMCIHists[ih][mc]->SetFillColor(fColorMap[fMCNames[mc]]);
      fInMCIHists[ih][mc]->SetLineColor(fColorMap[fMCNames[mc]]);
    }
  }
}

void StackPlots::InitOutputPlots() {
  // init output hists and sumw2 for Data + MC -- init output stacks for MC

  // double hists
  fOutDataDHists.reserve(fNDHists); // make enough space for data double hists
  fOutMCDHists.reserve(fNDHists); // make enough space for MC double hists
  fOutMCDStacks.reserve(fNDHists); // same with stack 
  for (UInt_t dh = 0; dh < fNDHists; dh++){
    fOutMCDStacks[dh] = new THStack("","");
  }
  
  // int hists
  fOutDataIHists.reserve(fNIHists); // make enough space for data int hists
  fOutMCIHists.reserve(fNIHists); // make enough space for MC int hists
  fOutMCIStacks.reserve(fNIHists); // same with stack 
  for (UInt_t ih = 0; ih < fNIHists; ih++){
    fOutMCIStacks[ih] = new THStack("","");
  }
}

void StackPlots::InitOutputLegends() {
  // init legends

  // double
  fDLegend.reserve(fNDHists);
  for (UInt_t dh = 0; dh < fNDHists; dh++){
    fDLegend[dh] = new TLegend();
  }

  // int
  fILegend.reserve(fNIHists);
  for (UInt_t ih = 0; ih < fNIHists; ih++){
    fILegend[ih] = new TLegend();
  }
}

void StackPlots::InitRatioPlots() {
  // init ratios 

  // double
  fOutRatioDHists.reserve(fNDHists);

  // int
  fOutRatioIHists.reserve(fNIHists);
}

void StackPlots::InitOutputCanvPads() {
  // init canvases + pads

  // double
  fOutDCanvas.reserve(fNDHists);
  fOutDStackPad.reserve(fNDHists);
  fOutDRatioPad.reserve(fNDHists);
  for (UInt_t dh = 0; dh < fNDHists; dh++){
    fOutDCanvas[dh] = new TCanvas();
    
    fOutDStackPad[dh] = new TPad("", "", 0, 0.3, 1, 1.0);
    fOutDStackPad[dh]->SetBottomMargin(0); // Upper and lower plot are joined
    
    fOutDRatioPad[dh] = new TPad("", "", 0, 0.05, 1, 0.3);
    fOutDRatioPad[dh]->SetTopMargin(0);
    fOutDRatioPad[dh]->SetBottomMargin(0.2);
  }

  // int
  fOutICanvas.reserve(fNIHists);
  fOutIStackPad.reserve(fNIHists);
  fOutIRatioPad.reserve(fNIHists);
  for (UInt_t ih = 0; ih < fNIHists; ih++){
    fOutICanvas[ih] = new TCanvas();

    fOutIStackPad[ih] = new TPad("", "", 0, 0.3, 1, 1.0);
    fOutIStackPad[ih]->SetBottomMargin(0); // Upper and lower plot are joined

    fOutIRatioPad[ih] = new TPad("", "", 0, 0.3, 1, 1.0);
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
	//	fOutDataDHists[dh]->Sumw2();
      }
      else{
	fOutDataDHists[dh]->Add(fInDataDHists[dh][dt]);
      }
    } // end loop over data samples
    fDLegend[dh]->AddEntry(fOutDataDHists[dh],"Data","l"); // add data entry to legend

    // mc, copy + add to hists, add to tstack
    for (UInt_t mc = 0; mc < fNMC; mc++) {
      if (mc == 0){ // add first for ratio

	std::cout << dh << " " << mc << " " << fInMCDHists[dh][mc]->GetName() << std::endl;
	fOutMCDHists[dh] = (TH1D*)fInMCDHists[dh][mc]->Clone();
	//	fOutMCDHists[dh]->Sumw2();
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
	fOutDataIHists[ih]->Sumw2();
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
	fOutMCIHists[ih]->Sumw2();
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
    fOutRatioDHists[dh]->Sumw2();
    fOutRatioDHists[dh]->Divide(fOutMCDHists[dh]);  
    fOutRatioDHists[dh]->SetLineColor(kBlack);
    fOutRatioDHists[dh]->SetMinimum(0.0);  // Define Y ..
    fOutRatioDHists[dh]->SetMaximum(2.0); // .. range
    fOutRatioDHists[dh]->SetStats(0);      // No statistics on lower plot
  }

  // int
  for (UInt_t ih = 0; ih < fNIHists; ih++){ // int hists
    fOutRatioIHists[ih] = (TH1I*)fOutDataIHists[ih]->Clone();
    fOutRatioIHists[ih]->Sumw2();
    fOutRatioIHists[ih]->Divide(fOutMCIHists[ih]);  
    fOutRatioIHists[ih]->SetLineColor(kBlack);
    fOutRatioIHists[ih]->SetMinimum(0.0);  // Define Y ..
    fOutRatioIHists[ih]->SetMaximum(2.0); // .. range
    fOutRatioIHists[ih]->SetStats(0);      // No statistics on lower plot
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
    fOutDataDHists[dh]->Draw("PE");
    fOutMCDStacks[dh]->Draw("HIST SAME");
    fDLegend[dh]->Draw("SAME");
   
    // lower pad is ratio
    fOutDCanvas[dh]->cd();          // Go back to the main canvas before defining pad2
    fOutDRatioPad[dh]->Draw(); // draw lower pad
    fOutDRatioPad[dh]->cd(); // lower pad is current pad
    fOutRatioDHists[dh]->Draw("ep"); // lower pad is current pad

    // save the overplot+ratio
    fOutDCanvas[dh]->cd();          // Go back to the main canvas before defining pad2
    fOutDCanvas[dh]->SaveAs(Form("%s/%s.%s",fOutName.Data(),fDHistNames[dh].Data(),fOutType.Data()));
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
    fOutDataIHists[ih]->Draw("PE");
    fOutMCIStacks[ih]->Draw("HIST SAME");
    fDLegend[ih]->Draw("SAME");
   
    // lower pad is ratio
    fOutICanvas[ih]->cd();          // Go back to the main canvas before defining pad2
    fOutIRatioPad[ih]->Draw(); // draw lower pad
    fOutIRatioPad[ih]->cd(); // lower pad is current pad
    fOutRatioIHists[ih]->Draw("ep"); // lower pad is current pad

    // save the overplot+ratio
    fOutICanvas[ih]->cd();          // Go back to the main canvas before defining pad2
    fOutICanvas[ih]->SaveAs(Form("%s/%s.%s",fOutName.Data(),fIHistNames[ih].Data(),fOutType.Data()));
    fOutFile->cd();
    fOutICanvas[ih]->Write();
  }

  // save output file
  fOutFile->Write();
}
