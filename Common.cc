#include "Common.hh"

void MakeOutDirectory(TString outdir){
  FileStat_t dummyFileStat;
  if (gSystem->GetPathInfo(outdir.Data(), dummyFileStat) == 1){
    TString mkDir = "mkdir -p ";
    mkDir += outdir.Data();
    gSystem->Exec(mkDir.Data());
  }
}

void CheckValidFile(TFile *& file, const TString fname){
  if (file == (TFile*) NULL) { // check if valid file
    std::cout << "Input file is bad pointer: " << fname.Data() << " ...exiting..." << std::endl;
    exit(1);
  }
  else {
    std::cout << "Successfully opened file: " << fname.Data() << std::endl;
  }
}

void CheckValidTree(TTree*& tree, const TString tname, const TString fname){
  if (tree == (TTree*) NULL) { // check if valid plot
    std::cout << "Input TTree is bad pointer: " << tname.Data() << " in input file: " << fname.Data() << " ...exiting..." << std::endl;
    exit(1);
  }
  else {
    //    std::cout << "Successfully opened tree: " << tname.Data() << " in input file: " << fname.Data() << std::endl;
  }
}

void CheckValidTH1D(TH1D*& plot, const TString pname, const TString fname){
  if (plot == (TH1D*) NULL) { // check if valid plot
    std::cout << "Input TH1D is bad pointer: " << pname.Data() << " in input file: " << fname.Data() << " ...exiting..." << std::endl;
    exit(1);
  }
  else {
    //    std::cout << "Successfully initialized plot: " << pname.Data() << " in input file: " << fname.Data() << std::endl;
  }
}

void HaddQCD(TStrVec qcdsamples) {
  TString hadd = "hadd qcd.root";
  TString tohadd = "";
  for (UInt_t qcd = 0; qcd < qcdsamples.size(); qcd++) {
    tohadd.Append(qcdsamples[qcd]);
  }
  hadd.Append(tohadd);
  
  gSystem->Exec(hadd.Data());
}
