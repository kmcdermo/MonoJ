void test(){

  TFile * file = TFile::Open("root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/zll/treewithwgt.root");
  TTree * tree = (TTree*)file->Get("tree/tree");

  UChar_t         hltdoublemu;
  UChar_t         hltsinglemu;

  Int_t           mu1id;
  Double_t        mu1pt;

  Double_t        zmass;

  Int_t           mu1pid;
  Int_t           mu2pid;

  UChar_t         flaghbhenoise;
  UChar_t         flagcsctight;

  Double_t        xsec;
  Double_t        wgt;
  Double_t        wgtsum;

  UInt_t          nvtx;
  TBranch *b_nvtx;

  tree->SetBranchAddress("nvtx", &nvtx, b_nvtx);

  TBranch *b_hltdoublemu;
  TBranch *b_hltsinglemu;

  TBranch *b_mu1id;
  TBranch *b_mu1pt;

  TBranch *b_zmass;

  TBranch *b_mu1pid;
  TBranch *b_mu2pid;

  TBranch *b_flaghbhenoise;
  TBranch *b_flagcsctight;

  TBranch *b_xsec;
  TBranch *b_wgt;
  TBranch *b_wgtsum;


  tree->SetBranchAddress("hltdoublemu", &hltdoublemu, b_hltdoublemu);
  tree->SetBranchAddress("hltsinglemu", &hltsinglemu, b_hltsinglemu);
  tree->SetBranchAddress("mu1id", &mu1id, b_mu1id);
  tree->SetBranchAddress("mu1pt", &mu1pt, b_mu1pt);
  tree->SetBranchAddress("zmass", &zmass, b_zmass);
  tree->SetBranchAddress("mu1pid", &mu1pid, b_mu1pid);
  tree->SetBranchAddress("mu2pid", &mu2pid, b_mu2pid);
  tree->SetBranchAddress("flaghbhenoise", &flaghbhenoise, b_flaghbhenoise);
  tree->SetBranchAddress("flagcsctight", &flagcsctight, b_flagcsctight);

  tree->SetBranchAddress("xsec", &xsec, b_xsec);
  tree->SetBranchAddress("wgt", &wgt, b_wgt);
  tree->SetBranchAddress("wgtsum", &wgtsum, b_wgtsum);

  TH1D * NVTX=new TH1D("","",50,0,50);
  NVTX->Sumw2();

  for (UInt_t entry = 0; entry < tree->GetEntries(); entry++) {
    tree->GetEntry(entry);

    Double_t weight = xsec * 0.03650 * wgt / wgtsum;  
    
    if ( (hltdoublemu > 0 || hltsinglemu > 0) && (mu1pt > 20) && (mu1id == 1) && (zmass < 120.) && (zmass > 60.) && (mu1pid == -mu2pid) ) { // plot valid zmass for dimuons
      if ( flagcsctight == 1 && flaghbhenoise == 1 ){
	
	//	std::cout << nvtx << std::endl;
	NVTX->Fill(nvtx,weight);
      }
    }
    
  }

  TCanvas * c1 =new TCanvas();
  c1->cd();
  NVTX->Draw();

}
