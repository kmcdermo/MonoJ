void hack_datawgtsum(TString run, TString sample) {
  TFile* infile = new TFile(Form("Data/%s/%s/tree.root",run.Data(),sample.Data()),"UPDATE");
  TTree* frtree = (TTree*)infile->Get("tree/tree");
  if (!frtree) return;

  double wgtsum = 1.0;
  TBranch* bwgtsum = frtree->Branch("wgtsum", &wgtsum, "wgtsum/D");

  for (Long64_t i = 0; i < frtree->GetEntries(); i++){
    bwgtsum->Fill();
  }

  infile->Write();
  infile->Close();

  // moving name to with wgtsum
  std::cout << "Moving file from tree.root to treewithwgt.root" << std::endl;

  gSystem->Exec(Form("mv Data/%s/%s/tree.root Data/%s/%s/treewithwgt.root",run.Data(),sample.Data(),run.Data(),sample.Data()));

  std::cout << "Finished macro" << std::endl;
  
}
