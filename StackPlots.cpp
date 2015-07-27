#include "StackPlots.hh"
#include "Common.hh"
#include "TGaxis.h"

StackPlots::StackPlots(SamplePairVec Samples, const Double_t lumi, const TString outname, const TString outtype){
  // input data members
  for (SamplePairVecIter iter = Samples.begin(); iter != Samples.end(); ++iter) {
    if ((*iter).second) { // isMC == true
      fMCNames.push_back((*iter).first);
    }
    else { // data
      fDataNames.push_back((*iter).first);
    }
  }

  // store for later ... would rather have move semantics ... iterators too annoying
  fNData = fDataNames.size();
  fNMC   = fMCNames.size();

  // save lumi
  fLumi = lumi;

  // use the plots already stored in Analysis.cpp --> doubles
  fTH1DNames.push_back("zmass");
  fTH1DNames.push_back("zpt");
  fTH1DNames.push_back("pfmet");
  fTH1DNames.push_back("signaljetpt");
  fTH1DNames.push_back("signaljeteta");
  fTH1DNames.push_back("signaljetCHfrac");
  fTH1DNames.push_back("signaljetNHfrac");
  fTH1DNames.push_back("signaljetEMfrac");
  fTH1DNames.push_back("signaljetCEMfrac");
  fTH1DNames.push_back("njets");
  fTH1DNames.push_back("nvtx");

  // store this too
  fNTH1D = fTH1DNames.size();

  // output data members
  fOutName = outname; // where to put output plots 
  MakeOutDirectory(fOutName); // make output directory --> same name as outname
  fOutFile = new TFile(Form("%s/%s_canvs.root",fOutName.Data(),fOutName.Data()),"RECREATE"); // make output tfile --> store canvas images here too, for quick editting
  fOutType = outtype; // allow user to pick png, pdf, gif, etc for stacked plots

  // define color map + title map
  fSampleTitleMap["zmumu"] = "Z #rightarrow #mu^{+} #mu^{-}";
  fSampleTitleMap["ttbar"] = "t#bar{t}";

  fColorMap["zmumu"] = kCyan;
  fColorMap["ttbar"] = kRed+2;

  // with all that defined, initialize everything in constructor

  StackPlots::OpenInputFiles();
  StackPlots::InitInputPlots();
  StackPlots::InitOutputPlots();
  StackPlots::InitOutputLegends();
  StackPlots::InitRatioPlots();
  StackPlots::InitOutputCanvPads();
}

StackPlots::~StackPlots(){
}

void StackPlots::OpenInputFiles() {
  // open input files into TFileVec --> data 
  fDataFiles.resize(fNData);
  for (UInt_t dt = 0; dt < fNData; dt++) {
    TString datafile = Form("%s_data/%s_data_plots.root",fDataNames[dt].Data(),fDataNames[dt].Data());
    fDataFiles[dt] = TFile::Open(datafile.Data());
    CheckValidFile(fDataFiles[dt],datafile);
  }

  // open input files into TFileVec --> mc 
  fMCFiles.resize(fNMC);
  for (UInt_t mc = 0; mc < fNMC; mc++) {
    TString mcfile = Form("%s_MC/%s_MC_plots.root",fMCNames[mc].Data(),fMCNames[mc].Data());
    fMCFiles[mc] = TFile::Open(mcfile.Data());
    CheckValidFile(fMCFiles[mc],mcfile);
  }
}

void StackPlots::InitInputPlots() {
  // init input th1d hists
  fInDataTH1Ds.resize(fNTH1D);
  fInMCTH1Ds.resize(fNTH1D);
  for (UInt_t ith1d = 0; ith1d < fNTH1D; ith1d++){ // loop over double hists
    // data first
    fInDataTH1D[ith1d].resize(fNData); 
    for (UInt_t dt = 0; dt < fNData; dt++) { // init data double hists
      fInDataTH1Ds[ith1d][dt] = (TH1D*)fDataFiles[dt]->Get(Form("%s",fTH1DNames[ith1d].Data()));
      CheckValidTH1D(fInDataTH1Ds[ith1d][dt],fTH1DNames[ith1d],fDataFiles[dt]->GetName());
    }

    // mc second
    fInMCTH1Ds[ith1d].resize(fNMC); 
    for (UInt_t mc = 0; mc < fNMC; mc++) { // init mc double hists
      fInMCTH1Ds[ith1d][mc] = (TH1D*)fMCFiles[mc]->Get(Form("%s",fTH1DNames[ith1d].Data()));
      CheckValidTH1D(fInMCTH1Ds[ith1d][mc],fTH1DNames[ith1d],fMCFiles[mc]->GetName());
      fInMCTH1Ds[ith1d][mc]->SetFillColor(fColorMap[fMCNames[mc]]);
      fInMCTH1Ds[ith1d][mc]->SetLineColor(fColorMap[fMCNames[mc]]);
    }
  }
}

