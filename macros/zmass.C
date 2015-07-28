void zmass() {
    TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 600);

    Int_t   nbins = 60;
    Float_t xmin  = 60.;
    Float_t xmax  = 120.;

    TFile* mcfile = TFile::Open("root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/zll/treewithwgt.root");
    TFile* dtfile = TFile::Open("root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Data/doublemu/treewithwgt.root");

    TTree* mctree = (TTree*)mcfile->Get("tree/tree");
    TTree* dttree = (TTree*)dtfile->Get("tree/tree");

    TH1F* mchist  = new TH1F("mchist", "", nbins, xmin, xmax);
    TH1F* dthist  = new TH1F("dthist", "", nbins, xmin, xmax);

    mctree->Draw("zmass>>mchist", "((hltdoublemu > 0 || hltsinglemu > 0) && mu1pt > 20 && mu1id == 1) * xsec * 0.035 * wgt / wgtsum");
    dttree->Draw("zmass>>dthist", " (hltdoublemu > 0 || hltsinglemu > 0) && mu1pt > 20 && mu1id == 1");

    mchist->SetFillColor(kCyan);

    TH1* frame = canvas->DrawFrame(xmin, 0.01, xmax, 800.0, "");
    canvas->SetRightMargin(0.075);
    canvas->SetTopMargin(0.075);
    frame->GetXaxis()->SetTitle("Dimuon Mass [GeV]");
    frame->GetYaxis()->SetTitle("Events / GeV");
    frame->GetXaxis()->SetLabelSize(0.9*frame->GetXaxis()->GetLabelSize());
    frame->GetYaxis()->SetLabelSize(0.9*frame->GetYaxis()->GetLabelSize());

    frame ->Draw();
    mchist->Draw("HIST SAME");
    dthist->Draw("PE SAME");

    TLegend* leg = new TLegend(0.6, 0.7, 0.9, 0.9);
    leg->SetFillColor(0);
    leg->AddEntry(dthist, "Data");
    leg->AddEntry(mchist, "Z#rightarrow #mu^{+} #mu^{-} MC", "F");        
    leg->Draw("SAME");

    canvas->RedrawAxis();
}
