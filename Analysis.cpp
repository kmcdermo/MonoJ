#include "Common.hh"
#include "Analysis.hh"

Analysis::Analysis(const TString sample, const Bool_t isMC, const Double_t lumi, const TString outType){
  // store in data members
  fSample = sample;
  fIsMC   = isMC;
  fLumi   = lumi;
  
  //Get File
  TString fileName = "root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/";
  if (fIsMC){ // MC
    fileName.Append("Spring15MC_50ns/");
    if (fSample.Contains("zmumu",TString::kExact)){ // z -> mm
      fileName.Append("zll/treewithwgt.root");
    }
    else if (fSample.Contains("ttbar",TString::kExact)){ // z -> mm
      fileName.Append("ttbar/treewithwgt.root");
    }
    else {
      std::cout << "Not a known sample: " << fSample << " isMC: " << fIsMC << " ...exiting..." << std::endl;
    }
  }
  else{ // Data
    fileName.Append("Data/");
    if (fSample.Contains("doublemu",TString::kExact)){ // z-> mm
      fileName.Append("doublemu/treewithwgt.root");
    }
    else {
      std::cout << "Not a known sample: " << fSample << " isMC: " << fIsMC << " ...exiting..." << std::endl;
    }
  }

  // Open the file + tree
  fInFile = TFile::Open(Form("%s",fileName.Data()));
  CheckValidFile(fInFile,fileName);
  fInTree = (TTree*)fInFile->Get("tree/tree");
  CheckValidTree(fInTree,"tree/tree",fileName);

  // set all the branch addresses appropriately
  Analysis::SetBranchAddresses();
  
  // set output variables and make the output directory if need be
  if (fIsMC) {
    fOutName = Form("%s_MC",fSample.Data());
  }
  else {
    fOutName = Form("%s_data",fSample.Data());
  }

  // make output directory
  MakeOutDirectory(fOutName);

  // make output root file
  fOutFile = new TFile(Form("%s/%s_plots.root",fOutName.Data(),fOutName.Data()),"RECREATE");

  // allow user to pick png, pdf, gif, etc
  fOutType = outType;

  // define color map here --> Set this here, and then make sure it is the same in the StackPlots
  fColorMap["zmumu"] = kCyan;
  fColorMap["ttbar"] = kRed+2;
}

Analysis::~Analysis(){
  delete fInTree;
  delete fInFile;
  Analysis::DeleteHists();
  delete fOutFile;
}

void Analysis::DoAnalysis(){
  // set up output plots to be produced
  Analysis::SetUpPlots();

  // keep track of nEvents that pass given selection selection 
  Double_t passed = 0;

  // Loop over entries in input tree
  for (UInt_t entry = 0; entry < fInTree->GetEntries(); entry++) {
    fInTree->GetEntry(entry);

    Double_t weight = 0;
    if (fIsMC) {
      weight = xsec * fLumi * wgt / wgtsum;  
    }
    else {
      weight = 1.0; // data is currently to 1.0
    }
    
    if ( (hltdoublemu > 0 || hltsinglemu > 0) && (mu1pt > 20) && (mu1id == 1) && (zmass < 120.) && (zmass > 60.) && (mu1pid == -mu2pid) ) { // plot valid zmass for dimuons
      // double plots
      
      if ( (fIsMC && flagcsctight == 1 && flaghbhenoise == 1) || (!fIsMC && cflagcsctight == 1 && cflaghbhenoise == 1) ){
	
	fTH1DMap["zmass"]->Fill(zmass,weight);
	fTH1DMap["zpt"]->Fill(zpt,weight);
	fTH1DMap["pfmet"]->Fill(pfmet,weight);
	
	fTH1DMap["signaljetpt"]->Fill(signaljetpt,weight);
	fTH1DMap["signaljeteta"]->Fill(signaljeteta,weight);
	fTH1DMap["signaljetCHfrac"]->Fill(signaljetCHfrac,weight);
	fTH1DMap["signaljetNHfrac"]->Fill(signaljetNHfrac,weight);
	fTH1DMap["signaljetEMfrac"]->Fill(signaljetEMfrac,weight);
	fTH1DMap["signaljetCEMfrac"]->Fill(signaljetCEMfrac,weight);
	
	// int plots
	fTH1IMap["njets"]->Fill(njets,weight);
	fTH1IMap["nvtx"]->Fill(nvtx,weight);
	
	passed += weight; // tiny counter to print yields
      } // extra selection over hbe noise filters
    }
  } // end loop over entries

  // save the histos once loop is over

  std::cout << "nEvents passed selection: " << passed << std::endl;
  Analysis::SaveHists();
}

