#include "../interface/PUReweight.hh"

PUReweight::PUReweight(SamplePairVec Samples, const TString selection, const Int_t njetsselection, const Double_t lumi, const Int_t nBins, const TString outdir, const TString outtype, const Bool_t runLocal) {
  fRunLocal = runLocal;

  // save samples for PU weighting
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
  
  // string for output of njets... -1 == no requirment on njets
  fNJetsStr = "";
  if (fNJetsSeln != -1){
    fNJetsStr = Form("_nj%i",fNJetsSeln);
  }

  // save lumi
  fLumi = lumi;

  // set nBins for nvtx distribution
  fNBins = nBins;

  // set outputs
  fOutDir  = outdir;
  fOutType = outtype;

  // Initialize output TH1D's for data
  fOutDataNvtx = new TH1D("nvtx_data","",fNBins,0.,Double_t(fNBins));
  fOutDataNvtx->Sumw2();

  // Initialize outputs for MC
  fOutMCNvtx = new TH1D("nvtx_mc","",fNBins,0.,Double_t(fNBins));
  fOutMCNvtx->Sumw2();

  // Intialize Ratio Hist
  fOutDataOverMCNvtx = new TH1D("nvtx_dataOverMC","",fNBins,0.,Double_t(fNBins));
  fOutDataOverMCNvtx->Sumw2();

  // Initialize Output root file to be used by other macros ... eventually could integrate... no need now
  fOutFile = new TFile(Form("%s/PURW_%s%s.root",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data()),"RECREATE");
}

PUReweight::~PUReweight() {
  //delete hists
  delete fOutDataNvtx;
  delete fOutMCNvtx;
  delete fOutDataOverMCNvtx;

  //delete
  delete fOutFile;
}