void StackPlots::InitOutputPlots() {
  // init output hists and sumw2 for Data + MC -- init output stacks for MC

  // th1D hists
  fOutDataTH1Ds.resize(fNTH1D); // make enough space for data double hists
  fOutMCDHists.resize(fNTH1D); // make enough space for MC double hists
  fOutMCDStacks.resize(fNTH1D); // same with stack 
  for (UInt_t ith1d = 0; ith1d < fNTH1D; ith1d++){
    fOutMCDStacks[ith1d] = new THStack("","");
  }
}

void StackPlots::InitOutputLegends() {
  // init legends

  // th1D hists
  fDLegend.resize(fNTH1D);
  for (UInt_t ith1d = 0; ith1d < fNTH1D; ith1d++){
    fDLegend[ith1d] = new TLegend(0.75,0.8,0.85,0.93);
  }
}

void StackPlots::InitRatioPlots() {
  // init ratios 

  // th1d hists
  fOutRatioDHists.resize(fNTH1D);
}

void StackPlots::InitOutputCanvPads() {
  // init canvases + pads

  // th1d
  fOutDCanvas.resize(fNTH1D);
  fOutDStackPad.resize(fNTH1D);
  fOutDRatioPad.resize(fNTH1D);
  for (UInt_t ith1d = 0; ith1d < fNTH1D; ith1d++){
    fOutDCanvas[ith1d] = new TCanvas(fTH1DNames[ith1d].Data(),"");
    fOutDCanvas[ith1d]->cd();
    
    fOutDStackPad[ith1d] = new TPad("", "", 0, 0.3, 1.0, 0.99);
    fOutDStackPad[ith1d]->SetBottomMargin(0); // Upper and lower plot are joined
    
    fOutDRatioPad[ith1d] = new TPad("", "", 0, 0.05, 1.0, 0.3);
    fOutDRatioPad[ith1d]->SetTopMargin(0);
    fOutDRatioPad[ith1d]->SetBottomMargin(0.2);
  }
}

void StackPlots::DoStacks() {
  StackPlots::MakeStackPlots();
  StackPlots::MakeRatioPlots();
  StackPlots::MakeOutputCanvas();
}

void StackPlots::MakeStackPlots(){
  // copy th1d plots into output hists/stacks
  for (UInt_t ith1d = 0; ith1d < fNTH1D; ith1d++){ //th1d hists
    // data, copy + add only
    for (UInt_t dt = 0; dt < fNData; dt++) {
      if (dt == 0){
	fOutDataTH1Ds[ith1d] = (TH1D*)fInDataTH1Ds[ith1d][dt]->Clone();
      }
      else{
	fOutDataTH1Ds[ith1d]->Add(fInDataTH1Ds[ith1d][dt]);
      }
    } // end loop over data samples
    fDLegend[ith1d]->AddEntry(fOutDataTH1Ds[ith1d],"Data","pl"); // add data entry to legend

    // mc, copy + add to hists, add to tstack
    for (UInt_t mc = 0; mc < fNMC; mc++) {
      if (mc == 0){ // add first for ratio
	fOutMCDHists[ith1d] = (TH1D*)fInMCTH1Ds[ith1d][mc]->Clone();
      }
      else{ // add first for ratio
	fOutMCDHists[ith1d]->Add(fInMCTH1Ds[ith1d][mc]);
      }
      //  just add input to stacks
      fOutMCDStacks[ith1d]->Add(fInMCTH1Ds[ith1d][mc]);
      fDLegend[ith1d]->AddEntry(fInMCTH1Ds[ith1d][mc],fSampleTitleMap[fMCNames[mc]],"lf");
      //      std::cout << "supposed title: " << fInMCTH1Ds[ith1d][mc]->GetYaxis()->GetTitle() << std::endl;
    } // end loop over mc samples
  } // end loop over th1d plots
}

