#ifndef _pureweight_
#define _pureweight_

#include "Common.hh"

#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TColor.h"
#include "TString.h"

#include <utility>
#include <vector>
#include <map>

class PUReweight
{
public:

  PUReweight(SamplePairVec Samples, const TString selection, const Int_t njetsselection, const Double_t lumi, const Int_t nBins, const TString outdir, const TString outtype, const Bool_t runLocal, const TString run);
  DblVec GetPUWeights();
  ~PUReweight();

private:
  
  TStrVec fDataNames;
  TStrVec fMCNames;
  
  UInt_t fNData;
  UInt_t fNMC;

  TString  fSelection;
  Int_t    fNJetsSeln;
  TString  fNJetsStr;

  TString  fRun;

  Double_t fLumi;
  Int_t    fNBins;

  TString fOutDir;
  TString fOutType;

  TH1D * fOutDataNvtx;
  TH1D * fOutMCNvtx;
  TH1D * fOutDataOverMCNvtx;

  TFile * fOutFile;

  Bool_t fRunLocal;
};

#endif

