#include "Common.hh"
#include "Analysis.hh"

Analysis::Analysis(const TString sample, const Bool_t isMC, const TString outType){
  // store in data members
  fSample = sample;
  fIsMC   = isMC;
  
  //Get File
  TString fileName;
  if (fIsMC){ // MC
    if (fSample.Contains("zmumu",TString::kExact)){ // z -> mm
      fileName = "root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/zll/treewithwgt.root";
    }
    else if (fSample.Contains("ttbar",TString::kExact)){ // z -> mm
      fileName = "root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/ttbar/treewithwgt.root";
    }
    else {
      std::cout << "Not a known sample: " << fSample << " isMC: " << fIsMC << " ...exiting..." << std::endl;
    }
  }
  else{ // Data
    if (fSample.Contains("doublemu",TString::kExact)){ // z-> mm
      fileName = "root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Data/doublemu/treewithwgt.root";
    }
    else {
      std::cout << "Not a known sample: " << fSample << " isMC: " << fIsMC << " ...exiting..." << std::endl;
    }
  }

  // Open the file + tree
  fInFile = TFile::Open(Form("%s",fileName.Data()));
  fInTree = (TTree*)fInFile->Get("tree/tree");

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

  // Loop over entries in input tree
  for (UInt_t entry = 0; entry < fInTree->GetEntries(); entry++) {
    fInTree->GetEntry(entry);

    float weight = 0;
    if (fIsMC) {
      weight = xsec * 0.005618 * wgt / wgtsum;  // current data int lumi = 56pb^-1 
    }
    else {
      weight = 1.0; // data is currently to 1.0
    }
    
    if ((hltdoublemu > 0 || hltsinglemu > 0) && mu1pt > 20 && mu1id == 1) { // plot valid zmass for dimuons
      // double plots
      
      fTH1DMap["zmass"].first->Fill(zmass,weight);
      fTH1DMap["zpt"].first->Fill(zpt,weight);
      fTH1DMap["pfmet"].first->Fill(pfmet,weight);

      fTH1DMap["signaljetpt"].first->Fill(signaljetpt,weight);
      fTH1DMap["signaljeteta"].first->Fill(signaljeteta,weight);
      fTH1DMap["signaljetCHfrac"].first->Fill(signaljetCHfrac,weight);
      fTH1DMap["signaljetNHfrac"].first->Fill(signaljetNHfrac,weight);
      fTH1DMap["signaljetEMfrac"].first->Fill(signaljetEMfrac,weight);
      fTH1DMap["signaljetCEMfrac"].first->Fill(signaljetCEMfrac,weight);

      // int plots
      fTH1IMap["njets"].first->Fill(njets,weight);
      fTH1IMap["nvtx"].first->Fill(nvtx,weight);
    }
    
  } // end loop over entries

  // save the histos once loop is over
  Analysis::SaveHists();

  // would like to put in destructor ... but can't delete objects until the end of the macro running!  --> can think of something clever later
}

void Analysis::SetUpPlots(){
  fTH1DMap["zmass"] = Analysis::MakeTH1DPlot("zmass","",60,60.,120.,"Dimuon Mass [GeV/c^{2}]","Events / GeV/c^{2}",false);
  fTH1DMap["zpt"]   = Analysis::MakeTH1DPlot("zpt","",20,0.,500.,"Dimuon p_{T} [GeV/c]","Events / 25 GeV/c",true); 
  fTH1DMap["pfmet"] = Analysis::MakeTH1DPlot("pfmet","",50,0.,200.,"E_{T}^{Miss} [GeV]","Events / 4 GeV",true); 

  fTH1DMap["signaljetpt"] = Analysis::MakeTH1DPlot("signaljetpt","",40,0.,400.,"Leading Jet p_{T} [GeV/c]","Events / 10 GeV/c",true); 
  fTH1DMap["signaljeteta"] = Analysis::MakeTH1DPlot("signaljeteta","",50,-5.,5.,"Leading Jet #eta","Events",false); 
  fTH1DMap["signaljetCHfrac"] = Analysis::MakeTH1DPlot("signaljetCHfrac","",50,0.,1.,"Leading Jet CH Fraction","Events",true); 
  fTH1DMap["signaljetNHfrac"] = Analysis::MakeTH1DPlot("signaljetNHfrac","",50,0.,1.,"Leading Jet NH Fraction","Events",true); 
  fTH1DMap["signaljetEMfrac"] = Analysis::MakeTH1DPlot("signaljetEMfrac","",50,0.,1.,"Leading Jet Neutral EM Fraction","Events",true); 
  fTH1DMap["signaljetCEMfrac"] = Analysis::MakeTH1DPlot("signaljetCEMfrac","",50,0.,1.,"Leading Jet Charged EM Fraction","Events",true); 

  fTH1IMap["nvtx"]  = Analysis::MakeTH1IPlot("nvtx","",50,0,50,"Number of Primary Vertices","Events",false);
  fTH1IMap["njets"] = Analysis::MakeTH1IPlot("njets","",50,0,50,"Jet Multiplicity","Events",true);
}