void StackPlots::MakeRatioPlots() {
  // Define the ratio plot
  
  // th1d
  for (UInt_t ith1d = 0; ith1d < fNTH1D; ith1d++){ // double hists
    fOutRatioDHists[ith1d] = (TH1D*)fOutDataTH1Ds[ith1d]->Clone();
    fOutRatioDHists[ith1d]->Divide(fOutMCDHists[ith1d]);  
    fOutRatioDHists[ith1d]->SetLineColor(kBlack);
    fOutRatioDHists[ith1d]->SetMinimum(0.0);  // Define Y ..
    fOutRatioDHists[ith1d]->SetMaximum(2.0); // .. range
    fOutRatioDHists[ith1d]->SetStats(0);      // No statistics on lower plot
    fOutRatioDHists[ith1d]->GetYaxis()->SetTitle("Data/MC");
  }
}

void StackPlots::MakeOutputCanvas() {
  // make output canvases
  
  // th1d
  for (UInt_t ith1d = 0; ith1d < fNTH1D; ith1d++){ // double hists
    //upper plot is stack
    fOutDCanvas[ith1d]->cd();
    fOutDStackPad[ith1d]->Draw(); // draw upper pad   
    fOutDStackPad[ith1d]->cd(); // upper pad is current pad
    fOutDataTH1Ds[ith1d]->SetStats(0); // No statistics on upper plot
    fOutMCDStacks[ith1d]->Draw("HIST");
    fOutDataTH1Ds[ith1d]->Draw("PE SAME");
    fDLegend[ith1d]->Draw("SAME");

    // lower pad is ratio
    fOutDCanvas[ith1d]->cd();          // Go back to the main canvas before defining pad2
    fOutDRatioPad[ith1d]->Draw(); // draw lower pad
    fOutDRatioPad[ith1d]->cd(); // lower pad is current pad
    fOutRatioDHists[ith1d]->GetXaxis()->SetNdivisions(505);
    fOutRatioDHists[ith1d]->GetYaxis()->SetNdivisions(505);
    fOutDRatioPad[ith1d]->Update();
    fOutRatioDHists[ith1d]->Draw("EP"); // lower pad is current pad
    TLine ratioline(fOutDRatioPad[ith1d]->GetUxmin(),1.0,fOutDRatioPad[ith1d]->GetUxmax(),1.0);
    ratioline.SetLineColor(kRed);
    ratioline.SetLineWidth(2);
    ratioline.Draw("SAME");

    // save the overplot+ratio with log scale
    fOutDStackPad[ith1d]->cd(); // upper pad is current pad
    fOutDStackPad[ith1d]->SetLogy(1); //  set logy on this pad
    fOutDCanvas[ith1d]->cd();          // Go back to the main canvas before saving
    StackPlots::CMS_Lumi(fOutDCanvas[ith1d],10); // write out Lumi info
    fOutDCanvas[ith1d]->SaveAs(Form("%s/%s_log.%s",fOutName.Data(),fTH1DNames[ith1d].Data(),fOutType.Data()));
    fOutFile->cd();
    fOutDCanvas[ith1d]->Write(Form("%s_log",fTH1DNames[ith1d].Data()));

    // save the overplot+ratio without log scale
    fOutDStackPad[ith1d]->cd(); // upper pad is current pad
    fOutDStackPad[ith1d]->SetLogy(0); //  set no logy on this pad
    fOutDCanvas[ith1d]->cd();          // Go back to the main canvas before saving
    StackPlots::CMS_Lumi(fOutDCanvas[ith1d],10); // write out Lumi info
    fOutDCanvas[ith1d]->SaveAs(Form("%s/%s_lin.%s",fOutName.Data(),fTH1DNames[ith1d].Data(),fOutType.Data()));
    fOutFile->cd();
    fOutDCanvas[ith1d]->Write(Form("%s_lin",fTH1DNames[ith1d].Data()));
  }
}

