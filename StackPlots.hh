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

typedef std::map<TString,TString> TStrMap;

typedef std::map<TString,Color_t> ColorMap;

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
  TStrVec fDHistNames;
  TStrVec fIHistNames;

  Double_t fLumi;
  
  UInt_t fNData;
  UInt_t fNMC;
  UInt_t fNDHists;
  UInt_t fNIHists;

  TFileVec fDataFiles;
  TFileVec fMCFiles;

  ColorMap fColorMap;

  TH1DVecVec fInDataDHists;
  TH1DVecVec fInMCDHists;
  TH1IVecVec fInDataIHists;
  TH1IVecVec fInMCIHists;

  TH1DVec fOutDataDHists;
  TH1DVec fOutMCDHists;
  THStackVec fOutMCDStacks;
  TH1IVec fOutDataIHists;
  TH1IVec fOutMCIHists;
  THStackVec fOutMCIStacks;

  TLegVec fDLegend;
  TLegVec fILegend;

  TH1DVec fOutRatioDHists;  
  TH1IVec fOutRatioIHists;

  TCanvVec fOutDCanvas;
  TPadVec  fOutDStackPad;
  TPadVec  fOutDRatioPad;
  TCanvVec fOutICanvas;
  TPadVec  fOutIStackPad;
  TPadVec  fOutIRatioPad;

  TString fOutName;
  TFile * fOutFile;
  TString fOutType;

  TStrMap  fSampleTitleMap;
};
