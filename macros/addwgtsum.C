#include <iostream>

double sumwgt(TTree* tree) {
  TBranch *bweight = tree->GetBranch("wgtsign");

  double vweight  = 0.0;

  bweight->SetAddress(&vweight);

  double weightsum = 0.;
  for (int i = 0; i < tree->GetEntries(); i++) {
    bweight->GetEvent(i);
    weightsum += vweight;
  }

  return weightsum;
}

//void addwgtsum(TString sample) {
void addwgtsum(TString dir) {
  //  TFile* infile = TFile::Open(Form("root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/%s/tree.root",sample.Data()));
  TFile* infile = TFile::Open(Form("%s/tree.root",dir.Data()),"UPDATE");
  TTree* intree = (TTree*)infile->Get("gentree/gentree");
  TTree* frtree = (TTree*)infile->Get("tree/tree");
  if (!intree) intree = (TTree*)infile->Get("gentree");
  if (!frtree) frtree = (TTree*)infile->Get("tree");

  std::cout << "Adding up weights to get sum" << std::endl;
  
  double wgtsum = sumwgt(intree);
  TBranch* bwgtsum = frtree->Branch("wgtsum", &wgtsum, "wgtsum/D");
    
  std::cout << "Copy wgtsum branch into tree" << std::endl;

  for (Long64_t i = 0; i < frtree->GetEntries(); i++){
    bwgtsum->Fill();
  }

  //  std::string cut = "mumet > 200 || t1mumet > 200";

  /*  TFile* outfile = TFile::Open("treewithwgt.root","RECREATE");

  // for now do not need to copy gen tree, just tree with weights
  //  TDirectoryFile* gentreedir = new TDirectoryFile("gentree", "gentree");
  //  gentreedir->cd();
  //  TTree* outgentree = intree->CopyTree("");

  std::cout << "Copying tree into new root file without gen tree" << std::endl;

  outfile->cd();
  TDirectoryFile* treedir = new TDirectoryFile("tree", "tree");
  treedir->cd();
  TTree* outtree = frtree->CopyTree("");
  outfile->Write();
  */
  infile->Write();
  infile->Close();
  //  outfile->Close();

  // moving name to with wgtsum
  std::cout << "Moving file from tree.root to treewithwgt.root" << std::endl;

  gSystem->Exec(Form("mv %s/tree.root %s/treewithwgt.root",dir.Data(),dir.Data()));

  std::cout << "Finished macro" << std::endl;
}

