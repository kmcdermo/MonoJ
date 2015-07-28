#include "StackPlots.hh"

StackPlots::StackPlots(SamplePairVec Samples, const Double_t lumi, const ColorMap colorMap, const TString outname, const TString outtype){
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
  fColorMap = colorMap;

  // with all that defined, initialize everything in constructor

  StackPlots::OpenInputFiles();
  StackPlots::InitInputPlots();
  StackPlots::InitOutputPlots();
  StackPlots::InitOutputLegends();
  StackPlots::InitRatioPlots();
  StackPlots::InitOutputCanvPads();
}

StackPlots::~StackPlots(){
  // delete all pointers
  for (UInt_t data = 0; data < fNData; data++) {
    delete fDataFiles[data];
  }

  for (UInt_t mc = 0; mc < fNMC; mc++) {
    delete fMCFiles[mc];
  }

  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){
    delete fOutDataTH1DHists[th1d];
    delete fOutMCTH1DHists[th1d];
    delete fOutMCTH1DStacks[th1d];

    delete fTH1DLegends[th1d];
    delete fOutRatioTH1DHists[th1d];
    delete fOutTH1DCanvases[th1d];
    delete fOutTH1DStackPads[th1d];
    delete fOutTH1DRatioPads[th1d];
    
    for (UInt_t data = 0; data < fNData; data++) {
      delete fInDataTH1DHists[th1d][data];
    }

    for (UInt_t mc = 0; mc < fNMC; mc++) {
      delete fInMCTH1DHists[th1d][mc];
    }
  }

  delete fOutFile;
}

void StackPlots::OpenInputFiles() {
  // open input files into TFileVec --> data 
  fDataFiles.resize(fNData);
  for (UInt_t data = 0; data < fNData; data++) {
    TString datafile = Form("%s_data/%s_data_plots.root",fDataNames[data].Data(),fDataNames[data].Data());
    fDataFiles[data] = TFile::Open(datafile.Data());
    CheckValidFile(fDataFiles[data],datafile);
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
  fInDataTH1DHists.resize(fNTH1D);
  fInMCTH1DHists.resize(fNTH1D);
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){ // loop over double hists
    // data first
    fInDataTH1DHists[th1d].resize(fNData); 
    for (UInt_t data = 0; data < fNData; data++) { // init data double hists
      fInDataTH1DHists[th1d][data] = (TH1D*)fDataFiles[data]->Get(Form("%s",fTH1DNames[th1d].Data()));
      CheckValidTH1D(fInDataTH1DHists[th1d][data],fTH1DNames[th1d],fDataFiles[data]->GetName());
    }

    // mc second
    fInMCTH1DHists[th1d].resize(fNMC); 
    for (UInt_t mc = 0; mc < fNMC; mc++) { // init mc double hists
      fInMCTH1DHists[th1d][mc] = (TH1D*)fMCFiles[mc]->Get(Form("%s",fTH1DNames[th1d].Data()));
      CheckValidTH1D(fInMCTH1DHists[th1d][mc],fTH1DNames[th1d],fMCFiles[mc]->GetName());
      fInMCTH1DHists[th1d][mc]->SetFillColor(fColorMap[fMCNames[mc]]);
      fInMCTH1DHists[th1d][mc]->SetLineColor(fColorMap[fMCNames[mc]]);
    }
  }
}

void StackPlots::InitOutputPlots() {
  // init output hists and sumw2 for Data + MC -- init output stacks for MC

  // th1D hists
  fOutDataTH1DHists.resize(fNTH1D); // make enough space for data double hists
  fOutMCTH1DHists.resize(fNTH1D); // make enough space for MC double hists
  fOutMCTH1DStacks.resize(fNTH1D); // same with stack 
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){
    fOutMCTH1DStacks[th1d] = new THStack("","");
  }
}

void StackPlots::InitOutputLegends() {
  // init legends

  // th1D hists
  fTH1DLegends.resize(fNTH1D);
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){
    fTH1DLegends[th1d] = new TLegend(0.75,0.8,0.85,0.93);
  }
}

void StackPlots::InitRatioPlots() {
  // init ratios 

  // th1d hists
  fOutRatioTH1DHists.resize(fNTH1D);
}

void StackPlots::InitOutputCanvPads() {
  // init canvases + pads

  // th1d
  fOutTH1DCanvases.resize(fNTH1D);
  fOutTH1DStackPads.resize(fNTH1D);
  fOutTH1DRatioPads.resize(fNTH1D);
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){
    fOutTH1DCanvases[th1d] = new TCanvas(fTH1DNames[th1d].Data(),"");
    fOutTH1DCanvases[th1d]->cd();
    
    fOutTH1DStackPads[th1d] = new TPad("", "", 0, 0.3, 1.0, 0.99);
    fOutTH1DStackPads[th1d]->SetBottomMargin(0); // Upper and lower plot are joined
    
    fOutTH1DRatioPads[th1d] = new TPad("", "", 0, 0.05, 1.0, 0.3);
    fOutTH1DRatioPads[th1d]->SetTopMargin(0);
    fOutTH1DRatioPads[th1d]->SetBottomMargin(0.2);
  }
}

void StackPlots::DoStacks() {
  StackPlots::MakeStackPlots();
  StackPlots::MakeRatioPlots();
  StackPlots::MakeOutputCanvas();
}

