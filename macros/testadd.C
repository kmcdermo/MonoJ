void testadd(){
  
  TFile * f1 = TFile::Open("../fullbatch_singlemu/singlemu/singlemu_data/plots.root");
  TFile * f2 = TFile::Open("../fullbatch_singlemu/singlemu/zll_MC/plots.root");
  TFile * f3 = TFile::Open("../fullbatch_singlemu/singlemu/top_MC/plots.root");
  TFile * f4 = TFile::Open("../fullbatch_singlemu/singlemu/wln_MC/plots.root");
  TFile * f5 = TFile::Open("../fullbatch_singlemu/singlemu/diboson_MC/plots.root");



  TH1D * h1 = (TH1D*)f1->Get("njets");
  TH1D * h2 = (TH1D*)f2->Get("njets");
  TH1D * h3 = (TH1D*)f3->Get("njets");
  TH1D * h4 = (TH1D*)f4->Get("njets");
  TH1D * h5 = (TH1D*)f5->Get("njets");

  TH1D * addData1 = new TH1D();
  addData1->Sumw2();
  addData1 = (TH1D*)h1->Clone();


  TH1D * addMC1 = new TH1D();
  addMC1->Sumw2();
  /*h2->Add(h3);
  h2->Add(h4);
  h2->Add(h5);*/




  addMC1 = (TH1D*)h2->Clone();
  //  addMC1->Add(h3);
  //  addMC1->Add(h4);
  //  addMC1->Add(h5);
  /*
  for (int i = 1; i <= h2->GetNbinsX(); i++){
    std::cout <<h2->GetBinContent(i) << " " <<  h2->GetBinError(i) <<std::endl;

  }
  */

  THStack * stackMC1;

  TH1D * ratio1 = new TH1D();
  ratio1->Sumw2();
  ratio1 = (TH1D*)addData1->Clone();
  ratio1->Divide(addMC1);  
  ratio1->SetLineColor(kBlack);
  ratio1->SetMinimum(-0.1);  // Define Y ..
  ratio1->SetMaximum(2.1); // .. range
  ratio1->SetStats(0);      // No statistics on lower plot
  ratio1->GetYaxis()->SetTitle("Data/MC");
    
  
  std::cout << "here" << std::endl;

  TLegend * leg1 =  new TLegend(0.682,0.7,0.825,0.92);
  leg1->SetBorderSize(4);
  leg1->SetLineColor(kBlack);

  


  std::cout << "here" << std::endl;

  
  TCanvas * c1 = new TCanvas();
  c1->cd();
  c1->SetLogy();
  /*
  TPad * pup1 = new TPad("", "", 0, 0.3, 1.0, 0.99);
  pup1->SetBottomMargin(0); // Upper and lower plot are joined
  pup1->cd();
  pup1->Draw();
  */
  addMC1->SetMarkerSize(0);
  addMC1->SetLineColor(kRed);
  addMC1->SetFillStyle(3004);
  addMC1->SetFillColor(kRed);

  //  addMC1->SetErrorX(0);
  //  addData1->Draw();
  addMC1->Draw("E2");

  /*
  TPad * pad2 = new TPad("", "", 0, 0.05, 1.0, 0.3);
  pad2->SetTopMargin(0);
  pad2->SetBottomMargin(0.2);
  */
  
    std::cout << "here" << std::endl;













  //  TLine * tl1 = new TLine();
  





}
