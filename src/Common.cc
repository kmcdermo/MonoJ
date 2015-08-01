#include "../interface/Common.hh"

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
    std::cout << "Successfully opened tree: " << tname.Data() << " in input file: " << fname.Data() << std::endl;
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

void Hadd(SamplePairVec samples, const TString outdir, const TString selection, const Int_t njetsselection, const TString combinedName) {
  // need to make directory for hadd'ed file
  TString jetstring = "";
  if (njetsselection != -1){
    jetstring = Form("_nj%i",njetsselection);
  }

  TString combinedOutdir = Form("%s/%s%s/%s_MC",outdir.Data(),selection.Data(),jetstring.Data(),combinedName.Data());
  MakeOutDirectory(combinedOutdir);
  
  // Form string for doing hadd
  TString hadd = Form("hadd -f %s/plots.root",combinedOutdir.Data());
  TString tohadd = "";

  for (SamplePairVecIter iter = samples.begin(); iter != samples.end(); ++iter) {
    tohadd.Append(Form(" %s/%s%s/%s_MC/plots.root",outdir.Data(),selection.Data(),jetstring.Data(),(*iter).first.Data()));
  }
  hadd.Append(tohadd);
  
  gSystem->Exec(hadd.Data());
}