DblVec PUReweight::GetPUWeights() {

  DblVec puweights; // return weights

  TString basecut; // selection based cut
  if (fSelection.Contains("zmumu",TString::kExact)) {
    basecut = "((hltdoublemu > 0) && (mu1pt > 20) && (mu1id == 1) && (zmass < 120.) && (zmass > 60.) && (mu1pid == -mu2pid))";
  }
  else if (fSelection.Contains("zelel",TString::kExact)) {
    basecut = "((hltdoubleel > 0) && (el1pt > 20) && (el1id == 1) && (zeemass < 120.) && (zeemass > 60.) && (el1pid == -el2pid))";
  }
  else if (fSelection.Contains("singlemu",TString::kExact)) {
    basecut = "((hltsinglemu == 1) && (nmuons == 1) && (mu1pt > 30) && (mu1id == 1))"; 
  }      
  else if (fSelection.Contains("singleel",TString::kExact)) {
    //    basecut = "((hltsingleel == 1) && (nelectrons == 1) && (el1pt > 30) && (el1id == 1))"; 
    basecut = "((nelectrons == 1) && (el1pt > 30) && (el1id == 1))"; 
  }      
  else if (fSelection.Contains("singlephoton",TString::kExact)) {    
    basecut = "((nphotons == 1) && (phpt > 200.))";
  }

  // add selection for njets
  if (fNJetsSeln != -1){
    basecut.Prepend(Form("(njets == %i) && ",fNJetsSeln));
  }

  // get vtx distribution for data first
  for (UInt_t data = 0; data < fNData; data++){
    
    // create appropriate selection cut
    TString cut = basecut.Data();
    cut.Prepend("( ");
    if (fSelection.Contains("singlephoton",TString::kExact)) { // annoying since data triggers != MC triggers
      cut.Append(" && ((hltphoton165 == 1) || (hltphoton175 == 1)) )");
    }
    else { // no photon triggers, and also no checks on these for photons as PHYS14MC sample is 50% efficient.
      cut.Append(" && ((cflagcsctight == 1) && (cflaghbhenoise == 1)) )"); // met filters for data
    }      

    // files + trees + tmp hist for data
    TString filename = "";
    if (fRunLocal) { // on Mac
      filename = Form("Data/%s/treewithwgt.root",fDataNames[data].Data());
    }
    else{ // pull from eos
     filename = Form("root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Data/%s/treewithwgt.root",fDataNames[data].Data());
    }
    TFile * file = TFile::Open(filename.Data());
    CheckValidFile(file,filename);
    TTree * tree = (TTree*)file->Get("tree/tree");      
    CheckValidTree(tree,"tree/tree,",filename);      
    TH1D * tmpnvtx = new TH1D("tmpnvtx","",fNBins,0.,Double_t(fNBins));
    tmpnvtx->Sumw2();

    // fill each input data nvtx
    std::cout << "Reading data nvtx: " << filename.Data() << " with cut: " << cut.Data() << std::endl;
    tree->Draw("nvtx>>tmpnvtx",Form("%s",cut.Data()),"goff");
    
    // add input data hist to total data hist
    fOutDataNvtx->Add(tmpnvtx);

    // delete objects
    delete tmpnvtx;
    delete tree;
    delete file;
  }

  // get vtx distribution for mc second
  for (UInt_t mc = 0; mc < fNMC; mc++){
    
    // create appropriate selection cut
    TString cut = basecut;
    cut.Prepend("( ");
    if (fSelection.Contains("singlephoton",TString::kExact)) { // no met filters for single photons from PHYS14
      cut.Append(" )");
    }
    else { // met filters for all other mc
      cut.Append(Form(" && ((flagcsctight == 1) && (flaghbhenoise == 1)) )"));
    }
      
    cut.Append(Form(" * (xsec * %f * wgt / wgtsum)",fLumi)); // make sure to add weights for all mc!
      
    // files + trees for mc + tmp hists
    TString filename = "";
    if (fRunLocal) { // run on Mac --> keep gamma and all others in "MC directory" for simplicity... may need to change
      filename = Form("MC/%s/treewithwgt.root",fMCNames[mc].Data());
    }
    else { // run on EOS
      if (fSelection.Contains("singlephoton",TString::kExact)) { // annoying since MC photon sits elsewhere
	filename = Form("root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/PHYS14MC/%s/treewithwgt.root",fMCNames[mc].Data());
      }
      else {
	filename = Form("root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/%s/treewithwgt.root",fMCNames[mc].Data());
      }
    }
    TFile * file = TFile::Open(filename.Data());
    CheckValidFile(file,filename);
    TTree * tree = (TTree*)file->Get("tree/tree");      
    CheckValidTree(tree,"tree/tree,",filename);            
    TH1D * tmpnvtx = new TH1D("tmpnvtx","",fNBins,0.,Double_t(fNBins));
    tmpnvtx->Sumw2();

    // fill each input mc nvtx
    std::cout << "Reading MC nvtx: " << filename.Data() << " with cut: " << cut.Data() << std::endl;
    tree->Draw("nvtx>>tmpnvtx",Form("%s",cut.Data()),"goff");

    // add input mc hist to total mc hist
    fOutMCNvtx->Add(tmpnvtx);

    // delete objects
    delete tmpnvtx;
    delete tree;
    delete file;
  }

  // Set line colors
  fOutDataNvtx->SetLineColor(kRed);
  fOutMCNvtx->SetLineColor(kBlue);
  
  // use these for scaling and rescaling
  const Double_t int_DataNvtx = fOutDataNvtx->Integral();
  const Double_t int_MCNvtx   = fOutMCNvtx->Integral();

  TCanvas * c0 = new TCanvas(); // Draw before reweighting --> unscaled
  c0->cd();
  c0->SetTitle("Before PU Reweighting Unnormalized");

  // draw and save in output directory --> appended by what selection we used for this pu reweight
  fOutDataNvtx->Draw("PE");
  fOutMCNvtx->Draw("HIST SAME");

  c0->SetLogy(1); // save log
  c0->SaveAs(Form("%s/nvtx_beforePURW_unnorm_%s%s_log.%s",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutType.Data()));

  c0->SetLogy(0); // save lin
  c0->SaveAs(Form("%s/nvtx_beforePURW_unnorm_%s%s_lin.%s",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutType.Data()));
  
  /////////////////////////////////////////////
  //       SCALE HERE TO GET REWEIGHTING     //
  /////////////////////////////////////////////
  // scale to unit area to not bias against data
  fOutDataNvtx->Scale(1.0/int_DataNvtx);  
  fOutMCNvtx->Scale(1.0/int_MCNvtx);

  // Draw before reweighting -- scaled
  TCanvas * c1 = new TCanvas();
  c1->cd();
  c1->SetTitle("Before PU Reweighting Normalized");

  // draw and save in output directory --> appended by what selection we used for this pu reweight
  fOutDataNvtx->Draw("PE");
  fOutMCNvtx->Draw("HIST SAME");

  c1->SetLogy(1); // save log
  c1->SaveAs(Form("%s/nvtx_beforePURW_norm_%s%s_log.%s",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutType.Data()));

  c1->SetLogy(0); // save lin
  c1->SaveAs(Form("%s/nvtx_beforePURW_norm_%s%s_lin.%s",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutType.Data()));

  /////////////////////////////////////////////
  //      DIVIDE HERE TO GET REWEIGHTING     //
  /////////////////////////////////////////////

  // copy fOutDataNvtx to save output of reweights properly
  for (Int_t ibin = 1; ibin <= fNBins; ibin++) {
    fOutDataOverMCNvtx->SetBinContent(ibin,fOutDataNvtx->GetBinContent(ibin));
  }

  // divide Data/MC after copy, now this original hist will be used for reweighting 
  fOutDataOverMCNvtx->Divide(fOutMCNvtx);

  /////////////////////////////////////////////
  //      STORE HERE TO USE REWEIGHTING      //
  /////////////////////////////////////////////

  // push back reweights and then scale MC to demonstrate that it works
  for (Int_t ibin = 1; ibin <= fNBins; ibin++) {
    // push back reweights 
    puweights.push_back(fOutDataOverMCNvtx->GetBinContent(ibin)); 

    // scale MC appropriately
    Double_t tmp = fOutMCNvtx->GetBinContent(ibin);
    fOutMCNvtx->SetBinContent(ibin,puweights[ibin-1]*tmp); 
  }

  fOutFile->cd();
  fOutDataOverMCNvtx->Write();

  // Draw after reweighting 
  TCanvas * c2 = new TCanvas();
  c2->cd();
  c2->SetTitle("After PU Reweighting Normalized");

  // draw output and save it, see comment above about selection
  fOutDataNvtx->Draw("PE");
  fOutMCNvtx->Draw("HIST SAME");

  c2->SetLogy(1); // save log
  c2->SaveAs(Form("%s/nvtx_afterPURW_norm_%s%s_log.%s",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutType.Data()));

  c2->SetLogy(0); // save lin
  c2->SaveAs(Form("%s/nvtx_afterPURW_norm_%s%s_lin.%s",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutType.Data()));

  TCanvas * c3 = new TCanvas(); // Draw before reweighting --> unscaled
  c3->cd();
  c3->SetTitle("After PU Reweighting Unnormalized"); 

  // now that the reweighting is applied, see total events again
  fOutDataNvtx->Scale(int_DataNvtx);
  fOutMCNvtx->Scale(int_MCNvtx);
  
  fOutDataNvtx->Draw("PE");
  fOutMCNvtx->Draw("HIST SAME");

  c3->SetLogy(1); // save log
  c3->SaveAs(Form("%s/nvtx_afterPURW_unnorm_%s%s_log.%s",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutType.Data()));

  c3->SetLogy(0); // save lin
  c3->SaveAs(Form("%s/nvtx_afterPURW_unnorm_%s%s_lin.%s",fOutDir.Data(),fSelection.Data(),fNJetsStr.Data(),fOutType.Data()));
  
  delete c0;
  delete c1;
  delete c2;
  delete c3;

  return puweights;
}

