#include "../interface/StackPlots.hh"

StackPlots::StackPlots(SamplePairVec Samples, const TString selection, const Double_t lumi, const ColorMap colorMap, const TString outdir, const TString outtype){
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

  // save selection
  fSelection = selection;

  // save lumi
  fLumi = lumi;

  // use the plots already stored in Analysis.cpp ... use selection to decide what to loop over --> doubles

  fTH1DNames.push_back("mu1eta");
  fTH1DNames.push_back("mu1phi");
  fTH1DNames.push_back("mu1pt");
  fTH1DNames.push_back("mu2eta");
  fTH1DNames.push_back("mu2phi");
  fTH1DNames.push_back("mu2pt");

  fTH1DNames.push_back("el1eta");
  fTH1DNames.push_back("el1phi");
  fTH1DNames.push_back("el1pt");
  fTH1DNames.push_back("el2eta");
  fTH1DNames.push_back("el2phi");
  fTH1DNames.push_back("el2pt");
  
  fTH1DNames.push_back("zeta");
  fTH1DNames.push_back("zmass");
  fTH1DNames.push_back("zphi");
  fTH1DNames.push_back("zpt");

  fTH1DNames.push_back("zeeeta");
  fTH1DNames.push_back("zeemass");
  fTH1DNames.push_back("zeephi");
  fTH1DNames.push_back("zeept");

  fTH1DNames.push_back("emueta");
  fTH1DNames.push_back("emumass");
  fTH1DNames.push_back("emuphi");
  fTH1DNames.push_back("emupt");

  fTH1DNames.push_back("wmt");
  fTH1DNames.push_back("wemt");

  fTH1DNames.push_back("signaljeteta");
  fTH1DNames.push_back("signaljetphi");
  fTH1DNames.push_back("signaljetpt");
  fTH1DNames.push_back("signaljetCHfrac");
  fTH1DNames.push_back("signaljetNHfrac");
  fTH1DNames.push_back("signaljetEMfrac");
  fTH1DNames.push_back("signaljetCEMfrac");

  fTH1DNames.push_back("secondjeteta");
  fTH1DNames.push_back("secondjetphi");
  fTH1DNames.push_back("secondjetpt");
  fTH1DNames.push_back("secondjetCHfrac");
  fTH1DNames.push_back("secondjetNHfrac");
  fTH1DNames.push_back("secondjetEMfrac");
  fTH1DNames.push_back("secondjetCEMfrac");

  fTH1DNames.push_back("thirdjeteta");
  fTH1DNames.push_back("thirdjetphi");
  fTH1DNames.push_back("thirdjetpt");
  fTH1DNames.push_back("thirdjetCHfrac");
  fTH1DNames.push_back("thirdjetNHfrac");
  fTH1DNames.push_back("thirdjetEMfrac");
  fTH1DNames.push_back("thirdjetCEMfrac");

  fTH1DNames.push_back("ht");
  fTH1DNames.push_back("mht");

  fTH1DNames.push_back("pfmet");
  fTH1DNames.push_back("pfmetphi");
  fTH1DNames.push_back("t1pfmet");
  fTH1DNames.push_back("t1pfmetphi");
  fTH1DNames.push_back("mumet");
  fTH1DNames.push_back("mumetphi");
  fTH1DNames.push_back("t1mumet");
  fTH1DNames.push_back("t1mumetphi");

  fTH1DNames.push_back("njets");
  fTH1DNames.push_back("nvtx");

  // store this too
  fNTH1D = fTH1DNames.size();

  // output data members
  fOutDir  = outdir;
  fOutName = "stackedplots"; // where to put output stack plots 
  MakeOutDirectory(Form("%s/%s/%s",fOutDir.Data(),fSelection.Data(),fOutName.Data())); // make output directory 
  fOutFile = new TFile(Form("%s/%s/%s/stackplots_canvases.root",fOutDir.Data(),fSelection.Data(),fOutName.Data()),"RECREATE"); // make output tfile --> store canvas images here too, for quick editting
  fOutType = outtype; // allow user to pick png, pdf, gif, etc for stacked plots

  // define color map + title map
  fSampleTitleMap["zll"]   = "Z #rightarrow l^{+}l^{-}";
  fSampleTitleMap["wln"]   = "W #rightarrow l#nu";
  fSampleTitleMap["ttbar"] = "t#bar{t}";
  fSampleTitleMap["qcd"]   = "QCD";
  fColorMap = colorMap;

  // with all that defined, initialize everything in constructor

  StackPlots::OpenInputFiles();
  StackPlots::InitInputPlots();
  StackPlots::InitOutputPlots();
  StackPlots::InitOutputLegends();
  StackPlots::InitRatioPlots();
  StackPlots::InitRatioLines();
  StackPlots::InitOutputCanvPads();
  StackPlots::InitUpperAxes();
}