void StackPlots::CMS_Lumi(TCanvas *& canv, const Int_t iPosX) { // borrowed from margaret
  TString cmsText      = "CMS";
  Double_t cmsTextFont = 61;  // default is helvetic-bold
  
  Bool_t writeExtraText  = true;
  TString extraText      = "Preliminary";
  Double_t extraTextFont = 52;  // default is helvetica-italics

  TString lumiText = Form("#sqrt{s} = 13 TeV, L = %f fb^{-1}", fLumi);
  
  // text sizes and text offsets with respect to the top frame
  // in unit of the top margin size
  Double_t lumiTextSize     = 0.6;
  Double_t lumiTextOffset   = 0.2;
  Double_t cmsTextSize      = 0.75;
  Double_t cmsTextOffset    = 0.1;  // only used in outOfFrame version

  Double_t relPosX    = 0.045;
  Double_t relPosY    = 0.035;
  Double_t relExtraDY = 1.2;
 
  // ratio of "CMS" and extra text size
  Double_t extraOverCmsTextSize  = 0.76;
 
  Bool_t outOfFrame    = false;
  if ( iPosX/10 == 0 ) {
    outOfFrame = true;
  }

  Int_t alignY_=3;
  Int_t alignX_=2;
  if (iPosX/10 == 0) {alignX_ = 1;}
  if (iPosX == 0)    {alignY_ = 1;}
  if (iPosX/10 == 1) {alignX_ = 1;}
  if (iPosX/10 == 2) {alignX_ = 2;}
  if (iPosX/10 == 3) {alignX_ = 3;}
  Int_t align_ = 10*alignX_ + alignY_;

  Double_t H = canv->GetWh();
  Double_t W = canv->GetWw();
  Double_t l = canv->GetLeftMargin();
  Double_t t = canv->GetTopMargin();
  Double_t r = canv->GetRightMargin();
  Double_t b = canv->GetBottomMargin();
  Double_t e = 0.025;

  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);    

  Double_t extraTextSize = extraOverCmsTextSize*cmsTextSize;

  latex.SetTextFont(42);
  latex.SetTextAlign(31); 
  latex.SetTextSize(lumiTextSize*t);    
  latex.DrawLatex(1-r,1-t+lumiTextOffset*t,lumiText);

  if (outOfFrame) {
    latex.SetTextFont(cmsTextFont);
    latex.SetTextAlign(11); 
    latex.SetTextSize(cmsTextSize*t);    
    latex.DrawLatex(l,1-t+lumiTextOffset*t,cmsText);
  }
  
  Double_t posX_;
  if (iPosX%10 <= 1) {
    posX_ =   l + relPosX*(1-l-r);
  }
  else if (iPosX%10 == 2) {
    posX_ =  l + 0.5*(1-l-r);
  }
  else if (iPosX%10 == 3) {
    posX_ =  1-r - relPosX*(1-l-r);
  }

  Double_t posY_ = 1-t - relPosY*(1-t-b);

  if (!outOfFrame) {
    latex.SetTextFont(cmsTextFont);
    latex.SetTextSize(cmsTextSize*t);
    latex.SetTextAlign(align_);
    latex.DrawLatex(posX_, posY_, cmsText);
    
    if (writeExtraText) {
      latex.SetTextFont(extraTextFont);
      latex.SetTextAlign(align_);
      latex.SetTextSize(extraTextSize*t);
      latex.DrawLatex(posX_, posY_- relExtraDY*cmsTextSize*t, extraText);
    }
  }
  
  else if (outOfFrame && writeExtraText){
    if (iPosX == 0) {
	posX_ = l +  relPosX*(1-l-r)+0.05;
	posY_ = 1-t+lumiTextOffset*t;
    }
    latex.SetTextFont(extraTextFont);
    latex.SetTextSize(extraTextSize*t);
    latex.SetTextAlign(align_);
    latex.DrawLatex(posX_, posY_, extraText);      
  }
}

/*  need y axis labels!!!!!!!!!!!
    fOutDataTH1Ds[ith1d]->GetYaxis()->SetTitle("akdfadf");
    fOutDStackPad[ith1d]->Modified();
*/

    /* // need a way to not cut off labels
    TGaxis tgaxis(fOutDStackPad[ith1d]->GetUxmin(),fOutDStackPad[ith1d]->GetUymin(),fOutDStackPad[ith1d]->GetUxmin(),fOutDStackPad[ith1d]->GetUymax(),fOutDStackPad[ith1d]->GetUymin(),fOutDStackPad[ith1d]->GetUymax(),505);
    tgaxis.Draw("SAME");
    */
