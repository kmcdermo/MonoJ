void getMax() {

  TFile * file = TFile::Open("root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Data/doublemu/treewithwgt.root");
  TTree * tree = (TTree*)file->Get("tree/tree");

  // zmumu selection
  
  TH1D * h1 = new TH1D("h1","",1000,0.,1000.);


  tree->Draw("mu1pt >> h1","((hltdoublemu > 0) && (mu1pt > 20) && (mu1id == 1) && (zmass < 120.) && (zmass > 60.) && (mu1pid == -mu2pid)) && (cflagcsctight == 1 && cflaghbhenoise == 1)","goff");

  double mu1pt = h1->GetMaximum();

  TCanvas * c1 = new TCanvas();
  c1->cd();
  h1->Draw();

  std::cout << mu1pt << std::endl;


}
