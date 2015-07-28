#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH1I.h"

#include "TString.h"
#include "TStyle.h"
#include "TColor.h"
#include "TLatex.h"

#include "TPad.h"
#include "THStack.h"
#include "TLegend.h"
#include "TLine.h"

#include <utility>
#include <map>
#include <vector>
#include <iostream>

typedef std::vector<TString>  TStrVec;

typedef std::pair<TString,Bool_t> SamplePair;
typedef std::vector<SamplePair> SamplePairVec;
typedef SamplePairVec::iterator SamplePairVecIter;

typedef std::vector<TFile*> TFileVec;
typedef std::vector<TH1D*> TH1DVec;
typedef std::vector<TH1DVec> TH1DVecVec;
typedef std::vector<TH1I*> TH1IVec;
typedef std::vector<TH1IVec> TH1IVecVec;
typedef std::vector<THStack*> THStackVec;
typedef std::vector<TLegend*> TLegVec;
typedef std::vector<TCanvas*> TCanvVec;
typedef std::vector<TPad*> TPadVec;

typedef std::map<TString,Color_t> ColorMap;

typedef std::map<TString,TString> TStrMap;

class StackPlots
{
public:
  StackPlots(const SamplePairVec Samples, const Double_t lumi, const TString outname, const TString outtype);
  ~StackPlots();
  
  void OpenInputFiles();
  void InitInputPlots();
  void InitOutputPlots();
  void InitOutputLegends();
  void InitRatioPlots();
  void InitOutputCanvPads();

  void DoStacks();

  void MakeStackPlots();
  void MakeRatioPlots();
  void MakeOutputCanvas();
  void CMS_Lumi(TCanvas *& pad, const Int_t iPosX);

private:
  TStrVec fDataNames;
  TStrVec fMCNames;
  TStrVec fTH1DNames;

  Double_t fLumi;
  
  UInt_t fNData;
  UInt_t fNMC;
  UInt_t fNTH1D;

  TFileVec fDataFiles;
  TFileVec fMCFiles;

  TH1DVecVec fInDataTH1DHists;
  TH1DVecVec fInMCTH1DHists;

  TH1DVec    fOutDataTH1DHists;
  TH1DVec    fOutMCTH1DHists;
  THStackVec fOutMCTH1DStacks;

  TLegVec fTH1DLegends;

  TH1DVec fOutRatioTH1DHists;  

  TCanvVec fOutTH1DCanvases;
  TPadVec  fOutTH1DStackPads;
  TPadVec  fOutTH1DRatioPads;

  TString fOutName;
  TFile * fOutFile;
  TString fOutType;

  TStrMap  fSampleTitleMap;

  ColorMap fColorMap;
};