TH1DBoolPair Analysis::MakeTH1DPlot(const TString hname, const TString htitle, const Int_t nbins, const Double_t xlow, const Double_t xhigh, const TString xtitle, const TString ytitle, const Bool_t logy){
  TH1D * hist = new TH1D(hname.Data(),htitle.Data(),nbins,xlow,xhigh);
  hist->GetXaxis()->SetTitle(xtitle.Data());
  hist->GetYaxis()->SetTitle(ytitle.Data());
  hist->Sumw2();
  return std::make_pair(hist,logy);
}

TH1IBoolPair Analysis::MakeTH1IPlot(const TString hname, const TString htitle, const Int_t nbins, const Int_t xlow, const Int_t xhigh, const TString xtitle, const TString ytitle, const Bool_t logy){
  TH1I * hist = new TH1I(hname.Data(),htitle.Data(),nbins,xlow,xhigh);
  hist->GetXaxis()->SetTitle(xtitle.Data());
  hist->GetYaxis()->SetTitle(ytitle.Data());
  hist->Sumw2();
  return std::make_pair(hist,logy);
}

void Analysis::SaveHists() {
  fOutFile->cd();

  TCanvas * canv = new TCanvas();
  for (TH1DMapIter mapiter = fTH1DMap.begin(); mapiter != fTH1DMap.end(); mapiter++) { // first do double hists ... could template this...
    (*mapiter).second.first->Write(); // map is map["hist name",pair<TH1*,bool>]

    canv->cd();
    canv->SetLogy((*mapiter).second.second);

    if (fIsMC){
      (*mapiter).second.first->SetLineColor(fColorMap[fSample]);
      (*mapiter).second.first->SetFillColor(fColorMap[fSample]);
      (*mapiter).second.first->Draw("HIST");
    }
    else {
      (*mapiter).second.first->Draw("PE");
    }

    canv->SaveAs(Form("%s/%s.%s",fOutName.Data(),(*mapiter).first.Data(),fOutType.Data()));
  }
  
  for (TH1IMapIter mapiter = fTH1IMap.begin(); mapiter != fTH1IMap.end(); mapiter++) {
    (*mapiter).second.first->Write(); // map is map["hist name",pair<TH1*,bool>]

    canv->cd();
    canv->SetLogy((*mapiter).second.second);

    if (fIsMC){
      (*mapiter).second.first->SetLineColor(fColorMap[fSample]);
      (*mapiter).second.first->SetFillColor(fColorMap[fSample]);
      (*mapiter).second.first->Draw("HIST");
    }
    else {
      (*mapiter).second.first->Draw("PE");
    }

    canv->SaveAs(Form("%s/%s.%s",fOutName.Data(),(*mapiter).first.Data(),fOutType.Data()));
  }
  delete canv;

  fOutFile->Write();
}

void Analysis::DeleteHists() {
  for (TH1DMapIter mapiter = fTH1DMap.begin(); mapiter != fTH1DMap.end(); mapiter++) {
    delete ((*mapiter).second.first); // map is map["hist name",pair<TH1*,bool>]
  }
  fTH1DMap.clear();

  for (TH1IMapIter mapiter = fTH1IMap.begin(); mapiter != fTH1IMap.end(); mapiter++) {
    delete ((*mapiter).second.first); // map is map["hist name",pair<TH1*,bool>]
  }
  fTH1IMap.clear();
}
