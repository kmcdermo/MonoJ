void hack_datawgtsum(TString dir) {
  TFile* infile = new TFile(Form("Data/%s/tree.root",dir.Data()));
  TTree* frtree = (TTree*)infile->Get("tree/tree");
  if (!frtree) return;

  double wgtsum = 1.0;
  TBranch* bwgtsum = frtree->Branch("wgtsum", &wgtsum, "wgtsum/D");

  for (Long64_t i = 0; i < frtree->GetEntries(); i++){
    bwgtsum->Fill();
  }

  TFile* outfile = new TFile(Form("Data/%s/treewithwgt.root",dir.Data()),"RECREATE");
  outfile->cd();
  TDirectoryFile* treedir = new TDirectoryFile("tree", "tree");
  treedir->cd();
  TTree* outtree = frtree->CopyTree("");
  outfile->Write();

  infile->Close();
  outfile->Close();
}
