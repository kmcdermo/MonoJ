#include "Analysis.hh"

Analysis::Analysis(const SamplePair samplePair, const Double_t lumi, const ColorMap colorMap, const TString outdir, const TString outType){
  // store in data members
  fSample = samplePair.first;
  fIsMC   = samplePair.second;
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
  fOutDir = outdir;

  if (fIsMC) {
    fOutName = Form("%s_MC",fSample.Data());
  }
  else {
    fOutName = Form("%s_data",fSample.Data());
  }

  // make output directory
  MakeOutDirectory(Form("%s/%s",fOutDir.Data(),fOutName.Data()));

  // make output root file
  fOutFile = new TFile(Form("%s/%s/plots.root",fOutDir.Data(),fOutName.Data()),"RECREATE");

  // set color map
  fColorMap = colorMap;

  // allow user to pick png, pdf, gif, etc
  fOutType = outType;
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

	if (njets>=1){ // plots stuff with jets >= 1
	  fTH1DMap["signaljetpt"]->Fill(signaljetpt,weight);
	  fTH1DMap["signaljeteta"]->Fill(signaljeteta,weight);
	  fTH1DMap["signaljetCHfrac"]->Fill(signaljetCHfrac,weight);
	  fTH1DMap["signaljetNHfrac"]->Fill(signaljetNHfrac,weight);
	  fTH1DMap["signaljetEMfrac"]->Fill(signaljetEMfrac,weight);
	  fTH1DMap["signaljetCEMfrac"]->Fill(signaljetCEMfrac,weight);
	}
	// int plots
	fTH1DMap["njets"]->Fill(njets,weight);
	fTH1DMap["nvtx"]->Fill(nvtx,weight);
	
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

  fTH1DMap["nvtx"]  = Analysis::MakeTH1DPlot("nvtx","",50,0,50,"Number of Primary Vertices","Events");
  fTH1DMap["njets"] = Analysis::MakeTH1DPlot("njets","",50,0,50,"Jet Multiplicity","Events");
}

TH1D * Analysis::MakeTH1DPlot(const TString hname, const TString htitle, const Int_t nbins, const Double_t xlow, const Double_t xhigh, const TString xtitle, const TString ytitle){
  TH1D * hist = new TH1D(hname.Data(),htitle.Data(),nbins,xlow,xhigh);
  hist->GetXaxis()->SetTitle(xtitle.Data());
  hist->GetYaxis()->SetTitle(ytitle.Data());
  hist->Sumw2();
  return hist;
}

void Analysis::SaveHists() {
  fOutFile->cd();

  TCanvas * canv = new TCanvas();
  for (TH1DMapIter mapiter = fTH1DMap.begin(); mapiter != fTH1DMap.end(); mapiter++) { // first do double hists ... could template this...
    for (Int_t ibin = 1; ibin <= (*mapiter).second->GetNbinsX(); ibin++){
      if ((*mapiter).second->GetBinContent(ibin) < 0) { // check to make sure negative weights are not messing things up
	(*mapiter).second->SetBinContent(ibin,0);
      }
    }
    (*mapiter).second->Write(); // map is map["hist name",TH1D*]

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
    canv->SaveAs(Form("%s/%s/%s_log.%s",fOutDir.Data(),fOutName.Data(),(*mapiter).first.Data(),fOutType.Data()));

    canv->SetLogy(0);
    canv->SaveAs(Form("%s/%s/%s_lin.%s",fOutDir.Data(),fOutName.Data(),(*mapiter).first.Data(),fOutType.Data()));
  }

  delete canv;
}

void Analysis::DeleteHists() {
  for (TH1DMapIter mapiter = fTH1DMap.begin(); mapiter != fTH1DMap.end(); mapiter++) {
    delete ((*mapiter).second);
  }
  fTH1DMap.clear();
}