void StackPlots::MakeStackPlots(){
  // copy th1d plots into output hists/stacks
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){ //th1d hists
    // data, copy + add only
    for (UInt_t data = 0; data < fNData; data++) {
      if (data == 0){
	fOutDataTH1DHists[th1d] = (TH1D*)fInDataTH1DHists[th1d][data]->Clone();
      }
      else{
	fOutDataTH1DHists[th1d]->Add(fInDataTH1DHists[th1d][data]);
      }
    } // end loop over data samples
    fTH1DLegends[th1d]->AddEntry(fOutDataTH1DHists[th1d],"Data","pl"); // add data entry to legend

    // mc, copy + add to hists, add to tstack
    for (UInt_t mc = 0; mc < fNMC; mc++) {
      if (mc == 0){ // add first for ratio
	fOutMCTH1DHists[th1d] = (TH1D*)fInMCTH1DHists[th1d][mc]->Clone();
      }
      else{ // add first for ratio
	fOutMCTH1DHists[th1d]->Add(fInMCTH1DHists[th1d][mc]);
      }
      //  just add input to stacks
      fOutMCTH1DStacks[th1d]->Add(fInMCTH1DHists[th1d][mc]);
      fTH1DLegends[th1d]->AddEntry(fInMCTH1DHists[th1d][mc],fSampleTitleMap[fMCNames[mc]],"lf");
      //      std::cout << "supposed title: " << fInMCTH1DHists[th1d][mc]->GetYaxis()->GetTitle() << std::endl;
    } // end loop over mc samples
  } // end loop over th1d plots
}

void StackPlots::MakeRatioPlots() {
  // Define the ratio plot
  
  // th1d
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){ // double hists
    fOutRatioTH1DHists[th1d] = (TH1D*)fOutDataTH1DHists[th1d]->Clone();
    fOutRatioTH1DHists[th1d]->Divide(fOutMCTH1DHists[th1d]);  
    fOutRatioTH1DHists[th1d]->SetLineColor(kBlack);
    fOutRatioTH1DHists[th1d]->SetMinimum(0.0);  // Define Y ..
    fOutRatioTH1DHists[th1d]->SetMaximum(2.0); // .. range
    fOutRatioTH1DHists[th1d]->SetStats(0);      // No statistics on lower plot
    fOutRatioTH1DHists[th1d]->GetYaxis()->SetTitle("Data/MC");
  }
}

void StackPlots::MakeOutputCanvas() {
  // make output canvases
  
  // th1d
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){ // double hists
    //upper plot is stack
    fOutTH1DCanvases[th1d]->cd();
    fOutTH1DStackPads[th1d]->Draw(); // draw upper pad   
    fOutTH1DStackPads[th1d]->cd(); // upper pad is current pad
    fOutDataTH1DHists[th1d]->SetStats(0); // No statistics on upper plot
    fOutMCTH1DStacks[th1d]->Draw("HIST");
    fOutDataTH1DHists[th1d]->Draw("PE SAME");
    fTH1DLegends[th1d]->Draw("SAME");

    // lower pad is ratio
    fOutTH1DCanvases[th1d]->cd();          // Go back to the main canvas before defining pad2
    fOutTH1DRatioPads[th1d]->Draw(); // draw lower pad
    fOutTH1DRatioPads[th1d]->cd(); // lower pad is current pad
    fOutRatioTH1DHists[th1d]->GetXaxis()->SetNdivisions(505);
    fOutRatioTH1DHists[th1d]->GetYaxis()->SetNdivisions(505);
    fOutTH1DRatioPads[th1d]->Update();
    fOutRatioTH1DHists[th1d]->Draw("EP"); // lower pad is current pad
    TLine ratioline(fOutTH1DRatioPads[th1d]->GetUxmin(),1.0,fOutTH1DRatioPads[th1d]->GetUxmax(),1.0);
    ratioline.SetLineColor(kRed);
    ratioline.SetLineWidth(2);
    ratioline.Draw("SAME");

    // save the overplot+ratio with log scale
    fOutTH1DStackPads[th1d]->cd(); // upper pad is current pad
    fOutTH1DStackPads[th1d]->SetLogy(1); //  set logy on this pad
    fOutTH1DCanvases[th1d]->cd();          // Go back to the main canvas before saving
    StackPlots::CMSLumi(fOutTH1DCanvases[th1d],10); // write out Lumi info
    fOutTH1DCanvases[th1d]->SaveAs(Form("%s/%s_log.%s",fOutName.Data(),fTH1DNames[th1d].Data(),fOutType.Data()));
    fOutFile->cd();
    fOutTH1DCanvases[th1d]->Write(Form("%s_log",fTH1DNames[th1d].Data()));

    // save the overplot+ratio without log scale
    fOutTH1DStackPads[th1d]->cd(); // upper pad is current pad
    fOutTH1DStackPads[th1d]->SetLogy(0); //  set no logy on this pad
    fOutTH1DCanvases[th1d]->cd();          // Go back to the main canvas before saving
    StackPlots::CMSLumi(fOutTH1DCanvases[th1d],10); // write out Lumi info
    fOutTH1DCanvases[th1d]->SaveAs(Form("%s/%s_lin.%s",fOutName.Data(),fTH1DNames[th1d].Data(),fOutType.Data()));
    fOutFile->cd();
    fOutTH1DCanvases[th1d]->Write(Form("%s_lin",fTH1DNames[th1d].Data()));
  }
}

void StackPlots::CMSLumi(TCanvas *& canv, const Int_t iPosX) { // borrowed from margaret
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
    fOutDataTH1DHists[th1d]->GetYaxis()->SetTitle("akdfadf");
    fOutTH1DStackPads[th1d]->Modified();
*/

    /* // need a way to not cut off labels
    TGaxis tgaxis(fOutTH1DStackPads[th1d]->GetUxmin(),fOutTH1DStackPads[th1d]->GetUymin(),fOutTH1DStackPads[th1d]->GetUxmin(),fOutTH1DStackPads[th1d]->GetUymax(),fOutTH1DStackPads[th1d]->GetUymin(),fOutTH1DStackPads[th1d]->GetUymax(),505);
    tgaxis.Draw("SAME");
    */
