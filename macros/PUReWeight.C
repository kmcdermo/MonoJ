#include <vector>


std::vector<Double_t> pureweight(){

  TFile * datafile = TFile::Open("root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Data/doublemu/treewithwgt.root");  
  TFile * mcfile1  = TFile::Open("root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/zll/treewithwgt.root");  
  TFile * mcfile2  = TFile::Open("root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/ttbar/treewithwgt.root");  

  TTree * datatree = (TTree*)datafile->Get("tree/tree");
  TTree * mctree1  = (TTree*)mcfile1->Get("tree/tree");
  TTree * mctree2  = (TTree*)mcfile2->Get("tree/tree");

  TH1D * datanvtx  = new TH1D("datanvtx","",50,0,50);
  TH1D * mcnvtx1   = new TH1D("mcnvtx1","",50,0,50);
  TH1D * mcnvtx2   = new TH1D("mcnvtx2","",50,0,50);

  datanvtx->Sumw2();
  mcnvtx1->Sumw2();
  mcnvtx2->Sumw2();
  

  // must have tobject name be the one, not variable!

  datatree->Draw("nvtx>>datanvtx","( ((hltdoublemu > 0) && (mu1pt > 20) && (mu1id == 1) && (zmass < 120.) && (zmass > 60.) && (mu1pid == -mu2pid)) && (cflagcsctight == 1 && cflaghbhenoise == 1) )","goff");
  mctree1->Draw("nvtx>>mcnvtx1","( ((hltdoublemu > 0) && (mu1pt > 20) && (mu1id == 1) && (zmass < 120.) && (zmass > 60.) && (mu1pid == -mu2pid)) && (flagcsctight == 1 && flaghbhenoise == 1) ) * (xsec * 0.04024 * wgt / wgtsum)","goff");
  mctree2->Draw("nvtx>>mcnvtx2","( ((hltdoublemu > 0) && (mu1pt > 20) && (mu1id == 1) && (zmass < 120.) && (zmass > 60.) && (mu1pid == -mu2pid)) && (flagcsctight == 1 && flaghbhenoise == 1) ) * (xsec * 0.04024 * wgt / wgtsum)","goff");

  mcnvtx1->Add(mcnvtx2);

  datanvtx->Scale(1.0/datanvtx->Integral());  
  mcnvtx1->Scale(1.0/mcnvtx1->Integral());

  TCanvas * c1 = new TCanvas();
  c1->cd();
  c1->SetLogy(1);
  
  datanvtx->SetLineColor(kRed);
  mcnvtx1->SetLineColor(kBlue);

  datanvtx->Draw("PE");
  mcnvtx1->Draw("HIST SAME");


  c1->SaveAs("before.png");

  TCanvas * c2 = new TCanvas();
  c2->cd();
  c2->SetLogy(1);

  TH1D * datanvtx_copy = datanvtx->Clone();
  
  datanvtx->Divide(mcnvtx1);

  std::vector<Double_t> puweights;
   
  for (Int_t i = 1; i <= 50; i++ ){

    puweights.push_back(datanvtx->GetBinContent(i));
    
    Double_t tmp = mcnvtx1->GetBinContent(i);

    mcnvtx1->SetBinContent(i,puweights[i-1]*tmp);
  }
  
  

  datanvtx_copy->Draw("PE");
  mcnvtx1->Draw("HIST SAME");

  c2->SaveAs("after.png");
 
  for (Int_t i = 0; i < 50; i++ ){

    std::cout << puweights[i] << std::endl;
  }


  /*



  TH1D * redatanvtx  = new TH1D("redatanvtx","",50,0,50);
  TH1D * remcnvtx1   = new TH1D("remcnvtx1","",50,0,50);
  TH1D * remcnvtx2   = new TH1D("remcnvtx2","",50,0,50);
  
  datatree->Draw("nvtx>>redatanvtx","( ((hltdoublemu > 0) && (mu1pt > 20) && (mu1id == 1) && (zmass < 120.) && (zmass > 60.) && (mu1pid == -mu2pid)) && (cflagcsctight == 1 && cflaghbhenoise == 1) )","goff");
  mctree1->Draw("nvtx>>remcnvtx1","( ((hltdoublemu > 0) && (mu1pt > 20) && (mu1id == 1) && (zmass < 120.) && (zmass > 60.) && (mu1pid == -mu2pid)) && (flagcsctight == 1 && flaghbhenoise == 1) ) * (xsec * 0.04024 * wgt / wgtsum)","goff");
  mctree2->Draw("nvtx>>remcnvtx2","( ((hltdoublemu > 0) && (mu1pt > 20) && (mu1id == 1) && (zmass < 120.) && (zmass > 60.) && (mu1pid == -mu2pid)) && (flagcsctight == 1 && flaghbhenoise == 1) ) * (xsec * 0.04024 * wgt / wgtsum)","goff");

  remcnvtx1->Add(remcnvtx2);


  TCanvas * c2 = new TCanvas();
  c2->cd();
  
  redatanvtx->SetLineColor(kRed);
  remcnvtx1->SetLineColor(kBlue);

  redatanvtx->Draw("PE");
  remcnvtx1->Draw("HIST SAME");

  */


  
  return puweights;
}

