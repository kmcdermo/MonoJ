void hack_datawgtsum(TString dir) {
  TFile* infile = new TFile(Form("Data/%s/tree.root",dir.Data()),"UPDATE");
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

  gSystem->Exec(Form("mv %s/tree.root %s/treewithwgt.root",dir.Data(),dir.Data()));

  std::cout << "Finished macro" << std::endl;
  
}
