#include "../interface/PUReweight.hh"

PUReweight::PUReweight(SamplePairVec Samples, const TString selection, const Double_t lumi, const Int_t nBins, const TString outdir, const TString outtype) {
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

  // save lumi
  fLumi = lumi;

  // set nBins for nvtx distribution
  fNBins = nBins;

  // set outputs
  fOutDir  = outdir;
  fOutType = outtype;

  // Initialize input TH1D's for data
  fInDataNvtx.resize(fNData);
  for (UInt_t data = 0; data < fNData; data++){
    fInDataNvtx[data] = new TH1D(Form("nvtx_%s",fDataNames[data].Data()),"",fNBins,0,fNBins);
    fInDataNvtx[data]->Sumw2();
  }
  // output th1d
  fOutDataNvtx      = new TH1D("nvtx_data","",fNBins,0,fNBins);
  fOutDataNvtx_copy = new TH1D("nvtx_data_copy","",fNBins,0,fNBins);

  // Initialize TH1D's for MC
  fInMCNvtx.resize(fNMC);
  for (UInt_t mc = 0; mc < fNMC; mc++){
    fInMCNvtx[mc] = new TH1D(Form("nvtx_%s",fMCNames[mc].Data()),"",fNBins,0,fNBins);
    fInMCNvtx[mc]->Sumw2();
  }
  // output th1d
  fOutMCNvtx = new TH1D("nvtx_mc","",fNBins,0,fNBins);
}

PUReweight::~PUReweight(){
  // delete data hists
  for (UInt_t data = 0; data < fNData; data++){
    delete fInDataNvtx[data];
  }
  delete fOutDataNvtx;
  delete fOutDataNvtx_copy;

  // delete MC hists
  for (UInt_t mc = 0; mc < fNMC; mc++){
    delete fInMCNvtx[mc];
  }
  delete fOutMCNvtx;
}

DblVec PUReweight::GetPUWeights(const Bool_t doPUReWeight){

  DblVec puweights; // return weights
  
  if (doPUReWeight) { // if true, perform reweighting procedure

    TString basecut; // selection based cut
    if (fSelection.Contains("zmumu"),TString::kExact) {
      basecut = "((hltdoublemu > 0) && (mu1pt > 20) && (mu1id == 1) && (zmass < 120.) && (zmass > 60.) && (mu1pid == -mu2pid))";
    }
    else if (fSelection.Contains("zelel"),TString::kExact) {
      basecut = "((hltdoubleel > 0) && (el1pt > 20) && (el1id == 1) && (zeemass < 120.) && (zeemass > 60.) && (el1pid == -el2pid))";
    }
    else if (fSelection.Contains("singlemu"),TString::kExact) {
      basecut = "((hltsinglemu == 1) && (nmuons == 1) && (mu1pt > 30) && (mu1id == 1))"; 
    }      

    // get vtx distribution for data first
    for (UInt_t data = 0; data < fNData; data++){
    
      // create appropriate selection cut
      TString cut = basecut;
      cut.Prepend("( ");
      cut.Append("&& (cflagcsctight == 1 && cflaghbhenoise == 1) )"); // met filters for data
      
      // files + trees for data
      TFile * file = TFile::Open(Form("root://eoscms//eos/cms/store/user/kmcdermo/MonoX/Trees/Data/%s/treewithwgt.root",fDataNames[data].Data()));
      TTree * tree = (TTree*)file->Get("tree/tree");      
      
      // fill each input data nvtx
      tree->Draw(Form("nvtx>>nvtx_%s",fDataNames[data].Data()),cut.Data(),"goff");

      // add input data hist to total data hist
      fOutDataNvtx->Add(fInDataNvtx[data]);

      // delete objects
      delete tree;
      delete file;
    }

    // get vtx distribution for mc second
    for (UInt_t mc = 0; mc < fNMC; mc++){
    
      // create appropriate selection cut
      TString cut = basecut;
      cut.Prepend("( ");
      cut.Append(Form("&& (flagcsctight == 1 && flaghbhenoise == 1) ) * (xsec * %f * wgt / wgtsum)",fLumi)); // met filters for mc + weights
      
      // files + trees for mc
      TFile * file = TFile::Open(Form("root://eoscms//eos/cms/store/user/kmcdermo/MonoX/Trees/Spring15MC_50ns/%s/treewithwgt.root",fMCNames[mc].Data()));
      TTree * tree = (TTree*)file->Get("tree/tree");      
      
      // fill each input mc nvtx
      tree->Draw(Form("nvtx>>nvtx_%s",fMCNames[mc].Data()),cut.Data(),"goff");

      // add input mc hist to total mc hist
      fOutMCNvtx->Add(fInMCNvtx[mc]);

      // delete objects
      delete tree;
      delete file;
    }
    
    // scale to unit area to not bias against data
    fOutDataNvtx->Scale(1.0/fOutDataNvtx->Integral());  
    fOutMCNvtx->Scale(1.0/fOutMCNvtx->Integral());

    // Draw before reweighting
    TCanvas * c1 = new TCanvas();
    c1->SetTitle("Before PU Reweighting");
    c1->cd();
    c1->SetLogy(1);
  
    fOutDataNvtx->SetLineColor(kRed);
    fOutMCNvtx->SetLineColor(kBlue);

    // draw and save
    fOutDataNvtx->Draw("PE");
    fOutMCNvtx->Draw("HIST SAME");
    c1->SaveAs(Form("%s/nvtx_beforePURW.%s",fOutDir.Data(),fOutType.Data()));

    // Draw after reweighting 
    TCanvas * c2 = new TCanvas();
    c2->SetTitle("After PU Reweighting");
    c2->cd();
    c2->SetLogy(1);

    // copy fOutDataNvtx to save output of reweights properly
    fOutDataNvtx_copy = (TH1D*)fOutDataNvtx->Clone();
  
    // divide Data/MC after copy, now this hist will be used for reweighting 
    fOutDataNvtx->Divide(fOutMCNvtx);

    // push back reweights and then scale MC to demonstrate that it works
    for (Int_t ibin = 1; ibin <= fNBins; ibin++) {
      // push back reweights 
      puweights.push_back(fOutDataNvtx->GetBinContent(ibin)); 

      // scale MC appropriately
      Double_t tmp = fOutMCNvtx->GetBinContent(ibin);
      fOutMCNvtx->SetBinContent(ibin,puweights[ibin-1]*tmp); 
    }

    // draw output and save it
    fOutDataNvtx_copy->Draw("PE");
    fOutMCNvtx->Draw("HIST SAME");
    c2->SaveAs(Form("%s/nvtx_afterPURW.%s",fOutDir.Data(),fOutType.Data()));

    delete c1;
    delete c2;
  } // do the reweight
  
  else{ // skip reweighting ... set weights to 1.0
    for (Int_t i = 1; i <= fNBins; i++ ){
      puweights.push_back(1.0);
    }
  }

  // return the weights regardless
  return puweights;
}

