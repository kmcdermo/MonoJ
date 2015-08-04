TFile * file;
TTree * tree;
TString cut;
TCanvas * c1;

void load(TString sample) {
 
  file = TFile::Open(Form("root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Data/%s/treewithwgt.root",sample.Data()));
  tree = (TTree*)file->Get("tree/tree");
 
  if (sample.Contains("doublemu",TString::kExact)) {
    cut = "((hltdoublemu > 0) && (mu1pt > 20) && (mu1id == 1) && (zmass < 120.) && (zmass > 60.) && (mu1pid == -mu2pid)) && (cflagcsctight == 1 && cflaghbhenoise == 1)"; 
  }
  else if (sample.Contains("doubleel",TString::kExact)) {
    cut = "((hltdoubleel > 0) && (el1pt > 20) && (el1id == 1) && (zeemass < 120.) && (zeemass > 60.) && (el1pid == -el2pid)) && (cflagcsctight == 1 && cflaghbhenoise == 1)";
  }
  else if (sample.Contains("singlephoton",TString::kExact)) {
    cut = "((hltphoton165 == 1) || (hltphoton175 == 1)) && (nphotons == 1) && (phpt>200.)";
  }
  else if (sample.Contains("singlemu",TString::kExact)) {
    cut = "((hltsinglemu == 1) && (nmuons == 1) && (mu1pt > 30) && (mu1id == 1)) && (cflagcsctight == 1 && cflaghbhenoise == 1)";
  }
  else {
    std::cout << sample.Data() << std::endl;
    exit(1);
  }

  c1  = new TCanvas();
  c1->SetLogy();
}