StackPlots::~StackPlots(){
  // delete all pointers
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){
    delete fOutRatioTH1DHists[th1d];
    delete fOutDataTH1DHists[th1d];
    delete fOutMCTH1DHists[th1d];
    delete fOutMCTH1DStacks[th1d];
    delete fTH1DLegends[th1d];
    delete fOutTH1DTGaxes[th1d];
    delete fOutTH1DRatioLines[th1d];
    delete fOutTH1DStackPads[th1d];
    delete fOutTH1DRatioPads[th1d];
    delete fOutTH1DCanvases[th1d];
    
    for (UInt_t data = 0; data < fNData; data++) {
      CheckValidTH1D(fInDataTH1DHists[th1d][data],fTH1DNames[th1d],fDataFiles[data]->GetName());
      delete fInDataTH1DHists[th1d][data];
    }
  
    for (UInt_t mc = 0; mc < fNMC; mc++) {
      delete fInMCTH1DHists[th1d][mc];
    }
  }

  for (UInt_t data = 0; data < fNData; data++) {
    delete fDataFiles[data];
  }

  for (UInt_t mc = 0; mc < fNMC; mc++) {
    delete fMCFiles[mc];
  }

  delete fOutFile;
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
    fOutRatioTH1DHists[th1d]->SetMinimum(-0.1);  // Define Y ..
    fOutRatioTH1DHists[th1d]->SetMaximum(2.1); // .. range
    fOutRatioTH1DHists[th1d]->SetStats(0);      // No statistics on lower plot
    fOutRatioTH1DHists[th1d]->GetYaxis()->SetTitle("Data/MC");
  }
}

void StackPlots::MakeOutputCanvas() {
  // make output canvases
  
  // th1d
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){ // double hists
    
    // draw first with  log scale
    Bool_t isLogY = true;
    StackPlots::DrawUpperPad(th1d,isLogY); // upper pad is stack
    StackPlots::DrawLowerPad(th1d); // lower pad is ratio
    StackPlots::SaveCanvas(th1d,isLogY);  // now save the canvas, w/ logy

    // draw second with lin scale
    isLogY = false;
    StackPlots::DrawUpperPad(th1d,isLogY); // upper pad is stack
    StackPlots::DrawLowerPad(th1d); // lower pad is ratio
    StackPlots::SaveCanvas(th1d,isLogY); // now save the canvas, w/o logy
  }
}

void StackPlots::DrawUpperPad(const UInt_t th1d, const Bool_t isLogY) {    
  // pad gymnastics
  fOutTH1DCanvases[th1d]->cd();
  fOutTH1DStackPads[th1d]->Draw(); // draw upper pad   
  fOutTH1DStackPads[th1d]->cd(); // upper pad is current pad
  
  // set maximum by comparing added mc vs added data
  Double_t max = StackPlots::GetMaximum(th1d);

  if (isLogY) {
    fOutDataTH1DHists[th1d]->SetMaximum(max*2.0);
  }
  else {
    fOutDataTH1DHists[th1d]->SetMaximum(max*1.05);
  }

  // now draw the plots for upper pad in absurd order because ROOT is dumb
  fOutDataTH1DHists[th1d]->Draw("PE"); // draw first so labels appear
  fOutMCTH1DStacks[th1d]->Draw("HIST SAME"); 
  fOutTH1DStackPads[th1d]->RedrawAxis("SAME"); // stack kills axis

  // maybe someday redraw axis to get proper labels
  // StackPlots::SetUpperAxes(th1d);
  // fOutMCTH1DStacks[th1d]->GetYaxis()->SetLabelSize(0.);
  // fOutDataTH1DHists[th1d]->GetYaxis()->SetLabelSize(0.);
  // fOutTH1DTGaxes[th1d]->Draw("SAME");

  fOutDataTH1DHists[th1d]->Draw("PE SAME"); // redraw data
  fTH1DLegends[th1d]->Draw("SAME");
}

void StackPlots::SetUpperAxes(const UInt_t th1d) { // to maybe never be used... cannot seem to work.
  // make new upper y-axis

  const Double_t x  = fOutDataTH1DHists[th1d]->GetXaxis()->GetXmin();
  const Double_t y1 = fOutDataTH1DHists[th1d]->GetYaxis()->GetXmin();
  const Double_t y2 = fOutDataTH1DHists[th1d]->GetYaxis()->GetXmax();

  fOutTH1DTGaxes[th1d]->SetX1( x );
  fOutTH1DTGaxes[th1d]->SetX2( x );

  fOutTH1DTGaxes[th1d]->SetY1( y1 * 0.05 );
  fOutTH1DTGaxes[th1d]->SetWmin( y1 * 0.05 );

  fOutTH1DTGaxes[th1d]->SetY2( y2 * 0.95 );
  fOutTH1DTGaxes[th1d]->SetWmax( y2 * 0.95 );

  fOutTH1DTGaxes[th1d]->SetLabelSize(0.11);
}

