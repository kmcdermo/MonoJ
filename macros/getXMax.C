#include <vector>

void getXMax(TString sample) {

  // sample name =  cut string 
  TString cutstring;

  if (sample.Contains("doublemu",TString::kExact)) {
    cutstring = "((hltdoublemu > 0) && (mu1pt > 20) && (mu1id == 1) && (zmass < 120.) && (zmass > 60.) && (mu1pid == -mu2pid)) && (cflagcsctight == 1 && cflaghbhenoise == 1)"; 
  }
  else if (sample.Contains("doubleel",TString::kExact)) {
    cutstring = "((hltdoubleel > 0) && (el1pt > 20) && (el1id == 1) && (zeemass < 120.) && (zeemass > 60.) && (el1pid == -el2pid)) && (cflagcsctight == 1 && cflaghbhenoise == 1)";
  }
  else if (sample.Contains("singlephoton",TString::kExact)) {
    cutstring = "((hltphoton165 == 1) || (hltphoton175 == 1)) && (nphotons == 1) && (phpt>200.)";
  }
  else if (sample.Contains("singlemu",TString::kExact)) {
    cutstring = "((hltsinglemu == 1) && (nmuons == 1) && (mu1pt > 30) && (mu1id == 1)) && (cflagcsctight == 1 && cflaghbhenoise == 1)";
  }
  else {
    std::cout << sample.Data() << std::endl;
    exit(1);
  }
 
  // open the file
  TString filestring = Form("root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Data/%s/treewithwgt.root",sample.Data());
  TFile * file = TFile::Open(filestring.Data());
  //  CheckValidFile(file,filestring);

  // open the tree
  TTree * tree = (TTree*)file->Get("tree/tree");
  //  CheckValidTree(tree,"tree/tree",filestring);

  // this is temp th1d that is big for now
  TH1D * h1 = new TH1D("h1","h1",500,0.,5000.);
 
  // variable string 
  std::vector<TString> vars;

  // pt variables
  vars.push_back("mu1pt");
  vars.push_back("mu2pt");
  vars.push_back("el1pt");
  vars.push_back("el2pt");
  vars.push_back("zpt");
  vars.push_back("zeept");
  vars.push_back("emupt");
  vars.push_back("signaljetpt");
  vars.push_back("secondjetpt");
  vars.push_back("ht");
  vars.push_back("phpt");
  //  vars.push_back("phpt_nj_lte2");

  vars.push_back("pfmet");
  vars.push_back("t1pfmet");
  vars.push_back("mumet");
  vars.push_back("t1mumet");
  vars.push_back("mht");

  std::vector<Double_t> xmaxes(vars.size()); // make size as variables
  
  for (UInt_t ivar = 0; ivar < vars.size(); ivar++) {
    
    // Fill h1
    tree->Draw(Form("%s >> h1",vars[ivar].Data()),cutstring.Data(),"goff");

    // Get max x bin per variable
    Double_t xmax = -1;
    for (Int_t ibin = 0; ibin <= h1->GetNbinsX(); ibin++) {
      if ( h1->GetBinContent(ibin) > 0 ){
	xmax = (ibin-1) * 10.;
      }
    }

    // set x maximum per variables
    xmaxes[ivar] = xmax;
  }

  std::cout << "Sample: " << sample.Data() << std::endl << "=========================================" << std::endl;
  for (UInt_t ivar = 0; ivar < vars.size(); ivar++) {
    std::cout << "Variable: " << vars[ivar] << " xmax: " << xmaxes[ivar] << std::endl;
  }

  std::cout << std::endl << "--------------------------------------------------" << std::endl;
  for (UInt_t ivar = 0; ivar < vars.size(); ivar++) {
    std::cout <<  vars[ivar] << std::endl;
  }

  std::cout << std::endl << "--------------------------------------------------" << std::endl;
  for (UInt_t ivar = 0; ivar < vars.size(); ivar++) {
    std::cout <<  xmaxes[ivar] << std::endl;
  }


}