void Analysis::SetUpPlots(){
  fTH1DMap["zmass"] = Analysis::MakeTH1DPlot("zmass","",60,60.,120.,"Dimuon Mass [GeV/c^{2}]","Events / GeV/c^{2}");
  fTH1DMap["zpt"]   = Analysis::MakeTH1DPlot("zpt","",20,0.,500.,"Dimuon p_{T} [GeV/c]","Events / 25 GeV/c"); 
  fTH1DMap["pfmet"] = Analysis::MakeTH1DPlot("pfmet","",50,0.,200.,"E_{T}^{Miss} [GeV]","Events / 4 GeV"); 

  fTH1DMap["signaljetpt"]      = Analysis::MakeTH1DPlot("signaljetpt","",40,0.,400.,"Leading Jet p_{T} [GeV/c]","Events / 10 GeV/c"); 
  fTH1DMap["signaljeteta"]     = Analysis::MakeTH1DPlot("signaljeteta","",50,-5.,5.,"Leading Jet #eta","Events"); 
  fTH1DMap["signaljetCHfrac"]  = Analysis::MakeTH1DPlot("signaljetCHfrac","",50,0.,1.,"Leading Jet CH Fraction","Events"); 
  fTH1DMap["signaljetNHfrac"]  = Analysis::MakeTH1DPlot("signaljetNHfrac","",50,0.,1.,"Leading Jet NH Fraction","Events"); 
  fTH1DMap["signaljetEMfrac"]  = Analysis::MakeTH1DPlot("signaljetEMfrac","",50,0.,1.,"Leading Jet Neutral EM Fraction","Events"); 
  fTH1DMap["signaljetCEMfrac"] = Analysis::MakeTH1DPlot("signaljetCEMfrac","",50,0.,1.,"Leading Jet Charged EM Fraction","Events"); 

  fTH1IMap["nvtx"]  = Analysis::MakeTH1IPlot("nvtx","",50,0,50,"Number of Primary Vertices","Events");
  fTH1IMap["njets"] = Analysis::MakeTH1IPlot("njets","",50,0,50,"Jet Multiplicity","Events");
}

TH1D * Analysis::MakeTH1DPlot(const TString hname, const TString htitle, const Int_t nbins, const Double_t xlow, const Double_t xhigh, const TString xtitle, const TString ytitle){
  TH1D * hist = new TH1D(hname.Data(),htitle.Data(),nbins,xlow,xhigh);
  hist->GetXaxis()->SetTitle(xtitle.Data());
  hist->GetYaxis()->SetTitle(ytitle.Data());
  hist->Sumw2();
  return hist;
}

TH1I * Analysis::MakeTH1IPlot(const TString hname, const TString htitle, const Int_t nbins, const Int_t xlow, const Int_t xhigh, const TString xtitle, const TString ytitle){
  TH1I * hist = new TH1I(hname.Data(),htitle.Data(),nbins,xlow,xhigh);
  hist->GetXaxis()->SetTitle(xtitle.Data());
  hist->GetYaxis()->SetTitle(ytitle.Data());
  hist->Sumw2();
  return hist;
}

void Analysis::SaveHists() {
  fOutFile->cd();

  TCanvas * canv = new TCanvas();
  for (TH1DMapIter mapiter = fTH1DMap.begin(); mapiter != fTH1DMap.end(); mapiter++) { // first do double hists ... could template this...
    (*mapiter).second->Write(); // map is map["hist name",TH1*]

    canv->cd();
    if (fIsMC){
      (*mapiter).second->SetLineColor(fColorMap[fSample]);
      (*mapiter).second->SetFillColor(fColorMap[fSample]);
      (*mapiter).second->Draw("HIST");
    }
    else {
      (*mapiter).second->Draw("PE");
    }

    // first save as log, then linear
    canv->SetLogy(1);
    canv->SaveAs(Form("%s/%s_log.%s",fOutName.Data(),(*mapiter).first.Data(),fOutType.Data()));

    canv->SetLogy(0);
    canv->SaveAs(Form("%s/%s_lin.%s",fOutName.Data(),(*mapiter).first.Data(),fOutType.Data()));
  }
  
  for (TH1IMapIter mapiter = fTH1IMap.begin(); mapiter != fTH1IMap.end(); mapiter++) {
    (*mapiter).second->Write(); // map is map["hist name",TH1*]

    canv->cd();

    if (fIsMC){
      (*mapiter).second->SetLineColor(fColorMap[fSample]);
      (*mapiter).second->SetFillColor(fColorMap[fSample]);
      (*mapiter).second->Draw("HIST");
    }
    else {
      (*mapiter).second->Draw("PE");
    }

    // first save as log, then linear
    canv->SetLogy(1);
    canv->SaveAs(Form("%s/%s_log.%s",fOutName.Data(),(*mapiter).first.Data(),fOutType.Data()));
   
    canv->SetLogy(0);
    canv->SaveAs(Form("%s/%s_lin.%s",fOutName.Data(),(*mapiter).first.Data(),fOutType.Data()));
  }
  delete canv;

  fOutFile->Write();
}

void Analysis::DeleteHists() {
  for (TH1DMapIter mapiter = fTH1DMap.begin(); mapiter != fTH1DMap.end(); mapiter++) {
    delete ((*mapiter).second);
  }
  fTH1DMap.clear();

  for (TH1IMapIter mapiter = fTH1IMap.begin(); mapiter != fTH1IMap.end(); mapiter++) {
    delete ((*mapiter).second);
  }
  fTH1IMap.clear();
}