Double_t StackPlots::GetMaximum(const UInt_t th1d) {
  Double_t max = -100;
  if (fOutDataTH1DHists[th1d]->GetBinContent(fOutDataTH1DHists[th1d]->GetMaximumBin()) > fOutMCTH1DHists[th1d]->GetBinContent(fOutMCTH1DHists[th1d]->GetMaximumBin())) {
    max = fOutDataTH1DHists[th1d]->GetBinContent(fOutDataTH1DHists[th1d]->GetMaximumBin());
  }
  else {
    max = fOutMCTH1DHists[th1d]->GetBinContent(fOutMCTH1DHists[th1d]->GetMaximumBin());
  }
  return max;
}

void StackPlots::DrawLowerPad(const UInt_t th1d) {    
  // pad gymnastics
  fOutTH1DCanvases[th1d]->cd();   // Go back to the main canvas before defining pad2
  fOutTH1DRatioPads[th1d]->Draw(); // draw lower pad
  fOutTH1DRatioPads[th1d]->cd(); // lower pad is current pad

  // make red line at ratio of 1.0
  StackPlots::SetLines(th1d);

  // draw th1 first so line can appear, then draw over it (and set Y axis divisions)
  fOutRatioTH1DHists[th1d]->Draw("EP"); // draw first so line can appear
  fOutTH1DRatioLines[th1d]->Draw("SAME");

  // some style since apparently TDR Style is crapping out --> would really not like this here
  fOutRatioTH1DHists[th1d]->GetYaxis()->SetNdivisions(505);

  fOutRatioTH1DHists[th1d]->GetXaxis()->SetLabelSize(0.11);
  fOutRatioTH1DHists[th1d]->GetXaxis()->SetTitleSize(0.15);
  fOutRatioTH1DHists[th1d]->GetXaxis()->SetTickSize(0.11);

  fOutRatioTH1DHists[th1d]->GetYaxis()->SetLabelSize(0.11);
  fOutRatioTH1DHists[th1d]->GetYaxis()->SetTitleSize(0.15);
  fOutRatioTH1DHists[th1d]->GetYaxis()->SetTitleOffset(0.5);

  // redraw to go over line
  fOutRatioTH1DHists[th1d]->Draw("EP SAME"); 
}

void StackPlots::SetLines(const UInt_t th1d){
  // have line held at ratio of 1.0 over whole x range
  fOutTH1DRatioLines[th1d]->SetX1(fOutRatioTH1DHists[th1d]->GetXaxis()->GetXmin());
  fOutTH1DRatioLines[th1d]->SetY1(1.0);
  fOutTH1DRatioLines[th1d]->SetX2(fOutRatioTH1DHists[th1d]->GetXaxis()->GetXmax());
  fOutTH1DRatioLines[th1d]->SetY2(1.0);

  // customize appearance
  fOutTH1DRatioLines[th1d]->SetLineColor(kRed);
  fOutTH1DRatioLines[th1d]->SetLineWidth(2);
}

void StackPlots::SaveCanvas(const UInt_t th1d, const Bool_t isLogY){
  TString suffix;

  if (isLogY) {
    suffix = "log";
  }
  else {
    suffix = "lin";
  }

  // cd to upper pad to make it log or not
  fOutTH1DStackPads[th1d]->cd(); // upper pad is current pad
  fOutTH1DStackPads[th1d]->SetLogy(isLogY); //  set no logy on this pad
  fOutTH1DCanvases[th1d]->cd();          // Go back to the main canvas before saving
  StackPlots::CMSLumi(fOutTH1DCanvases[th1d],10); // write out Lumi info
  fOutTH1DCanvases[th1d]->SaveAs(Form("%s/%s/%s/%s_%s.%s",fOutDir.Data(),fSelection.Data(),fOutName.Data(),fTH1DNames[th1d].Data(),suffix.Data(),fOutType.Data()));
  fOutFile->cd();
  fOutTH1DCanvases[th1d]->Write(Form("%s_%s",fTH1DNames[th1d].Data(),suffix.Data()));
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

void StackPlots::OpenInputFiles() {
  // open input files into TFileVec --> data 
  fDataFiles.resize(fNData);
  for (UInt_t data = 0; data < fNData; data++) {
    TString datafile = Form("%s/%s/%s_data/plots.root",fOutDir.Data(),fSelection.Data(),fDataNames[data].Data());
    fDataFiles[data] = TFile::Open(datafile.Data());
    CheckValidFile(fDataFiles[data],datafile);
  }

  // open input files into TFileVec --> mc 
  fMCFiles.resize(fNMC);
  for (UInt_t mc = 0; mc < fNMC; mc++) {
    TString mcfile = Form("%s/%s/%s_MC/plots.root",fOutDir.Data(),fSelection.Data(),fMCNames[mc].Data());
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

void StackPlots::InitRatioLines() {
  // init ratio line used for comparisons

  // th1D hists
  fOutTH1DRatioLines.resize(fNTH1D);
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){
    fOutTH1DRatioLines[th1d] = new TLine();
  }
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

void StackPlots::InitUpperAxes() {
  // init ratio line used for comparisons

  // th1D hists
  fOutTH1DTGaxes.resize(fNTH1D);
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){
    fOutTH1DTGaxes[th1d] = new TGaxis();
  }
}

