#include <vector>

void addpuwgtBranch(TTree *& tree, std::vector<Double_t> puweights);

void quickplot(){
  /////////////////////////////////////
  //                                 //
  //       OPEN FILES AND TREES      //
  //                                 //
  /////////////////////////////////////

  // choose local
  const Bool_t local = true;

  // set basenames
  TString fileNameBase = "";
  if (!local) {
    fileNameBase = "root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/";
  }
  else {
    fileNameBase = "";
  }

  //file 1 --> Data
  TFile * f1 = TFile::Open(Form("%sData/doublemu/treewithwgt.root",fileNameBase.Data())); // Data doublemu
  std::cout << "Opened file: " << Form("%sData/doublemu/treewithwgt.root",fileNameBase.Data()) << std::endl;
  TTree * t1 = (TTree*)f1->Get("tree/tree");
  if (f1 == (TFile*) NULL) { std::cout << "bad file... exiting" << std::endl; exit(1); }
  if (t1 == (TTree*) NULL) { std::cout << "bad tree... exiting" << std::endl; exit(1); }

  //file 2 --> MC
  TFile * f2 = TFile::Open(Form("%sMC/zll/treewithwgt.root",fileNameBase.Data())); // MC zll
  std::cout << "Opened file: " << Form("%sSpring15MC_50ns/zll/treewithwgt.root",fileNameBase.Data()) << std::endl;
  TTree * t2 = (TTree*)f2->Get("tree/tree");
  if (f2 == (TFile*) NULL) { std::cout << "bad file... exiting" << std::endl; exit(1); }
  if (t2 == (TTree*) NULL) { std::cout << "bad tree... exiting" << std::endl; exit(1); }

  //file 3 --> MC
  TFile * f3 = TFile::Open(Form("%sMC/ttbar/treewithwgt.root",fileNameBase.Data())); // MC ttbar
  std::cout << "Opened file: " << Form("%sSpring15MC_50ns/ttbar/treewithwgt.root",fileNameBase.Data()) << std::endl;
  TTree * t3 = (TTree*)f3->Get("tree/tree");
  if (f3 == (TFile*) NULL) { std::cout << "bad file... exiting" << std::endl; exit(1); }
  if (t3 == (TTree*) NULL) { std::cout << "bad tree... exiting" << std::endl; exit(1); }

  /////////////////////////////////////
  //                                 //
  //          SET SELECTIONS         //
  //                                 //
  /////////////////////////////////////

  // Base Selection
  TString baseSelection = "((hltdoublemu > 0) && (mu1pt > 20) && (mu1id == 1) && (zmass < 120.) && (zmass > 60.) && (mu1pid == -mu2pid))";

  // Njets Selection
  TString njetsSelection = " && (njets >= 0)";

  // set PU RW selections --> njets inclusive
  TString dataPUSelection = baseSelection;
  dataPUSelection.Prepend("( ");
  dataPUSelection.Append(" && ((cflagcsctight == 1) && (cflaghbhenoise == 1)) )"); // met filters for data

  TString mcPUSelection = baseSelection;
  mcPUSelection.Prepend("( ");
  mcPUSelection.Append(" && ((flagcsctight == 1) && (flaghbhenoise == 1)) )"); // met filters for MC

  // set analysis selections ... includes njets cuts!
  TString dataANSelection = baseSelection;
  dataANSelection.Prepend("( ");
  dataANSelection.Append(njetsSelection.Data()); // add njets selection
  dataANSelection.Append(" && ((cflagcsctight == 1) && (cflaghbhenoise == 1)) )"); // met filters for data

  TString mcANSelection = baseSelection;
  mcANSelection.Append(njetsSelection.Data()); // add njets selection
  mcANSelection.Append(" && ((flagcsctight == 1) && (flaghbhenoise == 1)) )"); // met filters for MC
  
  /////////////////////////////////////
  //                                 //
  //            DO PURW              //
  //                                 //
  /////////////////////////////////////

  const Bool_t doReweight = true;
  if (doReweight) {
    TBranch * bpuwgt2 = t2->GetBranch("puwgt");
    TBranch * bpuwgt3 = t3->GetBranch("puwgt");
    //    if ((bpuwgt2 == (TBranch*) NULL) || (bpuwgt3 == (TBranch*) NULL)) { // if branches do not exist, do the reweighting!
      std::cout << "Doing PU Reweight" << std::endl;

      TH1D * nvtx1 = new TH1D("nvtx1","",50,0,50); // double mu data
      nvtx1->Sumw2();
      TH1D * nvtx2 = new TH1D("nvtx2","",50,0,50); // zll mc
      nvtx2->Sumw2();

      // fill data
      t1->Draw("nvtx>>nvtx1",Form("%s",dataPUSelection.Data()),"goff");
      // fill mc
      t2->Draw("nvtx>>nvtx2",Form("%s * (wgt * 0.04003 * xsec / wgtsum)",mcPUSelection.Data()),"goff");
      
      // once filled, scale then divide data by MC
      nvtx1->Scale(1.0/nvtx1->Integral());
      nvtx2->Scale(1.0/nvtx2->Integral());

      nvtx1->Divide(nvtx2);
      
      // now store into tmp vector
      std::vector<Double_t> puweights;
      for (Int_t ibin = 1; ibin <= 50; ibin++) {
	// push back reweights 
	puweights.push_back(nvtx1->GetBinContent(ibin)); 
      }
      
      // delete the histos first...
      delete nvtx1;
      delete nvtx2;
    
      // now add the puweights as a branch to the appropriate MC samples
      if (bpuwgt2 != (TBranch*) NULL) {
	std::cout << "first delete old puwgt" << std::endl;
	t2->GetListOfBranches()->Remove(bpuwgt2);
      }
      
      std::cout << "add puwgts to file:" << f2->GetName() << std::endl;
      addpuwgtBranch(t2,puweights);
      
    	
  
      //    }
    //    f2->Write()


  }

}

void addpuwgtBranch(TTree *& tree, std::vector<Double_t> puweights){
  Double_t puwgt = -99999;
  UInt_t nvtx = 0;

  if (tree == (TTree*) NULL) std::cout << "adskjfadkljfasdjlfafk" << std::endl;

  std::cout << "here" <<std::endl;
  TBranch *bpuwgt = tree->Branch("puwgt",&puwgt,"puwgt/D");

  std::cout << "here" <<std::endl;
  tree->SetBranchAddress("nvtx", &nvtx);

  std::cout << "here" <<std::endl;



  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("nvtx",1);
  tree->SetBranchStatus("puwgt",1);
  
  for (Int_t i = 0; i < tree->GetEntries(); i++){
    tree->GetEntry(i,0);
    puwgt = puweights[nvtx];
    bpuwgt->Fill();
  }
  
  //  tree->Write();

}