void Analysis::SetBranchAddresses() {
  fInTree->SetBranchAddress("event", &event, &b_event);
  fInTree->SetBranchAddress("run", &run, &b_run);
  fInTree->SetBranchAddress("lumi", &lumi, &b_lumi);
  fInTree->SetBranchAddress("xsec", &xsec, &b_xsec);
  fInTree->SetBranchAddress("wgt", &wgt, &b_wgt);
  fInTree->SetBranchAddress("kfact", &kfact, &b_kfact);
  fInTree->SetBranchAddress("puwgt", &puwgt, &b_puwgt);
  fInTree->SetBranchAddress("puobs", &puobs, &b_puobs);
  fInTree->SetBranchAddress("putrue", &putrue, &b_putrue);
  fInTree->SetBranchAddress("nvtx", &nvtx, &b_nvtx);
  fInTree->SetBranchAddress("hltmet90", &hltmet90, &b_hltmet90);
  fInTree->SetBranchAddress("hltmet120", &hltmet120, &b_hltmet120);
  fInTree->SetBranchAddress("hltmetwithmu90", &hltmetwithmu90, &b_hltmetwithmu90);
  fInTree->SetBranchAddress("hltmetwithmu120", &hltmetwithmu120, &b_hltmetwithmu120);
  fInTree->SetBranchAddress("hltmetwithmu170", &hltmetwithmu170, &b_hltmetwithmu170);
  fInTree->SetBranchAddress("hltmetwithmu300", &hltmetwithmu300, &b_hltmetwithmu300);
  fInTree->SetBranchAddress("hltjetmet90", &hltjetmet90, &b_hltjetmet90);
  fInTree->SetBranchAddress("hltjetmet120", &hltjetmet120, &b_hltjetmet120);
  fInTree->SetBranchAddress("hltphoton165", &hltphoton165, &b_hltphoton165);
  fInTree->SetBranchAddress("hltphoton175", &hltphoton175, &b_hltphoton175);
  fInTree->SetBranchAddress("hltdoublemu", &hltdoublemu, &b_hltdoublemu);
  fInTree->SetBranchAddress("hltsinglemu", &hltsinglemu, &b_hltsinglemu);
  fInTree->SetBranchAddress("hltdoubleel", &hltdoubleel, &b_hltdoubleel);
  fInTree->SetBranchAddress("hltsingleel", &hltsingleel, &b_hltsingleel);
  fInTree->SetBranchAddress("flagcsctight", &flagcsctight, &b_flagcsctight);
  fInTree->SetBranchAddress("flaghbhenoise", &flaghbhenoise, &b_flaghbhenoise);
  fInTree->SetBranchAddress("flaghcallaser", &flaghcallaser, &b_flaghcallaser);
  fInTree->SetBranchAddress("flagecaltrig", &flagecaltrig, &b_flagecaltrig);
  fInTree->SetBranchAddress("flageebadsc", &flageebadsc, &b_flageebadsc);
  fInTree->SetBranchAddress("flagecallaser", &flagecallaser, &b_flagecallaser);
  fInTree->SetBranchAddress("flagtrkfail", &flagtrkfail, &b_flagtrkfail);
  fInTree->SetBranchAddress("flagtrkpog", &flagtrkpog, &b_flagtrkpog);
  fInTree->SetBranchAddress("flaghnoiseloose", &flaghnoiseloose, &b_flaghnoiseloose);
  fInTree->SetBranchAddress("flaghnoisetight", &flaghnoisetight, &b_flaghnoisetight);
  fInTree->SetBranchAddress("flaghnoisehilvl", &flaghnoisehilvl, &b_flaghnoisehilvl);
  fInTree->SetBranchAddress("nmuons", &nmuons, &b_nmuons);
  fInTree->SetBranchAddress("nelectrons", &nelectrons, &b_nelectrons);
  fInTree->SetBranchAddress("ntightmuons", &ntightmuons, &b_ntightmuons);
  fInTree->SetBranchAddress("ntightelectrons", &ntightelectrons, &b_ntightelectrons);
  fInTree->SetBranchAddress("ntaus", &ntaus, &b_ntaus);
  fInTree->SetBranchAddress("njets", &njets, &b_njets);
  fInTree->SetBranchAddress("nbjets", &nbjets, &b_nbjets);
  fInTree->SetBranchAddress("nfatjets", &nfatjets, &b_nfatjets);
  fInTree->SetBranchAddress("nphotons", &nphotons, &b_nphotons);
  fInTree->SetBranchAddress("pfmet", &pfmet, &b_pfmet);
  fInTree->SetBranchAddress("pfmetphi", &pfmetphi, &b_pfmetphi);
  fInTree->SetBranchAddress("t1pfmet", &t1pfmet, &b_t1pfmet);
  fInTree->SetBranchAddress("t1pfmetphi", &t1pfmetphi, &b_t1pfmetphi);
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
  fInTree->SetBranchAddress("fatjetprmass", &fatjetprmass, &b_fatjetprmass);
  fInTree->SetBranchAddress("fatjetsdmass", &fatjetsdmass, &b_fatjetsdmass);
  fInTree->SetBranchAddress("fatjettrmass", &fatjettrmass, &b_fatjettrmass);
  fInTree->SetBranchAddress("fatjetftmass", &fatjetftmass, &b_fatjetftmass);
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
  fInTree->SetBranchAddress("signaljetbtag", &signaljetbtag, &b_signaljetbtag);
  fInTree->SetBranchAddress("signaljetCHfrac", &signaljetCHfrac, &b_signaljetCHfrac);
  fInTree->SetBranchAddress("signaljetNHfrac", &signaljetNHfrac, &b_signaljetNHfrac);
  fInTree->SetBranchAddress("signaljetEMfrac", &signaljetEMfrac, &b_signaljetEMfrac);
  fInTree->SetBranchAddress("signaljetCEMfrac", &signaljetCEMfrac, &b_signaljetCEMfrac);
  fInTree->SetBranchAddress("signaljetmetdphi", &signaljetmetdphi, &b_signaljetmetdphi);
  fInTree->SetBranchAddress("secondjetpt", &secondjetpt, &b_secondjetpt);
  fInTree->SetBranchAddress("secondjeteta", &secondjeteta, &b_secondjeteta);
  fInTree->SetBranchAddress("secondjetphi", &secondjetphi, &b_secondjetphi);
  fInTree->SetBranchAddress("secondjetbtag", &secondjetbtag, &b_secondjetbtag);
  fInTree->SetBranchAddress("secondjetCHfrac", &secondjetCHfrac, &b_secondjetCHfrac);
  fInTree->SetBranchAddress("secondjetNHfrac", &secondjetNHfrac, &b_secondjetNHfrac);
  fInTree->SetBranchAddress("secondjetEMfrac", &secondjetEMfrac, &b_secondjetEMfrac);
  fInTree->SetBranchAddress("secondjetCEMfrac", &secondjetCEMfrac, &b_secondjetCEMfrac);
  fInTree->SetBranchAddress("secondjetmetdphi", &secondjetmetdphi, &b_secondjetmetdphi);
  fInTree->SetBranchAddress("thirdjetpt", &thirdjetpt, &b_thirdjetpt);
  fInTree->SetBranchAddress("thirdjeteta", &thirdjeteta, &b_thirdjeteta);
  fInTree->SetBranchAddress("thirdjetphi", &thirdjetphi, &b_thirdjetphi);
  fInTree->SetBranchAddress("thirdjetbtag", &thirdjetbtag, &b_thirdjetbtag);
  fInTree->SetBranchAddress("thirdjetCHfrac", &thirdjetCHfrac, &b_thirdjetCHfrac);
  fInTree->SetBranchAddress("thirdjetNHfrac", &thirdjetNHfrac, &b_thirdjetNHfrac);
  fInTree->SetBranchAddress("thirdjetEMfrac", &thirdjetEMfrac, &b_thirdjetEMfrac);
  fInTree->SetBranchAddress("thirdjetCEMfrac", &thirdjetCEMfrac, &b_thirdjetCEMfrac);
  fInTree->SetBranchAddress("thirdjetmetdphi", &thirdjetmetdphi, &b_thirdjetmetdphi);
  fInTree->SetBranchAddress("jetjetdphi", &jetjetdphi, &b_jetjetdphi);
  fInTree->SetBranchAddress("jetmetdphimin", &jetmetdphimin, &b_jetmetdphimin);
  fInTree->SetBranchAddress("incjetmetdphimin", &incjetmetdphimin, &b_incjetmetdphimin);
  fInTree->SetBranchAddress("ht", &ht, &b_ht);
  fInTree->SetBranchAddress("dht", &dht, &b_dht);
  fInTree->SetBranchAddress("mht", &mht, &b_mht);
  fInTree->SetBranchAddress("alphat", &alphat, &b_alphat);
  fInTree->SetBranchAddress("apcjetmetmax", &apcjetmetmax, &b_apcjetmetmax);
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
  fInTree->SetBranchAddress("loosephpt", &loosephpt, &b_loosephpt);
  fInTree->SetBranchAddress("loosepheta", &loosepheta, &b_loosepheta);
  fInTree->SetBranchAddress("loosephphi", &loosephphi, &b_loosephphi);
  fInTree->SetBranchAddress("loosephsieie", &loosephsieie, &b_loosephsieie);
  fInTree->SetBranchAddress("loosephrndiso", &loosephrndiso, &b_loosephrndiso);
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
  fInTree->SetBranchAddress("i1id", &i1id, &b_i1id);
  fInTree->SetBranchAddress("i1pt", &i1pt, &b_i1pt);
  fInTree->SetBranchAddress("i1eta", &i1eta, &b_i1eta);
  fInTree->SetBranchAddress("i1phi", &i1phi, &b_i1phi);
  fInTree->SetBranchAddress("i2id", &i2id, &b_i2id);
  fInTree->SetBranchAddress("i2pt", &i2pt, &b_i2pt);
  fInTree->SetBranchAddress("i2eta", &i2eta, &b_i2eta);
  fInTree->SetBranchAddress("i2phi", &i2phi, &b_i2phi);
  fInTree->SetBranchAddress("i3id", &i3id, &b_i3id);
  fInTree->SetBranchAddress("i3pt", &i3pt, &b_i3pt);
  fInTree->SetBranchAddress("i3eta", &i3eta, &b_i3eta);
  fInTree->SetBranchAddress("i3phi", &i3phi, &b_i3phi);
  fInTree->SetBranchAddress("cflagcsctight", &cflagcsctight, &b_cflagcsctight);
  fInTree->SetBranchAddress("cflaghbhenoise", &cflaghbhenoise, &b_cflaghbhenoise);
  fInTree->SetBranchAddress("cflaghcallaser", &cflaghcallaser, &b_cflaghcallaser);
  fInTree->SetBranchAddress("cflagecaltrig", &cflagecaltrig, &b_cflagecaltrig);
  fInTree->SetBranchAddress("cflageebadsc", &cflageebadsc, &b_cflageebadsc);
  fInTree->SetBranchAddress("cflagecallaser", &cflagecallaser, &b_cflagecallaser);
  fInTree->SetBranchAddress("cflagtrkfail", &cflagtrkfail, &b_cflagtrkfail);
  fInTree->SetBranchAddress("cflagtrkpog", &cflagtrkpog, &b_cflagtrkpog);
  fInTree->SetBranchAddress("wgtsum", &wgtsum, &b_wgtsum);
}

