#include "../interface/StackPlots.hh"

StackPlots::StackPlots(SamplePairVec Samples, const TString selection, const Int_t njetsselection, const Double_t lumi, const TString outdir, const ColorMap & colorMap, const TString outtype){
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
  fNJetsSeln = njetsselection;

  fNJetsStr = "";
  if (fNJetsSeln != -1){
    fNJetsStr = Form("_nj%i",fNJetsSeln);
  }

  // save lumi
  fLumi = lumi;

  // use the plots already stored in Analysis.cpp ... use selection to decide what to loop over --> doubles + set th1d subdir names
  StackPlots::InitTH1DNamesAndSubDNames();
  
  // store this too
  fNTH1D = fTH1DNames.size();

  // output data members
  fOutDir  = outdir;
  fOutName = "stackedplots"; // where to put output stack plots 
  MakeOutDirectory(Form("%s/%s%s/%s",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data())); // make output directory 
  MakeOutDirectory(Form("%s/%s%s/%s/Photons",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Leptons",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Leptons/Muons",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Leptons/Electrons",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Leptons/Dileptons",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Jets",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Jets/Leading",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Jets/Subleading",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Jets/Subsubleading",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Jets/FatJets",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/MET",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));

  fOutFile = new TFile(Form("%s/%s%s/%s/stackplots_canvases.root",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()),"RECREATE"); // make output tfile --> store canvas images here too, for quick editting
  fOutType = outtype; // allow user to pick png, pdf, gif, etc for stacked plots

  // define color map + title map
  fSampleTitleMap["zll"]     = "Z #rightarrow l^{+}l^{-}";
  fSampleTitleMap["wln"]     = "W #rightarrow l#nu";
  fSampleTitleMap["diboson"] = "Dibosons";
  fSampleTitleMap["top"]     = "Top";
  fSampleTitleMap["qcd"]     = "QCD";
  fSampleTitleMap["gamma"]   = "#gamma + Jets";

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

void StackPlots::DoStacks(std::ofstream & yields) {
  StackPlots::MakeStackPlots(yields);
  StackPlots::MakeRatioPlots();
  StackPlots::MakeOutputCanvas();
}

void StackPlots::MakeStackPlots(std::ofstream & yields){
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

      if (fTH1DNames[th1d].Contains("nvtx",TString::kExact)) { // save individual yields for data using nvtx, as only the selection on this plot is event selection
	yields << fDataNames[data].Data() << ": " << fInDataTH1DHists[th1d][data]->Integral() << std::endl;
      }
    } // end loop over data samples

    // add legend entry for total data
    fTH1DLegends[th1d]->AddEntry(fOutDataTH1DHists[th1d],"Data","pl"); // add data entry to legend

    // add total yield for data here if nvtx plot
    if (fTH1DNames[th1d].Contains("nvtx",TString::kExact)) { // save total yields for data
      yields << "--------------------" << std::endl;
      yields << "Data Total: " << fOutDataTH1DHists[th1d]->Integral() << std::endl << std::endl;
    }

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
      fTH1DLegends[th1d]->AddEntry(fInMCTH1DHists[th1d][mc],fSampleTitleMap[fMCNames[mc]],"f");

      if (fTH1DNames[th1d].Contains("nvtx",TString::kExact)) { // save individual contributions for yields for MC
	yields << fMCNames[mc].Data() << ": " << fInMCTH1DHists[th1d][mc]->Integral() << std::endl;
      }
    } // end loop over mc samples

    // will use the output MC added Hists for plotting uncertainties and add it to legend once
    fOutMCTH1DHists[th1d]->SetMarkerSize(0);
    fOutMCTH1DHists[th1d]->SetFillStyle(3254);
    fOutMCTH1DHists[th1d]->SetFillColor(kGray+3);
    fTH1DLegends[th1d]->AddEntry(fOutMCTH1DHists[th1d],"MC Unc.","f");
  
    // add total yield for MC here if nvtx plot
    if (fTH1DNames[th1d].Contains("nvtx",TString::kExact)) { // save total yields for MC
      yields << "--------------------" << std::endl;
      yields << "MC Total: " << fOutMCTH1DHists[th1d]->Integral() << std::endl;
    }
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
  Double_t min = StackPlots::GetMinimum(th1d);
  Double_t max = StackPlots::GetMaximum(th1d);

  if (isLogY) { // set min for log only... maybe consider min for linear eventually
    // only go to 20th of event to absolute min
    //    if (min < 0.05) {
    //      fOutDataTH1DHists[th1d]->SetMinimum(0.05);
    //    }
    //    else {
    fOutDataTH1DHists[th1d]->SetMinimum(0.08);
      //    }

    // set max with 2.0 scale to give enough space 
    fOutDataTH1DHists[th1d]->SetMaximum(max*1.5);
  }
  else {
    if (max > 0){
      fOutDataTH1DHists[th1d]->SetMaximum(max*1.05);      
    }
    else {
      fOutDataTH1DHists[th1d]->SetMaximum(1.0);      
    }
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

  //Draw MC sum total error as well on top of stack --> E2 makes error appear as rectangle
  fOutMCTH1DHists[th1d]->Draw("E2 SAME");

  // redraw data (ROOT IS SO DUMBBBBBB)
  fOutDataTH1DHists[th1d]->Draw("PE SAME"); 
  fTH1DLegends[th1d]->Draw("SAME"); // make sure to include the legend!
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

Double_t StackPlots::GetMinimum(const UInt_t th1d) {
  // need to loop through to check bin != 0
  Double_t datamin  = 1e9;
  Bool_t newdatamin = false;
  for (Int_t bin = 1; bin <= fOutDataTH1DHists[th1d]->GetNbinsX(); bin++){
    Float_t tmpmin = fOutDataTH1DHists[th1d]->GetBinContent(bin);
    if ((tmpmin < datamin) && (tmpmin > 0)) {
      datamin    = tmpmin;
      newdatamin = true;
    }
  }

  Double_t mcmin  = 1e9;
  Bool_t newmcmin = false;
  for (UInt_t mc = 0; mc < fNMC; mc++) {
    for (Int_t bin = 1; bin <= fInMCTH1DHists[th1d][mc]->GetNbinsX(); bin++){
      Float_t tmpmin = fInMCTH1DHists[th1d][mc]->GetBinContent(bin);
      if ((tmpmin < mcmin) && (tmpmin > 0)) {
	mcmin    = tmpmin;
	newmcmin = true;
      }
    }
  }
  
  // now set return variable min
  Double_t min = 1; // to not royally mess up logy plots in case where plots have no fill and no new min is set for data or mc
  if (newdatamin || newmcmin) {
    if (datamin < mcmin) {
      min = datamin;
    }
    else {
      min = mcmin;
    }
  }
  return min;
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
  fOutRatioTH1DHists[th1d]->GetYaxis()->SetTitleOffset(0.4); 

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
  fOutTH1DStackPads[th1d]->SetLogy(isLogY); //  set logy on this pad
  if (fTH1DNames[th1d].Contains("signaljetCHfrac")) {
    if (isLogY) {
      fTH1DLegends[th1d]->SetX1(0.182);
      fTH1DLegends[th1d]->SetY1(0.725);
      fTH1DLegends[th1d]->SetX2(0.32);
      fTH1DLegends[th1d]->SetY2(0.947);
    }
  }

  fOutTH1DCanvases[th1d]->cd();          // Go back to the main canvas before saving
  StackPlots::CMSLumi(fOutTH1DCanvases[th1d],0); // write out Lumi info
  fOutTH1DCanvases[th1d]->SaveAs(Form("%s/%s%s/%s/%s%s_%s.%s",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data(),fTH1DSubDMap[fTH1DNames[th1d]].Data(),fTH1DNames[th1d].Data(),suffix.Data(),fOutType.Data()));
  fOutFile->cd();
  fOutTH1DCanvases[th1d]->Write(Form("%s_%s",fTH1DNames[th1d].Data(),suffix.Data()));
}

void StackPlots::CMSLumi(TCanvas *& canv, const Int_t iPosX) { // borrowed from margaret
  TString cmsText      = "CMS";
  Double_t cmsTextFont = 61;  // default is helvetic-bold
  
  Bool_t writeExtraText  = true;
  TString extraText      = "Preliminary";
  Double_t extraTextFont = 52;  // default is helvetica-italics

  TString lumiText = Form("#sqrt{s} = 13 TeV, L = %2.2f pb^{-1}", fLumi*1000); // must change this spec once we are in fb range!
  
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

void StackPlots::InitTH1DNamesAndSubDNames(){
  // will use the integral of nvtx to derive total yields as no additional cuts are placed on ntvx --> key on name for yields
  fTH1DNames.push_back("nvtx");
  fTH1DSubDMap["nvtx"] = "";

  // photon plots
  fTH1DNames.push_back("nphotons");
  fTH1DNames.push_back("phpt");
  fTH1DNames.push_back("phpt_nj_lte2");
  fTH1DSubDMap["nphotons"]     = "Photons/";
  fTH1DSubDMap["phpt"]         = "Photons/";
  fTH1DSubDMap["phpt_nj_lte2"] = "Photons/";

  // lepton plots
  fTH1DNames.push_back("nelectrons");
  fTH1DNames.push_back("ntightelectrons");
  fTH1DNames.push_back("nmuons");
  fTH1DNames.push_back("ntightmuons");
  fTH1DNames.push_back("ntaus");
  fTH1DSubDMap["nelectrons"]      = "Leptons/";
  fTH1DSubDMap["ntightelectrons"] = "Leptons/";
  fTH1DSubDMap["nmuons"]          = "Leptons/";
  fTH1DSubDMap["ntightmuons"]     = "Leptons/";
  fTH1DSubDMap["ntaus"]           = "Leptons/";

  // muons plots
  fTH1DNames.push_back("mu1eta");
  fTH1DNames.push_back("mu1phi");
  fTH1DNames.push_back("mu1pt");
  fTH1DNames.push_back("mu2eta");
  fTH1DNames.push_back("mu2phi");
  fTH1DNames.push_back("mu2pt");
  fTH1DSubDMap["mu1eta"] = "Leptons/Muons/";
  fTH1DSubDMap["mu1phi"] = "Leptons/Muons/";
  fTH1DSubDMap["mu1pt"]  = "Leptons/Muons/";
  fTH1DSubDMap["mu2eta"] = "Leptons/Muons/";
  fTH1DSubDMap["mu2phi"] = "Leptons/Muons/";
  fTH1DSubDMap["mu2pt"]  = "Leptons/Muons/";

  fTH1DNames.push_back("wmt");
  fTH1DSubDMap["wmt"] = "Leptons/Muons/";

  // electron plots
  fTH1DNames.push_back("el1eta");
  fTH1DNames.push_back("el1phi");
  fTH1DNames.push_back("el1pt");
  fTH1DNames.push_back("el2eta");
  fTH1DNames.push_back("el2phi");
  fTH1DNames.push_back("el2pt");
  fTH1DSubDMap["el1eta"] = "Leptons/Electrons/";
  fTH1DSubDMap["el1phi"] = "Leptons/Electrons/";
  fTH1DSubDMap["el1pt"]  = "Leptons/Electrons/";
  fTH1DSubDMap["el2eta"] = "Leptons/Electrons/";
  fTH1DSubDMap["el2phi"] = "Leptons/Electrons/";
  fTH1DSubDMap["el2pt"]  = "Leptons/Electrons/";

  fTH1DNames.push_back("wemt");
  fTH1DSubDMap["wemt"]  = "Leptons/Electrons/";

  // dilepton plots
  fTH1DNames.push_back("zeta");
  fTH1DNames.push_back("zmass");
  fTH1DNames.push_back("zphi");
  fTH1DNames.push_back("zpt");
  fTH1DSubDMap["zeta"]  = "Leptons/Dileptons/";
  fTH1DSubDMap["zmass"] = "Leptons/Dileptons/";
  fTH1DSubDMap["zphi"]  = "Leptons/Dileptons/";
  fTH1DSubDMap["zpt"]   = "Leptons/Dileptons/";

  fTH1DNames.push_back("zeeeta");
  fTH1DNames.push_back("zeemass");
  fTH1DNames.push_back("zeephi");
  fTH1DNames.push_back("zeept");
  fTH1DSubDMap["zeeeta"]  = "Leptons/Dileptons/";
  fTH1DSubDMap["zeemass"] = "Leptons/Dileptons/";
  fTH1DSubDMap["zeephi"]  = "Leptons/Dileptons/";
  fTH1DSubDMap["zeept"]   = "Leptons/Dileptons/";

  fTH1DNames.push_back("emueta");
  fTH1DNames.push_back("emumass");
  fTH1DNames.push_back("emuphi");
  fTH1DNames.push_back("emupt");
  fTH1DSubDMap["emueta"]  = "Leptons/Dileptons/";
  fTH1DSubDMap["emumass"] = "Leptons/Dileptons/";
  fTH1DSubDMap["emuphi"]  = "Leptons/Dileptons/";
  fTH1DSubDMap["emupt"]   = "Leptons/Dileptons/";

  // Jet plots
  fTH1DNames.push_back("njets");
  fTH1DNames.push_back("nbjets");
  fTH1DNames.push_back("nfatjets");
  fTH1DNames.push_back("ht");
  fTH1DSubDMap["njets"]    = "Jets/";
  fTH1DSubDMap["nbjets"]   = "Jets/";
  fTH1DSubDMap["nfatjets"] = "Jets/";
  fTH1DSubDMap["ht"]       = "Jets/";

  fTH1DNames.push_back("jetmetdphimin");
  fTH1DNames.push_back("incjetmetdphimin");
  fTH1DNames.push_back("jetjetdphi");
  fTH1DSubDMap["jetmetdphimin"]    = "Jets/";
  fTH1DSubDMap["incjetmetdphimin"] = "Jets/";
  fTH1DSubDMap["jetjetdphi"]       = "Jets/";

  // Leading jet plots
  fTH1DNames.push_back("signaljeteta");
  fTH1DNames.push_back("signaljetphi");
  fTH1DNames.push_back("signaljetpt");
  fTH1DNames.push_back("signaljetCHfrac");
  fTH1DNames.push_back("signaljetNHfrac");
  fTH1DNames.push_back("signaljetEMfrac");
  fTH1DNames.push_back("signaljetCEMfrac");
  fTH1DNames.push_back("signaljetmetdphi");
  fTH1DSubDMap["signaljeteta"]     = "Jets/Leading/";
  fTH1DSubDMap["signaljetphi"]     = "Jets/Leading/";
  fTH1DSubDMap["signaljetpt"]      = "Jets/Leading/";
  fTH1DSubDMap["signaljetCHfrac"]  = "Jets/Leading/";
  fTH1DSubDMap["signaljetNHfrac"]  = "Jets/Leading/";
  fTH1DSubDMap["signaljetEMfrac"]  = "Jets/Leading/";
  fTH1DSubDMap["signaljetCEMfrac"] = "Jets/Leading/";
  fTH1DSubDMap["signaljetmetdphi"] = "Jets/Leading/";

  // subleading jet plots
  fTH1DNames.push_back("secondjeteta");
  fTH1DNames.push_back("secondjetphi");
  fTH1DNames.push_back("secondjetpt");
  fTH1DNames.push_back("secondjetCHfrac");
  fTH1DNames.push_back("secondjetNHfrac");
  fTH1DNames.push_back("secondjetEMfrac");
  fTH1DNames.push_back("secondjetCEMfrac");
  fTH1DNames.push_back("secondjetmetdphi");
  fTH1DSubDMap["secondjeteta"]     = "Jets/Subleading/";
  fTH1DSubDMap["secondjetphi"]     = "Jets/Subleading/";
  fTH1DSubDMap["secondjetpt"]      = "Jets/Subleading/";
  fTH1DSubDMap["secondjetCHfrac"]  = "Jets/Subleading/";
  fTH1DSubDMap["secondjetNHfrac"]  = "Jets/Subleading/";
  fTH1DSubDMap["secondjetEMfrac"]  = "Jets/Subleading/";
  fTH1DSubDMap["secondjetCEMfrac"] = "Jets/Subleading/";
  fTH1DSubDMap["secondjetmetdphi"] = "Jets/Subleading/";

  // subsubleading jet plots
  fTH1DNames.push_back("thirdjeteta");
  fTH1DNames.push_back("thirdjetphi");
  fTH1DNames.push_back("thirdjetpt");
  fTH1DNames.push_back("thirdjetCHfrac");
  fTH1DNames.push_back("thirdjetNHfrac");
  fTH1DNames.push_back("thirdjetEMfrac");
  fTH1DNames.push_back("thirdjetCEMfrac");
  fTH1DNames.push_back("thirdjetmetdphi");
  fTH1DSubDMap["thirdjeteta"]     = "Jets/Subsubleading/";
  fTH1DSubDMap["thirdjetphi"]     = "Jets/Subsubleading/";
  fTH1DSubDMap["thirdjetpt"]      = "Jets/Subsubleading/";
  fTH1DSubDMap["thirdjetCHfrac"]  = "Jets/Subsubleading/";
  fTH1DSubDMap["thirdjetNHfrac"]  = "Jets/Subsubleading/";
  fTH1DSubDMap["thirdjetEMfrac"]  = "Jets/Subsubleading/";
  fTH1DSubDMap["thirdjetCEMfrac"] = "Jets/Subsubleading/";
  fTH1DSubDMap["thirdjetmetdphi"] = "Jets/Subsubleading/";

  fTH1DNames.push_back("fatjeteta");
  fTH1DNames.push_back("fatjetphi");
  fTH1DNames.push_back("fatjetpt"); 
  fTH1DNames.push_back("fatjetCHfrac"); 
  fTH1DNames.push_back("fatjetNHfrac"); 
  fTH1DNames.push_back("fatjetEMfrac"); 
  fTH1DNames.push_back("fatjetCEMfrac");
  fTH1DNames.push_back("fatjetmetdphi");
  fTH1DNames.push_back("fatjetprmass"); 
  fTH1DNames.push_back("fatjetsdmass"); 
  fTH1DNames.push_back("fatjettrmass"); 
  fTH1DNames.push_back("fatjetftmass"); 
  fTH1DSubDMap["fatjeteta"]     = "Jets/FatJets/";
  fTH1DSubDMap["fatjetphi"]     = "Jets/FatJets/";
  fTH1DSubDMap["fatjetpt"]      = "Jets/FatJets/";
  fTH1DSubDMap["fatjetCHfrac"]  = "Jets/FatJets/";
  fTH1DSubDMap["fatjetNHfrac"]  = "Jets/FatJets/";
  fTH1DSubDMap["fatjetEMfrac"]  = "Jets/FatJets/";
  fTH1DSubDMap["fatjetCEMfrac"] = "Jets/FatJets/";
  fTH1DSubDMap["fatjetmetdphi"] = "Jets/FatJets/";
  fTH1DSubDMap["fatjetprmass"]  = "Jets/FatJets/";
  fTH1DSubDMap["fatjetsdmass"]  = "Jets/FatJets/";
  fTH1DSubDMap["fatjettrmass"]  = "Jets/FatJets/";
  fTH1DSubDMap["fatjetftmass"]  = "Jets/FatJets/";

  // MET plots
  fTH1DNames.push_back("pfmet");
  fTH1DNames.push_back("t1pfmet");
  fTH1DNames.push_back("mumet");
  fTH1DNames.push_back("t1mumet");
  fTH1DNames.push_back("mht");
  fTH1DSubDMap["pfmet"]   = "MET/";
  fTH1DSubDMap["t1pfmet"] = "MET/";
  fTH1DSubDMap["mumet"]   = "MET/";
  fTH1DSubDMap["t1mumet"] = "MET/";
  fTH1DSubDMap["mht"]     = "MET/";

  fTH1DNames.push_back("pfmetphi");
  fTH1DNames.push_back("t1pfmetphi");
  fTH1DNames.push_back("mumetphi");
  fTH1DNames.push_back("t1mumetphi");
  fTH1DSubDMap["pfmetphi"]   = "MET/";
  fTH1DSubDMap["t1pfmetphi"] = "MET/";
  fTH1DSubDMap["mumetphi"]   = "MET/";
  fTH1DSubDMap["t1mumetphi"] = "MET/";
}

void StackPlots::OpenInputFiles() {
  // open input files into TFileVec --> data 
  fDataFiles.resize(fNData);
  for (UInt_t data = 0; data < fNData; data++) {
    TString datafile = Form("%s/%s%s/%s_data/plots.root",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fDataNames[data].Data());
    fDataFiles[data] = TFile::Open(datafile.Data());
    CheckValidFile(fDataFiles[data],datafile);
  }

  // open input files into TFileVec --> mc 
  fMCFiles.resize(fNMC);
  for (UInt_t mc = 0; mc < fNMC; mc++) {
    TString mcfile = Form("%s/%s%s/%s_MC/plots.root",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fMCNames[mc].Data());
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
      fInMCTH1DHists[th1d][mc]->SetLineColor(kBlack);
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
    fOutMCTH1DStacks[th1d] = new THStack(Form("%s_stack",fTH1DNames[th1d].Data()),"");
  }
}

void StackPlots::InitOutputLegends() {
  // init legends

  // th1D hists
  fTH1DLegends.resize(fNTH1D);
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){
    fTH1DLegends[th1d] = new TLegend(0.682,0.7,0.825,0.92);
    fTH1DLegends[th1d]->SetBorderSize(4);
    fTH1DLegends[th1d]->SetLineColor(kBlack);
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
    fOutTH1DCanvases[th1d] = new TCanvas(Form("%s_canv",fTH1DNames[th1d].Data()),"");
    fOutTH1DCanvases[th1d]->cd();
    
    fOutTH1DStackPads[th1d] = new TPad(Form("%s_upad",fTH1DNames[th1d].Data()),"", 0, 0.3, 1.0, 0.99);
    fOutTH1DStackPads[th1d]->SetBottomMargin(0); // Upper and lower plot are joined
    
    fOutTH1DRatioPads[th1d] = new TPad(Form("%s_lpad",fTH1DNames[th1d].Data()), "", 0, 0.05, 1.0, 0.3);
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

