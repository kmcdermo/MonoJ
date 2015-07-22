#include "StackPlots.hh"

StackPlots::StackPlots(const SampleVec samples, const TString outdir, const TString outtype){
  fSamples = samples; // set samples to stack
  fOutDir = outdir; // where to put stacked histos

  // make output directory --> same name as outname
  FileStat_t dummyFileStat;
  if (gSystem->GetPathInfo(fOutDir.Data(), dummyFileStat) == 1){
    TString mkDir = "mkdir -p ";
    mkDir += fOutDir.Data();
    gSystem->Exec(mkDir.Data());
  }

  // allow user to pick png, pdf, gif, etc for stacked plots
  fOutType = outType;

  // define color map here --> will move this plus type defs to a dedicated file
  fColorMap["zmumu"] = kCyan;
  
  // define title map here --> will move
  fSampleTitleMap["zmumu"] = "Z #rightarrow #mu^{+} #mu^{-}";
}

StackPlots::~StackPlots(){
  for (UInt_t h = 0; h < fStacks; h++){
    delete fStacks[h];
  }
}

void StackPlots::DoStacking(){
  StackPlots::InitStackPlots();

  //outer loop over variable we want stacked
  //inner loop over samples
  for (UInt_t p = 0; p < fStacks.size(); p++){
    TString histName = fStacks[p]->GetName();
    
    TLegend * stackleg = new TLegend(0.8,0.8,0.97,0.97);

    for (UInt_t s = 0; s < fSamples.size(); s++){
      // get the input sample
      TString sampleName = fSamples[s].first;
      Bool_t  isMC = fSamples[s].second;
      TString sampleInput = sampleName+(isMC?"_MC":"_data");
      
      // open the root file. string is created by analysis.cpp
      TFile * sample = TFile::Open("%s/%s_plots.root",sampleInput.Data()+" MC",sampleInput.Data());
      TH1* hist = (TH1*)sample->Get(histName.Data());
      
      // add hist to stack
      fStacks[p]->Add(hist);
      // legend for stack
      stackleg->AddEntry(hist,(isMC?fSampleTitleMap[sampleName].Data():"Data"),(isMC?"lf":"p"));
      
      delete sample;
      delete hist;
    }
    
    // now draw the stack
    TCanvas * stackcanv = new TCanvas();    
    stackcanv->cd();
    fStack[p]->Draw();
    stackleg->Draw("SAME");

    // save the stack
    stackcanv->SaveAs(Form("%s/%s.%s",fOutDir.Name(),histname.Data(),fOutType.Data()));

    // how to do the ratio??? may need to place stack somehere on canvas

    delete stackleg;
    delete stackcanv;
  }
}

void StackPlots::InitStackPlots(){
  // just make list of stacks we want to make, then new, then push back.
  TString plotNames = {"zmass",
		       "zpt",
		       "mht",
		       "signaljetpt",
		       "signaljeteta",
		       "signaljetCHfrac",
		       "signaljetNHfrac",
		       "signaljetEMfrac",
		       "signaljetCEMfrac",
		       "njets",
		       "ntvx"};
  
  fStacks.reserve[plotNames.size()]; // make enough space, then add each plot to the stack vec
  for (UInt_t p = 0; p < plotNames.size(); p++){
    fStacks[p] = new THStack(plotNames[i].Data(),"");
  }
}
