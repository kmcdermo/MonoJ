#include "../interface/Analysis.hh"

Analysis::Analysis(const SamplePair samplePair, const TString selection, const Int_t njetsselection, const DblVec puweights, const Double_t lumi, const Int_t nBins_vtx, const ColorMap colorMap, const TString outdir, const TString outType){
  // store in data members
  fSample    = samplePair.first;
  fIsMC      = samplePair.second;
  fSelection = selection;
  fNJetsSeln = njetsselection;
  fPUWeights = puweights;
  fLumi      = lumi;
  fNBins_vtx = nBins_vtx;

  // string for output of njets... -1 == no requirment on njets
  fNJetsStr = "";
  if (fNJetsSeln != -1){
    fNJetsStr = Form("_nj%i",fNJetsSeln);
  }

  //Get File
  TString fileName = "root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/";
  //TString fileName = ""; // do this for local macbook copy
  if (fIsMC){ // MC
    if (fSample.Contains("gamma",TString::kExact) ){ // use phys14 for photons
      fileName.Append("PHYS14MC/");
    }
    else {
      fileName.Append("Spring15MC_50ns/");
    }
  }
  else{ // Data
    fileName.Append("Data/");
  }
  fileName.Append(Form("%s/treewithwgt.root",fSample.Data()));
    
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

  // make output directory + subdirectories
  MakeOutDirectory(Form("%s/%s%s/%s",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Photons",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Leptons",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Leptons/Muons",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Leptons/Electrons",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Leptons/Dileptons",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Jets",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Jets/Leading",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Jets/Subleading",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/Jets/Subsubleading",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));
  MakeOutDirectory(Form("%s/%s%s/%s/MET",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()));

  // make output root file
  fOutFile = new TFile(Form("%s/%s%s/%s/plots.root",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data()),"RECREATE");

  // set color map
  fColorMap = colorMap;

  // allow user to pick png, pdf, gif, etc
  fOutType = outType;

  // make sure counter for yields is zero
  fNSelected = 0;
}

Analysis::~Analysis(){
  //  Analysis::DeleteBranches();
  delete fInTree;
  delete fInFile;
  Analysis::DeleteHists();
  delete fOutFile;
}

void Analysis::DoAnalysis(std::ofstream & yields){
  // set up output plots to be produced
  Analysis::SetUpPlots();

  // use these bools to save time inside entry loop
  Bool_t zmumuselection        = false;
  Bool_t zelelselection        = false;
  Bool_t singlemuselection     = false;
  Bool_t singlephotonselection = false;
  if (fSelection.Contains("zmumu",TString::kExact)) {
    zmumuselection = true;
  }
  else if (fSelection.Contains("zelel",TString::kExact)) {
    zelelselection = true;
  }
  else if (fSelection.Contains("singlemu",TString::kExact)){
    singlemuselection = true;
  }  
  else if (fSelection.Contains("singlephoton",TString::kExact)){
    singlephotonselection = true;
  }
  else {
    std::cout << "Not a valid selection: " << fSelection.Data() << " exiting... " << std::endl;
    exit(1);
  }

  // Loop over entries in input tree
  for (UInt_t entry = 0; entry < fInTree->GetEntries(); entry++) {
    fInTree->GetEntry(entry);

    Double_t weight = 0;
    if (fIsMC) {
      weight = (xsec * fLumi * wgt / wgtsum) * fPUWeights[nvtx];  
    }
    else {
      weight = 1.0; // data is currently to 1.0
    }
    
    // set selection here ... apply met_filters to all selections except singlephoton
    Bool_t selection   = false;
    Bool_t met_filters = false;
    if (zmumuselection) {
      selection = ((hltdoublemu > 0) && (mu1pt > 20) && (mu1id == 1) && (zmass < 120.) && (zmass > 60.) && (mu1pid == -mu2pid));
      met_filters = ((fIsMC && flagcsctight == 1 && flaghbhenoise == 1) || (!fIsMC && cflagcsctight == 1 && cflaghbhenoise == 1));
    }
    else if (zelelselection) {
      selection = ((hltdoubleel > 0) && (el1pt > 20) && (el1id == 1) && (zeemass < 120.) && (zeemass > 60.) && (el1pid == -el2pid));
      met_filters = ((fIsMC && flagcsctight == 1 && flaghbhenoise == 1) || (!fIsMC && cflagcsctight == 1 && cflaghbhenoise == 1));
    }
    else if (singlemuselection) {
      selection = ((hltsinglemu == 1) && (nmuons == 1) && (mu1pt > 30) && (mu1id == 1));
      met_filters = ((fIsMC && flagcsctight == 1 && flaghbhenoise == 1) || (!fIsMC && cflagcsctight == 1 && cflaghbhenoise == 1));
    }
    else if (singlephotonselection) {
      if (fIsMC) { // triggers bugged in phys14
	selection = ((nphotons == 1) && (phpt>200.));
      }
      else { // only apply triggers in data
	selection = (((hltphoton165 == 1) || (hltphoton175 == 1)) && (nphotons == 1) && (phpt>200.));
      }
      met_filters = true; // bugged met filters in PHYS14 MC single photons (50% efficient), so do not apply, just set to true
    }

    Bool_t jet_selection = false;
    if (fNJetsSeln != -1) {
      jet_selection = ((njets == fNJetsSeln));
    }
    else {
      jet_selection = true; // no selection, so set it to true
    }

    // now see if event passes final selection
    if ( selection && met_filters && jet_selection ){ 
      fNSelected += weight; // save the event weight for yields!

      // use this plot to get integral of yields --> no extra requirements for selection
      fTH1DMap["nvtx"]->Fill(nvtx,weight);

      // photon plots
      if (nphotons>=1) {
	fTH1DMap["phpt"]->Fill(phpt,weight);

	if (njets<=2){ //only two jets
	  fTH1DMap["phpt_nj_lte2"]->Fill(phpt,weight);
	}
      }

      // lepton plots
      // muons
      if (nmuons>=1) {
	fTH1DMap["mu1eta"]->Fill(mu1eta,weight);
	fTH1DMap["mu1phi"]->Fill(mu1phi,weight);
	fTH1DMap["mu1pt"]->Fill(mu1pt,weight);
      }

      if (nmuons>=2) {
	fTH1DMap["mu2eta"]->Fill(mu2eta,weight);
	fTH1DMap["mu2phi"]->Fill(mu2phi,weight);
	fTH1DMap["mu2pt"]->Fill(mu2pt,weight);
      }

      if (nmuons==1){
	fTH1DMap["wmt"]->Fill(wmt,weight);
      }

      // electrons
      if (nelectrons>=1) {
	fTH1DMap["el1eta"]->Fill(el1eta,weight);
	fTH1DMap["el1phi"]->Fill(el1phi,weight);
	fTH1DMap["el1pt"]->Fill(el1pt,weight);
      }

      if (nelectrons>=2) {
	fTH1DMap["el2eta"]->Fill(el2eta,weight);
	fTH1DMap["el2phi"]->Fill(el2phi,weight);
	fTH1DMap["el2pt"]->Fill(el2pt,weight);
      }

      if (nelectrons==1){
	fTH1DMap["wemt"]->Fill(wemt,weight);
      }

      // dileptons
      if (nmuons==2) {
	fTH1DMap["zeta"]->Fill(zeta,weight);
	fTH1DMap["zmass"]->Fill(zmass,weight);
	fTH1DMap["zphi"]->Fill(zphi,weight);
	fTH1DMap["zpt"]->Fill(zpt,weight);
      }

      if (nelectrons==2){
	fTH1DMap["zeeeta"]->Fill(zeeeta,weight);
	fTH1DMap["zeemass"]->Fill(zeemass,weight);
	fTH1DMap["zeephi"]->Fill(zeephi,weight);
	fTH1DMap["zeept"]->Fill(zeept,weight);
      }
 
      if (nelectrons==1 && nmuons==1){
	fTH1DMap["emueta"]->Fill(emueta,weight);
	fTH1DMap["emumass"]->Fill(emumass,weight);
	fTH1DMap["emuphi"]->Fill(emuphi,weight);
	fTH1DMap["emupt"]->Fill(emupt,weight);
      }
      
      // jets
      fTH1DMap["njets"]->Fill(njets,weight);

      if (njets>=1){ // make sure we have one jet to make ht plots!
	fTH1DMap["ht"]->Fill(ht,weight);
      }

      if (njets>=1){ // plots stuff for signal jet
	fTH1DMap["signaljeteta"]->Fill(signaljeteta,weight);
	fTH1DMap["signaljetphi"]->Fill(signaljetphi,weight);
	fTH1DMap["signaljetpt"]->Fill(signaljetpt,weight);
	fTH1DMap["signaljetCHfrac"]->Fill(signaljetCHfrac,weight);
	fTH1DMap["signaljetNHfrac"]->Fill(signaljetNHfrac,weight);
	fTH1DMap["signaljetEMfrac"]->Fill(signaljetEMfrac,weight);
	fTH1DMap["signaljetCEMfrac"]->Fill(signaljetCEMfrac,weight);
      }
      
      if (njets>=2){ // plots stuff with sub leading jet
	fTH1DMap["secondjeteta"]->Fill(secondjeteta,weight);
	fTH1DMap["secondjetphi"]->Fill(secondjetphi,weight);
	fTH1DMap["secondjetpt"]->Fill(secondjetpt,weight);
	fTH1DMap["secondjetCHfrac"]->Fill(secondjetCHfrac,weight);
	fTH1DMap["secondjetNHfrac"]->Fill(secondjetNHfrac,weight);
	fTH1DMap["secondjetEMfrac"]->Fill(secondjetEMfrac,weight);
	fTH1DMap["secondjetCEMfrac"]->Fill(secondjetCEMfrac,weight);
      }
      
      if (njets>=3){ // plots stuff with sub sub leading jet
	fTH1DMap["thirdjeteta"]->Fill(thirdjeteta,weight);
	fTH1DMap["thirdjetphi"]->Fill(thirdjetphi,weight);
	fTH1DMap["thirdjetpt"]->Fill(thirdjetpt,weight);
	fTH1DMap["thirdjetCHfrac"]->Fill(thirdjetCHfrac,weight);
	fTH1DMap["thirdjetNHfrac"]->Fill(thirdjetNHfrac,weight);
	fTH1DMap["thirdjetEMfrac"]->Fill(thirdjetEMfrac,weight);
	fTH1DMap["thirdjetCEMfrac"]->Fill(thirdjetCEMfrac,weight);
      }

      // MET Plots
      fTH1DMap["pfmet"]->Fill(pfmet,weight);
      fTH1DMap["t1pfmet"]->Fill(t1pfmet,weight);
      fTH1DMap["pfmetphi"]->Fill(pfmetphi,weight);
      fTH1DMap["t1pfmetphi"]->Fill(t1pfmetphi,weight);

      if (nmuons>=1){ // plots need muons!
	fTH1DMap["mumet"]->Fill(mumet,weight);
	fTH1DMap["t1mumet"]->Fill(t1mumet,weight);
	fTH1DMap["mumetphi"]->Fill(mumetphi,weight);
	fTH1DMap["t1mumetphi"]->Fill(t1mumetphi,weight); 
      }

      if (njets>=1){ // make sure we have one jet to make ht plots!
	fTH1DMap["mht"]->Fill(mht,weight);
      }
    } // end filling histos after checking selection criteria 
  } // end loop over entries

  // dump yields into text file
  yields << fSample.Data() << ", isMC: " << fIsMC << ": " << fNSelected << std::endl;

  // save the histos once loop is over
  Analysis::SaveHists();
}

void Analysis::SetUpPlots() {
  // overall plots, no subdir
  fTH1DMap["nvtx"]  = Analysis::MakeTH1DPlot("nvtx","",fNBins_vtx,0,fNBins_vtx,"Number of Primary Vertices","Events");
  fTH1DSubDMap["nvtx"] = "";

  // photon plots
  fTH1DMap["phpt"]         = Analysis::MakeTH1DPlot("phpt","",40,200.,1000.,"Phton p_{T} [GeV/c]","Events / 20 GeV/c"); 
  fTH1DMap["phpt_nj_lte2"] = Analysis::MakeTH1DPlot("phpt_nj_lte2","",40,200.,1000.,"Phton p_{T} (n_{jets} #leq 2) [GeV/c]","Events / 20 GeV/c"); 
  fTH1DSubDMap["phpt"]         = "Photons/";
  fTH1DSubDMap["phpt_nj_lte2"] = "Photons/";

  // lepton plots
  // muon plots
  fTH1DMap["mu1eta"] = Analysis::MakeTH1DPlot("mu1eta","",30,-3.,3.,"Leading Muon #eta","Events"); 
  fTH1DMap["mu1phi"] = Analysis::MakeTH1DPlot("mu1phi","",32,-3.2,3.2,"Leading Muon #phi","Events"); 
  fTH1DMap["mu1pt"]  = Analysis::MakeTH1DPlot("mu1pt","",50,0.,500.,"Leading Muon p_{T} [GeV/c]","Events / 10 GeV/c"); 
  fTH1DMap["mu2eta"] = Analysis::MakeTH1DPlot("mu2eta","",30,-3.,3.,"Subleading Muon #eta","Events"); 
  fTH1DMap["mu2phi"] = Analysis::MakeTH1DPlot("mu2phi","",32,-3.2,3.2,"Subleading Muon #phi","Events"); 
  fTH1DMap["mu2pt"]  = Analysis::MakeTH1DPlot("mu2pt","",50,0.,500.,"Subleading Muon p_{T} [GeV/c]","Events / 10 GeV/c"); 
  fTH1DSubDMap["mu1eta"] = "Leptons/Muons/";
  fTH1DSubDMap["mu1phi"] = "Leptons/Muons/";
  fTH1DSubDMap["mu1pt"]  = "Leptons/Muons/";
  fTH1DSubDMap["mu2eta"] = "Leptons/Muons/";
  fTH1DSubDMap["mu2phi"] = "Leptons/Muons/";
  fTH1DSubDMap["mu2pt"]  = "Leptons/Muons/";

  fTH1DMap["wmt"] = Analysis::MakeTH1DPlot("wmt","",24,0.,120.,"W_{T} Mass from Single Muon [GeV/c^{2}]","Events / 5 GeV/c^{2}");
  fTH1DSubDMap["wmt"] = "Leptons/Muons/";

  // electron plots
  fTH1DMap["el1eta"] = Analysis::MakeTH1DPlot("el1eta","",30,-3.,3.,"Leading Electron #eta","Events"); 
  fTH1DMap["el1phi"] = Analysis::MakeTH1DPlot("el1phi","",32,-3.2,3.2,"Leading Electron #phi","Events"); 
  fTH1DMap["el1pt"]  = Analysis::MakeTH1DPlot("el1pt","",50,0.,500.,"Leading Electron p_{T} [GeV/c]","Events / 10 GeV/c"); 
  fTH1DMap["el2eta"] = Analysis::MakeTH1DPlot("el2eta","",30,-3.,3.,"Subleading Electron #eta","Events"); 
  fTH1DMap["el2phi"] = Analysis::MakeTH1DPlot("el2phi","",32,-3.2,3.2,"Subleading Electron #phi","Events"); 
  fTH1DMap["el2pt"]  = Analysis::MakeTH1DPlot("el2pt","",50,0.,500.,"Subleading Electron p_{T} [GeV/c]","Events / 10 GeV/c"); 
  fTH1DSubDMap["el1eta"] = "Leptons/Electrons/";
  fTH1DSubDMap["el1phi"] = "Leptons/Electrons/";
  fTH1DSubDMap["el1pt"]  = "Leptons/Electrons/";
  fTH1DSubDMap["el2eta"] = "Leptons/Electrons/";
  fTH1DSubDMap["el2phi"] = "Leptons/Electrons/";
  fTH1DSubDMap["el2pt"]  = "Leptons/Electrons/";

  fTH1DMap["wemt"] = Analysis::MakeTH1DPlot("wemt","",24,0.,120.,"W_{T} Mass from Single Electron [GeV/c^{2}]","Events / 5 GeV/c^{2}");
  fTH1DSubDMap["wemt"] = "Leptons/Electrons/";

  // dilepton plots
  fTH1DMap["zeta"]  = Analysis::MakeTH1DPlot("zeta","",30,-3.,3.,"Dimuon #eta","Events"); 
  fTH1DMap["zmass"] = Analysis::MakeTH1DPlot("zmass","",60,60.,120.,"Dimuon Mass [GeV/c^{2}]","Events / GeV/c^{2}");
  fTH1DMap["zphi"]  = Analysis::MakeTH1DPlot("zphi","",32,-3.2,3.2,"Dimuon #phi","Events"); 
  fTH1DMap["zpt"]   = Analysis::MakeTH1DPlot("zpt","",50,0.,1000.,"Dimuon p_{T} [GeV/c]","Events / 20 GeV/c"); 
  fTH1DSubDMap["zeta"]  = "Leptons/Dileptons/";
  fTH1DSubDMap["zmass"] = "Leptons/Dileptons/";
  fTH1DSubDMap["zphi"]  = "Leptons/Dileptons/";
  fTH1DSubDMap["zpt"]   = "Leptons/Dileptons/";

  fTH1DMap["zeeeta"]  = Analysis::MakeTH1DPlot("zeeeta","",30,-3.,3.,"Dielectron #eta","Events"); 
  fTH1DMap["zeemass"] = Analysis::MakeTH1DPlot("zeemass","",60,60.,120.,"Dielectron Mass [GeV/c^{2}]","Events / GeV/c^{2}");
  fTH1DMap["zeephi"]  = Analysis::MakeTH1DPlot("zeephi","",32,-3.2,3.2,"Dielectron #phi","Events"); 
  fTH1DMap["zeept"]   = Analysis::MakeTH1DPlot("zeept","",50,0.,1000.,"Dielectron p_{T} [GeV/c]","Events / 20 GeV/c"); 
  fTH1DSubDMap["zeeeta"]  = "Leptons/Dileptons/";
  fTH1DSubDMap["zeemass"] = "Leptons/Dileptons/";
  fTH1DSubDMap["zeephi"]  = "Leptons/Dileptons/";
  fTH1DSubDMap["zeept"]   = "Leptons/Dileptons/";

  fTH1DMap["emueta"]  = Analysis::MakeTH1DPlot("emueta","",30,-3.,3.,"Electron-Muon #eta","Events"); 
  fTH1DMap["emumass"] = Analysis::MakeTH1DPlot("emumass","",60,60.,120.,"Electron-Muon Mass [GeV/c^{2}]","Events / GeV/c^{2}");
  fTH1DMap["emuphi"]  = Analysis::MakeTH1DPlot("emuphi","",32,-3.2,3.2,"Electron-Muon #phi","Events"); 
  fTH1DMap["emupt"]   = Analysis::MakeTH1DPlot("emupt","",50,0.,1000.,"Electron-Muon p_{T} [GeV/c]","Events / 20 GeV/c"); 
  fTH1DSubDMap["emueta"]  = "Leptons/Dileptons/";
  fTH1DSubDMap["emumass"] = "Leptons/Dileptons/";
  fTH1DSubDMap["emuphi"]  = "Leptons/Dileptons/";
  fTH1DSubDMap["emupt"]   = "Leptons/Dileptons/";

  // Jet plots
  fTH1DMap["njets"] = Analysis::MakeTH1DPlot("njets","",10,0,10,"Jet Multiplicity","Events");
  fTH1DMap["ht"]    = Analysis::MakeTH1DPlot("ht","",50,0.,1000.,"H_{T} [GeV/c]","Events / 20 GeV/c"); 
  fTH1DSubDMap["njets"] = "Jets/";
  fTH1DSubDMap["ht"]    = "Jets/";

  // leading jet plots
  fTH1DMap["signaljeteta"]     = Analysis::MakeTH1DPlot("signaljeteta","",30,-3.,3.,"Leading Jet #eta","Events"); 
  fTH1DMap["signaljetphi"]     = Analysis::MakeTH1DPlot("signaljetphi","",32,-3.2,3.2,"Leading Jet #phi","Events"); 
  fTH1DMap["signaljetpt"]      = Analysis::MakeTH1DPlot("signaljetpt","",50,0.,1000.,"Leading Jet p_{T} [GeV/c]","Events / 20 GeV/c"); 
  fTH1DMap["signaljetCHfrac"]  = Analysis::MakeTH1DPlot("signaljetCHfrac","",50,0.,1.,"Leading Jet CH Fraction","Events"); 
  fTH1DMap["signaljetNHfrac"]  = Analysis::MakeTH1DPlot("signaljetNHfrac","",50,0.,1.,"Leading Jet NH Fraction","Events"); 
  fTH1DMap["signaljetEMfrac"]  = Analysis::MakeTH1DPlot("signaljetEMfrac","",50,0.,1.,"Leading Jet Neutral EM Fraction","Events"); 
  fTH1DMap["signaljetCEMfrac"] = Analysis::MakeTH1DPlot("signaljetCEMfrac","",50,0.,1.,"Leading Jet Charged EM Fraction","Events"); 
  fTH1DSubDMap["signaljeteta"]     = "Jets/Leading/";
  fTH1DSubDMap["signaljetphi"]     = "Jets/Leading/";
  fTH1DSubDMap["signaljetpt"]      = "Jets/Leading/";
  fTH1DSubDMap["signaljetCHfrac"]  = "Jets/Leading/";
  fTH1DSubDMap["signaljetNHfrac"]  = "Jets/Leading/";
  fTH1DSubDMap["signaljetEMfrac"]  = "Jets/Leading/";
  fTH1DSubDMap["signaljetCEMfrac"] = "Jets/Leading/";

  // subleading jet plots
  fTH1DMap["secondjeteta"]     = Analysis::MakeTH1DPlot("secondjeteta","",30,-3.,3.,"Subleading Jet #eta","Events"); 
  fTH1DMap["secondjetphi"]     = Analysis::MakeTH1DPlot("secondjetphi","",32,-3.2,3.2,"Subleading Jet #phi","Events"); 
  fTH1DMap["secondjetpt"]      = Analysis::MakeTH1DPlot("secondjetpt","",50,0.,1000.,"Subleading Jet p_{T} [GeV/c]","Events / 20 GeV/c"); 
  fTH1DMap["secondjetCHfrac"]  = Analysis::MakeTH1DPlot("secondjetCHfrac","",50,0.,1.,"Subleading Jet CH Fraction","Events"); 
  fTH1DMap["secondjetNHfrac"]  = Analysis::MakeTH1DPlot("secondjetNHfrac","",50,0.,1.,"Subleading Jet NH Fraction","Events"); 
  fTH1DMap["secondjetEMfrac"]  = Analysis::MakeTH1DPlot("secondjetEMfrac","",50,0.,1.,"Subleading Jet Neutral EM Fraction","Events"); 
  fTH1DMap["secondjetCEMfrac"] = Analysis::MakeTH1DPlot("secondjetCEMfrac","",50,0.,1.,"Subleading Jet Charged EM Fraction","Events"); 
  fTH1DSubDMap["secondjeteta"]     = "Jets/Subleading/";
  fTH1DSubDMap["secondjetphi"]     = "Jets/Subleading/";
  fTH1DSubDMap["secondjetpt"]      = "Jets/Subleading/";
  fTH1DSubDMap["secondjetCHfrac"]  = "Jets/Subleading/";
  fTH1DSubDMap["secondjetNHfrac"]  = "Jets/Subleading/";
  fTH1DSubDMap["secondjetEMfrac"]  = "Jets/Subleading/";
  fTH1DSubDMap["secondjetCEMfrac"] = "Jets/Subleading/";

  // subsubleading jet plots
  fTH1DMap["thirdjeteta"]      = Analysis::MakeTH1DPlot("thirdjeteta","",30,-3.,3.,"Subsubleading Jet #eta","Events"); 
  fTH1DMap["thirdjetphi"]      = Analysis::MakeTH1DPlot("thirdjetphi","",32,-3.2,3.2,"Subsubleading Jet #phi","Events"); 
  fTH1DMap["thirdjetpt"]       = Analysis::MakeTH1DPlot("thirdjetpt","",50,0.,1000.,"Subsubleading Jet p_{T} [GeV/c]","Events / 20 GeV/c"); 
  fTH1DMap["thirdjetCHfrac"]   = Analysis::MakeTH1DPlot("thirdjetCHfrac","",50,0.,1.,"Subsubleading Jet CH Fraction","Events"); 
  fTH1DMap["thirdjetNHfrac"]   = Analysis::MakeTH1DPlot("thirdjetNHfrac","",50,0.,1.,"Subsubleading Jet NH Fraction","Events"); 
  fTH1DMap["thirdjetEMfrac"]   = Analysis::MakeTH1DPlot("thirdjetEMfrac","",50,0.,1.,"Subsubleading Jet Neutral EM Fraction","Events"); 
  fTH1DMap["thirdjetCEMfrac"]  = Analysis::MakeTH1DPlot("thirdjetCEMfrac","",50,0.,1.,"Subsubleading Jet Charged EM Fraction","Events"); 
  fTH1DSubDMap["thirdjeteta"]     = "Jets/Subsubleading/";
  fTH1DSubDMap["thirdjetphi"]     = "Jets/Subsubleading/";
  fTH1DSubDMap["thirdjetpt"]      = "Jets/Subsubleading/";
  fTH1DSubDMap["thirdjetCHfrac"]  = "Jets/Subsubleading/";
  fTH1DSubDMap["thirdjetNHfrac"]  = "Jets/Subsubleading/";
  fTH1DSubDMap["thirdjetEMfrac"]  = "Jets/Subsubleading/";
  fTH1DSubDMap["thirdjetCEMfrac"] = "Jets/Subsubleading/";

  // MET plots
  fTH1DMap["pfmet"]   = Analysis::MakeTH1DPlot("pfmet","",50,0.,1000.,"PF E_{T}^{Miss} [GeV]","Events / 20 GeV"); 
  fTH1DMap["t1pfmet"] = Analysis::MakeTH1DPlot("t1pfmet","",50,0.,1000.,"Type-1 PF E_{T}^{Miss} [GeV]","Events / 20 GeV"); 
  fTH1DMap["mumet"]   = Analysis::MakeTH1DPlot("mumet","",50,0.,1000.,"PF E_{T}^{Miss} without Muons [GeV]","Events / 20 GeV");
  fTH1DMap["t1mumet"] = Analysis::MakeTH1DPlot("t1mumet","",50,0.,1000.,"Type-1 PF E_{T}^{Miss} without Muons [GeV]","Events / 20 GeV"); 
  fTH1DMap["mht"]     = Analysis::MakeTH1DPlot("mht","",50,0.,1000.,"E_{T}^{Miss} from H_{T} [GeV/c]","Events / 20 GeV/c"); 
  fTH1DSubDMap["pfmet"]   = "MET/";
  fTH1DSubDMap["t1pfmet"] = "MET/";
  fTH1DSubDMap["mumet"]   = "MET/";
  fTH1DSubDMap["t1mumet"] = "MET/";
  fTH1DSubDMap["mht"]     = "MET/";

  fTH1DMap["pfmetphi"]   = Analysis::MakeTH1DPlot("pfmetphi","",32,-3.2,3.2,"PF E_{T}^{Miss} #phi","Events"); 
  fTH1DMap["t1pfmetphi"] = Analysis::MakeTH1DPlot("t1pfmetphi","",32,-3.2,3.2,"Type-1 PF E_{T}^{Miss} #phi","Events"); 
  fTH1DMap["mumetphi"]   = Analysis::MakeTH1DPlot("mumetphi","",32,-3.2,3.2,"PF E_{T}^{Miss} without Muons #phi","Events");  
  fTH1DMap["t1mumetphi"] = Analysis::MakeTH1DPlot("t1mumetphi","",32,-3.2,3.2,"Type-1 PF E_{T}^{Miss} without Muons #phi","Events");  
  fTH1DSubDMap["pfmetphi"]   = "MET/";
  fTH1DSubDMap["t1pfmetphi"] = "MET/";
  fTH1DSubDMap["mumetphi"]   = "MET/";
  fTH1DSubDMap["t1mumetphi"] = "MET/";
}

TH1D * Analysis::MakeTH1DPlot(const TString hname, const TString htitle, const Int_t nbins, const Double_t xlow, const Double_t xhigh, const TString xtitle, const TString ytitle) {
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
      if ( (fSample.Contains("ww",TString::kExact)) || (fSample.Contains("wz",TString::kExact)) || (fSample.Contains("zz",TString::kExact)) ){
	(*mapiter).second->SetLineColor(fColorMap["diboson"]);
	(*mapiter).second->SetFillColor(fColorMap["diboson"]);
      }
      else if ( (fSample.Contains("ttbar",TString::kExact)) || (fSample.Contains("singlett",TString::kExact)) || (fSample.Contains("singletbart",TString::kExact)) || (fSample.Contains("singletw",TString::kExact)) || (fSample.Contains("singletbarw",TString::kExact)) ) {
	(*mapiter).second->SetLineColor(fColorMap["top"]);
	(*mapiter).second->SetFillColor(fColorMap["top"]);
      }
      else if (fSample.Contains("qcd",TString::kExact)) {
	(*mapiter).second->SetLineColor(fColorMap["qcd"]);
	(*mapiter).second->SetFillColor(fColorMap["qcd"]);
      }
      else if (fSample.Contains("gamma",TString::kExact)) {
	(*mapiter).second->SetLineColor(fColorMap["gamma"]);
	(*mapiter).second->SetFillColor(fColorMap["gamma"]);
      }
      else {
	(*mapiter).second->SetLineColor(fColorMap[fSample]);
	(*mapiter).second->SetFillColor(fColorMap[fSample]);
      }
      
      (*mapiter).second->Draw("HIST");
    }
    else {
      (*mapiter).second->Draw("PE");
    }

    // first save as log, then linear
    canv->SetLogy(0);
    canv->SaveAs(Form("%s/%s%s/%s/%s%s_lin.%s",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data(),fTH1DSubDMap[(*mapiter).first].Data(),(*mapiter).first.Data(),fOutType.Data()));

    canv->SetLogy(1);
    canv->SaveAs(Form("%s/%s%s/%s/%s%s_log.%s",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutName.Data(),fTH1DSubDMap[(*mapiter).first].Data(),(*mapiter).first.Data(),fOutType.Data()));
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

void Analysis::DeleteBranches() {
  delete b_event;   //!
  delete b_run;   //!
  delete b_lumi;   //!
  delete b_xsec;   //!
  delete b_wgt;   //!
  delete b_kfact;   //!
  delete b_puwgt;   //!
  delete b_puobs;   //!
  delete b_putrue;   //!
  delete b_nvtx;   //!
  delete b_hltmet90;   //!
  delete b_hltmet120;   //!
  delete b_hltmetwithmu90;   //!
  delete b_hltmetwithmu120;   //!
  delete b_hltmetwithmu170;   //!
  delete b_hltmetwithmu300;   //!
  delete b_hltjetmet90;   //!
  delete b_hltjetmet120;   //!
  delete b_hltphoton165;   //!
  delete b_hltphoton175;   //!
  delete b_hltdoublemu;   //!
  delete b_hltsinglemu;   //!
  delete b_hltdoubleel;   //!
  delete b_hltsingleel;   //!
  delete b_flagcsctight;   //!
  delete b_flaghbhenoise;   //!
  delete b_flaghcallaser;   //!
  delete b_flagecaltrig;   //!
  delete b_flageebadsc;   //!
  delete b_flagecallaser;   //!
  delete b_flagtrkfail;   //!
  delete b_flagtrkpog;   //!
  delete b_flaghnoiseloose;   //!
  delete b_flaghnoisetight;   //!
  delete b_flaghnoisehilvl;   //!
  delete b_nmuons;   //!
  delete b_nelectrons;   //!
  delete b_ntightmuons;   //!
  delete b_ntightelectrons;   //!
  delete b_ntaus;   //!
  delete b_njets;   //!
  delete b_nbjets;   //!
  delete b_nfatjets;   //!
  delete b_nphotons;   //!
  delete b_pfmet;   //!
  delete b_pfmetphi;   //!
  delete b_t1pfmet;   //!
  delete b_t1pfmetphi;   //!
  delete b_pfmupt;   //!
  delete b_pfmuphi;   //!
  delete b_mumet;   //!
  delete b_mumetphi;   //!
  delete b_phmet;   //!
  delete b_phmetphi;   //!
  delete b_t1mumet;   //!
  delete b_t1mumetphi;   //!
  delete b_t1phmet;   //!
  delete b_t1phmetphi;   //!
  delete b_fatjetpt;   //!
  delete b_fatjeteta;   //!
  delete b_fatjetphi;   //!
  delete b_fatjetprmass;   //!
  delete b_fatjetsdmass;   //!
  delete b_fatjettrmass;   //!
  delete b_fatjetftmass;   //!
  delete b_fatjettau2;   //!
  delete b_fatjettau1;   //!
  delete b_fatjetCHfrac;   //!
  delete b_fatjetNHfrac;   //!
  delete b_fatjetEMfrac;   //!
  delete b_fatjetCEMfrac;   //!
  delete b_fatjetmetdphi;   //!
  delete b_signaljetpt;   //!
  delete b_signaljeteta;   //!
  delete b_signaljetphi;   //!
  delete b_signaljetbtag;   //!
  delete b_signaljetCHfrac;   //!
  delete b_signaljetNHfrac;   //!
  delete b_signaljetEMfrac;   //!
  delete b_signaljetCEMfrac;   //!
  delete b_signaljetmetdphi;   //!
  delete b_secondjetpt;   //!
  delete b_secondjeteta;   //!
  delete b_secondjetphi;   //!
  delete b_secondjetbtag;   //!
  delete b_secondjetCHfrac;   //!
  delete b_secondjetNHfrac;   //!
  delete b_secondjetEMfrac;   //!
  delete b_secondjetCEMfrac;   //!
  delete b_secondjetmetdphi;   //!
  delete b_thirdjetpt;   //!
  delete b_thirdjeteta;   //!
  delete b_thirdjetphi;   //!
  delete b_thirdjetbtag;   //!
  delete b_thirdjetCHfrac;   //!
  delete b_thirdjetNHfrac;   //!
  delete b_thirdjetEMfrac;   //!
  delete b_thirdjetCEMfrac;   //!
  delete b_thirdjetmetdphi;   //!
  delete b_jetjetdphi;   //!
  delete b_jetmetdphimin;   //!
  delete b_incjetmetdphimin;   //!
  delete b_ht;   //!
  delete b_dht;   //!
  delete b_mht;   //!
  delete b_alphat;   //!
  delete b_apcjetmetmax;   //!
  delete b_apcjetmetmin;   //!
  delete b_mu1pid;   //!
  delete b_mu1pt;   //!
  delete b_mu1eta;   //!
  delete b_mu1phi;   //!
  delete b_mu1id;   //!
  delete b_mu2pid;   //!
  delete b_mu2pt;   //!
  delete b_mu2eta;   //!
  delete b_mu2phi;   //!
  delete b_mu2id;   //!
  delete b_el1pid;   //!
  delete b_el1pt;   //!
  delete b_el1eta;   //!
  delete b_el1phi;   //!
  delete b_el1id;   //!
  delete b_el2pid;   //!
  delete b_el2pt;   //!
  delete b_el2eta;   //!
  delete b_el2phi;   //!
  delete b_el2id;   //!
  delete b_zmass;   //!
  delete b_zpt;   //!
  delete b_zeta;   //!
  delete b_zphi;   //!
  delete b_wmt;   //!
  delete b_emumass;   //!
  delete b_emupt;   //!
  delete b_emueta;   //!
  delete b_emuphi;   //!
  delete b_zeemass;   //!
  delete b_zeeept;   //!
  delete b_zeeeta;   //!
  delete b_zeephi;   //!
  delete b_wemt;   //!
  delete b_phpt;   //!
  delete b_pheta;   //!
  delete b_phphi;   //!
  delete b_loosephpt;   //!
  delete b_loosepheta;   //!
  delete b_loosephphi;   //!
  delete b_loosephsieie;   //!
  delete b_loosephrndiso;   //!
  delete b_wzid;   //!
  delete b_wzmass;   //!
  delete b_wzmt;   //!
  delete b_wzpt;   //!
  delete b_wzeta;   //!
  delete b_wzphi;   //!
  delete b_l1id;   //!
  delete b_l1pt;   //!
  delete b_l1eta;   //!
  delete b_l1phi;   //!
  delete b_l2id;   //!
  delete b_l2pt;   //!
  delete b_l2eta;   //!
  delete b_l2phi;   //!
  delete b_i1id;   //!
  delete b_i1pt;   //!
  delete b_i1eta;   //!
  delete b_i1phi;   //!
  delete b_i2id;   //!
  delete b_i2pt;   //!
  delete b_i2eta;   //!
  delete b_i2phi;   //!
  delete b_i3id;   //!
  delete b_i3pt;   //!
  delete b_i3eta;   //!
  delete b_i3phi;   //!
  delete b_cflagcsctight;   //!
  delete b_cflaghbhenoise;   //!
  delete b_cflaghcallaser;   //!
  delete b_cflagecaltrig;   //!
  delete b_cflageebadsc;   //!
  delete b_cflagecallaser;   //!
  delete b_cflagtrkfail;   //!
  delete b_cflagtrkpog;   //!
  delete b_wgtsum;   //!
}
