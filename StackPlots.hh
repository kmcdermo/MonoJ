#include "TFile.h"
#include "TH1D.h"
#include "TH1I.h"
#include "TString.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TColor.h"
#include "THStack.h"
#include "TList.h"
#include "TLegend.h"

#include <vector>
#include <utility>
#include <iostream>
#include <map>

typedef std::vector<TString>  TStrVec;

typedef std::map<TString,Color_t> ColorMap;
typedef std::map<TString,TString> TStrMap;

typedef std::vector<TFile*> TFileVec;

typedef std::vector<TH1D*> TH1DVec;
typedef std::vector<TH1DVec> TH1DVecVec;

typedef std::vector<TH1I*> TH1IVec;
typedef std::vector<TH1IVec> TH1IVecVec;

typedef std::vector<THStack*> THStackVec;

typedef std::vector<TLegend*> TLegVec;

typedef std::vector<TCanvas*> TCanvVec;
typedef std::vector<TPad*> TPadVec;

class StackPlots
{
public:
  StackPlots(const TStrVec data, const TStrVec mc, const TStrVec dhistNames, const TStrVec ihistNames, const TString outname, const TString outtype);
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

private:
  TStrVec fDataNames;
  TStrVec fMCNames;
  TStrVec fDHistNames;
  TStrVec fIHistNames;
  
  UInt_t fNData;
  UInt_t fNMC;
  UInt_t fNDHists;
  UInt_t fNIHists;

  ColorMap fColorMap;
  TStrMap fSampleTitleMap;

  TFileVec fDataFiles;
  TFileVec fMCFiles;

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
};