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
  }
  else{ // Data
    if (fSample.Contains("zmumu",TString::kExact)){ // z-> mm
      fileName = "root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Run2012B/DoubleMuon/tree.root";
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

  // make output directory --> same name as outname
  FileStat_t dummyFileStat;
  if (gSystem->GetPathInfo(fOutName.Data(), dummyFileStat) == 1){
    TString mkDir = "mkdir -p ";
    mkDir += fOutName.Data();
    gSystem->Exec(mkDir.Data());
  }

  // make output root file
  fOutFile = new TFile(Form("%s/%s_plots.root",fOutName.Data(),fOutName.Data()),"RECREATE");

  // allow user to pick png, pdf, gif, etc
  fOutType = outType;

  // define color map here
  fColorMap["zmumu"] = kCyan;
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
      //      weight = xsec * 0.005618 * wgt / weightsum;  // current data int lumi = 56pb^-1 // can we get wgtsum in data as well?
    }
    else {
      weight = 1.0; // data is currently to 1.0
    }
    
    if ((hltdoublemu > 0 || hltsinglemu > 0) && mu1pt > 20 && mu1id == 1) { // plot valid zmass for dimuons
      // double plots
      
      fTH1DMap["zmass"].first->Fill(zmass,weight);
      fTH1DMap["zpt"].first->Fill(zpt,weight);
      fTH1DMap["mht"].first->Fill(mht,weight);

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
  fTH1DMap["mht"]   = Analysis::MakeTH1DPlot("mht","",50,0.,200.,"E_{T}^{Miss} [GeV]","Events / 4 GeV",true); 

  fTH1DMap["signaljetpt"] = Analysis::MakeTH1DPlot("signaljetpt","",40,0.,400.,"Leading Jet p_{T} [GeV/c]","Events / 10 GeV/c",true); 
  fTH1DMap["signaljeteta"] = Analysis::MakeTH1DPlot("signaljetpt","",50,-5.,5.,"Leading Jet #eta","Events",false); 
  fTH1DMap["signaljetCHfrac"] = Analysis::MakeTH1DPlot("signaljetCHfrac","",50,0.,1.,"Leading Jet CH Fraction","Events",false); 
  fTH1DMap["signaljetNHfrac"] = Analysis::MakeTH1DPlot("signaljetNHfrac","",50,0.,1.,"Leading Jet NH Fraction","Events",false); 
  fTH1DMap["signaljetEMfrac"] = Analysis::MakeTH1DPlot("signaljetEMfrac","",50,0.,1.,"Leading Jet Neutral EM Fraction","Events",false); 
  fTH1DMap["signaljetCEMfrac"] = Analysis::MakeTH1DPlot("signaljetCEMfrac","",50,0.,1.,"Leading Jet Charged EM Fraction","Events",false); 

  fTH1IMap["nvtx"]  = Analysis::MakeTH1IPlot("nvtx","",50,0,50,"Number of Primary Vertices","Events",false);
  fTH1IMap["njets"] = Analysis::MakeTH1IPlot("njets","",50,0,50,"Jet Multiplicity","Events",true);
}

void Analysis::SetBranchAddresses(){
   fInTree->SetBranchAddress("event", &event, &b_event);
   fInTree->SetBranchAddress("run", &run, &b_run);
   fInTree->SetBranchAddress("lumi", &lumi, &b_lumi);
   fInTree->SetBranchAddress("wgt", &wgt, &b_wgt);
   fInTree->SetBranchAddress("puwgt", &puwgt, &b_puwgt);
   fInTree->SetBranchAddress("xsec", &xsec, &xsec);
   //   fInTree->SetBranchAddress("weight", &weight, &b_weight);
   fInTree->SetBranchAddress("puobs", &puobs, &b_puobs);
   fInTree->SetBranchAddress("putrue", &putrue, &b_putrue);
   fInTree->SetBranchAddress("nvtx", &nvtx, &b_nvtx);
   fInTree->SetBranchAddress("hltmet120", &hltmet120, &b_hltmet120);
   //   fInTree->SetBranchAddress("hltmet95jet80", &hltmet95jet80, &b_hltmet95jet80);
   //   fInTree->SetBranchAddress("hltmet105jet80", &hltmet105jet80, &b_hltmet105jet80);
   fInTree->SetBranchAddress("hltdoublemu", &hltdoublemu, &b_hltdoublemu);
   fInTree->SetBranchAddress("hltsinglemu", &hltsinglemu, &b_hltsinglemu);
   fInTree->SetBranchAddress("hltdoubleel", &hltdoubleel, &b_hltdoubleel);
   //   fInTree->SetBranchAddress("hltphoton135", &hltphoton135, &b_hltphoton135);
   //   fInTree->SetBranchAddress("hltphoton150", &hltphoton150, &b_hltphoton150);
   fInTree->SetBranchAddress("nmuons", &nmuons, &b_nmuons);
   fInTree->SetBranchAddress("nelectrons", &nelectrons, &b_nelectrons);
   fInTree->SetBranchAddress("nphotons", &nphotons, &b_nphotons);
   fInTree->SetBranchAddress("ntightmuons", &ntightmuons, &b_ntightmuons);
   fInTree->SetBranchAddress("ntightelectrons", &ntightelectrons, &b_ntightelectrons);
   //   fInTree->SetBranchAddress("ntightphotons", &ntightphotons, &b_ntightphotons);
   fInTree->SetBranchAddress("ntaus", &ntaus, &b_ntaus);
   fInTree->SetBranchAddress("njets", &njets, &b_njets);
   //   fInTree->SetBranchAddress("ncentjets", &ncentjets, &b_ncentjets);
   fInTree->SetBranchAddress("nbjets", &nbjets, &b_nbjets);
   fInTree->SetBranchAddress("nfatjets", &nfatjets, &b_nfatjets);
   //   fInTree->SetBranchAddress("ncentfatjets", &ncentfatjets, &b_ncentfatjets);
   //   fInTree->SetBranchAddress("njetsnotfat", &njetsnotfat, &b_njetsnotfat);
   //   fInTree->SetBranchAddress("ncentjetsnotfat", &ncentjetsnotfat, &b_ncentjetsnotfat);
   //   fInTree->SetBranchAddress("nbjetsnotfat", &nbjetsnotfat, &b_nbjetsnotfat);
   fInTree->SetBranchAddress("pfmet", &pfmet, &b_pfmet);
   fInTree->SetBranchAddress("pfmetphi", &pfmetphi, &b_pfmetphi);
   fInTree->SetBranchAddress("t1pfmet", &t1pfmet, &b_t1pfmet);
   fInTree->SetBranchAddress("t1pfmetphi", &t1pfmetphi, &b_t1pfmetphi);
   //   fInTree->SetBranchAddress("calomet", &calomet, &b_calomet);
   //   fInTree->SetBranchAddress("calometphi", &calometphi, &b_calometphi);
   fInTree->SetBranchAddress("pfmupt", &pfmupt, &b_pfmupt);
   fInTree->SetBranchAddress("pfmuphi", &pfmuphi, &b_pfmuphi);
   fInTree->SetBranchAddress("mumet", &mumet, &b_mumet);
   fInTree->SetBranchAddress("mumetphi", &mumetphi, &b_mumetphi);
   fInTree->SetBranchAddress("phmet", &phmet, &b_phmet);
   fInTree->SetBranchAddress("phmetphi", &phmetphi, &b_phmetphi);
   fInTree->SetBranchAddress("t1mumet", &t1mumet, &b_t1mumet);
   fInTree->SetBranchAddress("t1mumetphi", &t1mumetphi, &b_t1mumetphi);
   fInTree->SetBranchAddress("t1phmet", &t1phmet, &b_t1phmet);
   fInTree->SetBranchAddress("t1phmetphi", &t1phmetphi, &b_t1phmetphi);
   fInTree->SetBranchAddress("fatjetpt", &fatjetpt, &b_fatjetpt);
   fInTree->SetBranchAddress("fatjeteta", &fatjeteta, &b_fatjeteta);
   fInTree->SetBranchAddress("fatjetphi", &fatjetphi, &b_fatjetphi);
   //   fInTree->SetBranchAddress("fatjetmass", &fatjetmass, &b_fatjetmass);
   //   fInTree->SetBranchAddress("fatjetprunedmass", &fatjetprunedmass, &b_fatjetprunedmass);
   //   fInTree->SetBranchAddress("fatjetmassdrop", &fatjetmassdrop, &b_fatjetmassdrop);
   fInTree->SetBranchAddress("fatjettau2", &fatjettau2, &b_fatjettau2);
   fInTree->SetBranchAddress("fatjettau1", &fatjettau1, &b_fatjettau1);
   fInTree->SetBranchAddress("fatjetCHfrac", &fatjetCHfrac, &b_fatjetCHfrac);
   fInTree->SetBranchAddress("fatjetNHfrac", &fatjetNHfrac, &b_fatjetNHfrac);
   fInTree->SetBranchAddress("fatjetEMfrac", &fatjetEMfrac, &b_fatjetEMfrac);
   fInTree->SetBranchAddress("fatjetCEMfrac", &fatjetCEMfrac, &b_fatjetCEMfrac);
   fInTree->SetBranchAddress("fatjetmetdphi", &fatjetmetdphi, &b_fatjetmetdphi);
   fInTree->SetBranchAddress("signaljetpt", &signaljetpt, &b_signaljetpt);
   fInTree->SetBranchAddress("signaljeteta", &signaljeteta, &b_signaljeteta);
   fInTree->SetBranchAddress("signaljetphi", &signaljetphi, &b_signaljetphi);
   fInTree->SetBranchAddress("signaljetCHfrac", &signaljetCHfrac, &b_signaljetCHfrac);
   fInTree->SetBranchAddress("signaljetNHfrac", &signaljetNHfrac, &b_signaljetNHfrac);
   fInTree->SetBranchAddress("signaljetEMfrac", &signaljetEMfrac, &b_signaljetEMfrac);
   fInTree->SetBranchAddress("signaljetCEMfrac", &signaljetCEMfrac, &b_signaljetCEMfrac);
   fInTree->SetBranchAddress("signaljetmetdphi", &signaljetmetdphi, &b_signaljetmetdphi);
   fInTree->SetBranchAddress("secondjetpt", &secondjetpt, &b_secondjetpt);
   fInTree->SetBranchAddress("secondjeteta", &secondjeteta, &b_secondjeteta);
   fInTree->SetBranchAddress("secondjetphi", &secondjetphi, &b_secondjetphi);
   fInTree->SetBranchAddress("secondjetCHfrac", &secondjetCHfrac, &b_secondjetCHfrac);
   fInTree->SetBranchAddress("secondjetNHfrac", &secondjetNHfrac, &b_secondjetNHfrac);
   fInTree->SetBranchAddress("secondjetEMfrac", &secondjetEMfrac, &b_secondjetEMfrac);
   fInTree->SetBranchAddress("secondjetCEMfrac", &secondjetCEMfrac, &b_secondjetCEMfrac);
   fInTree->SetBranchAddress("secondjetmetdphi", &secondjetmetdphi, &b_secondjetmetdphi);
   fInTree->SetBranchAddress("jetjetdphi", &jetjetdphi, &b_jetjetdphi);
   fInTree->SetBranchAddress("jetmetdphimin", &jetmetdphimin, &b_jetmetdphimin);
   fInTree->SetBranchAddress("thirdjetpt", &thirdjetpt, &b_thirdjetpt);
   fInTree->SetBranchAddress("thirdjeteta", &thirdjeteta, &b_thirdjeteta);
   fInTree->SetBranchAddress("thirdjetphi", &thirdjetphi, &b_thirdjetphi);
   fInTree->SetBranchAddress("thirdjetCHfrac", &thirdjetCHfrac, &b_thirdjetCHfrac);
   fInTree->SetBranchAddress("thirdjetNHfrac", &thirdjetNHfrac, &b_thirdjetNHfrac);
   fInTree->SetBranchAddress("thirdjetEMfrac", &thirdjetEMfrac, &b_thirdjetEMfrac);
   fInTree->SetBranchAddress("thirdjetCEMfrac", &thirdjetCEMfrac, &b_thirdjetCEMfrac);
   fInTree->SetBranchAddress("thirdjetmetdphi", &thirdjetmetdphi, &b_thirdjetmetdphi);
   fInTree->SetBranchAddress("ht", &ht, &b_ht);
   fInTree->SetBranchAddress("dht", &dht, &b_dht);
   fInTree->SetBranchAddress("mht", &mht, &b_mht);
   fInTree->SetBranchAddress("alphat", &alphat, &b_alphat);
   //   fInTree->SetBranchAddress("apcjet", &apcjet, &b_apcjet);
   //   fInTree->SetBranchAddress("apcmet", &apcmet, &b_apcmet);
   //   fInTree->SetBranchAddress("apcjetmet", &apcjetmet, &b_apcjetmet);
   //   fInTree->SetBranchAddress("apcjetmax", &apcjetmax, &b_apcjetmax);
   fInTree->SetBranchAddress("apcjetmetmax", &apcjetmetmax, &b_apcjetmetmax);
   //   fInTree->SetBranchAddress("apcjetmin", &apcjetmin, &b_apcjetmin);
   fInTree->SetBranchAddress("apcjetmetmin", &apcjetmetmin, &b_apcjetmetmin);
   fInTree->SetBranchAddress("mu1pid", &mu1pid, &b_mu1pid);
   fInTree->SetBranchAddress("mu1pt", &mu1pt, &b_mu1pt);
   fInTree->SetBranchAddress("mu1eta", &mu1eta, &b_mu1eta);
   fInTree->SetBranchAddress("mu1phi", &mu1phi, &b_mu1phi);
   fInTree->SetBranchAddress("mu1id", &mu1id, &b_mu1id);
   fInTree->SetBranchAddress("mu2pid", &mu2pid, &b_mu2pid);
   fInTree->SetBranchAddress("mu2pt", &mu2pt, &b_mu2pt);
   fInTree->SetBranchAddress("mu2eta", &mu2eta, &b_mu2eta);
   fInTree->SetBranchAddress("mu2phi", &mu2phi, &b_mu2phi);
   fInTree->SetBranchAddress("mu2id", &mu2id, &b_mu2id);
   fInTree->SetBranchAddress("el1pid", &el1pid, &b_el1pid);
   fInTree->SetBranchAddress("el1pt", &el1pt, &b_el1pt);
   fInTree->SetBranchAddress("el1eta", &el1eta, &b_el1eta);
   fInTree->SetBranchAddress("el1phi", &el1phi, &b_el1phi);
   fInTree->SetBranchAddress("el1id", &el1id, &b_el1id);
   fInTree->SetBranchAddress("el2pid", &el2pid, &b_el2pid);
   fInTree->SetBranchAddress("el2pt", &el2pt, &b_el2pt);
   fInTree->SetBranchAddress("el2eta", &el2eta, &b_el2eta);
   fInTree->SetBranchAddress("el2phi", &el2phi, &b_el2phi);
   fInTree->SetBranchAddress("el2id", &el2id, &b_el2id);
   fInTree->SetBranchAddress("zmass", &zmass, &b_zmass);
   fInTree->SetBranchAddress("zpt", &zpt, &b_zpt);
   fInTree->SetBranchAddress("zeta", &zeta, &b_zeta);
   fInTree->SetBranchAddress("zphi", &zphi, &b_zphi);
   fInTree->SetBranchAddress("wmt", &wmt, &b_wmt);
   fInTree->SetBranchAddress("emumass", &emumass, &b_emumass);
   fInTree->SetBranchAddress("emupt", &emupt, &b_emupt);
   fInTree->SetBranchAddress("emueta", &emueta, &b_emueta);
   fInTree->SetBranchAddress("emuphi", &emuphi, &b_emuphi);
   fInTree->SetBranchAddress("zeemass", &zeemass, &b_zeemass);
   fInTree->SetBranchAddress("zeept", &zeept, &b_zeeept);
   fInTree->SetBranchAddress("zeeeta", &zeeeta, &b_zeeeta);
   fInTree->SetBranchAddress("zeephi", &zeephi, &b_zeephi);
   fInTree->SetBranchAddress("wemt", &wemt, &b_wemt);
   fInTree->SetBranchAddress("phpt", &phpt, &b_phpt);
   fInTree->SetBranchAddress("pheta", &pheta, &b_pheta);
   fInTree->SetBranchAddress("phphi", &phphi, &b_phphi);
   fInTree->SetBranchAddress("wzid", &wzid, &b_wzid);
   fInTree->SetBranchAddress("wzmass", &wzmass, &b_wzmass);
   fInTree->SetBranchAddress("wzmt", &wzmt, &b_wzmt);
   fInTree->SetBranchAddress("wzpt", &wzpt, &b_wzpt);
   fInTree->SetBranchAddress("wzeta", &wzeta, &b_wzeta);
   fInTree->SetBranchAddress("wzphi", &wzphi, &b_wzphi);
   fInTree->SetBranchAddress("l1id", &l1id, &b_l1id);
   fInTree->SetBranchAddress("l1pt", &l1pt, &b_l1pt);
   fInTree->SetBranchAddress("l1eta", &l1eta, &b_l1eta);
   fInTree->SetBranchAddress("l1phi", &l1phi, &b_l1phi);
   fInTree->SetBranchAddress("l2id", &l2id, &b_l2id);
   fInTree->SetBranchAddress("l2pt", &l2pt, &b_l2pt);
   fInTree->SetBranchAddress("l2eta", &l2eta, &b_l2eta);
   fInTree->SetBranchAddress("l2phi", &l2phi, &b_l2phi);
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
