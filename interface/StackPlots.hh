#ifndef _stackplots_
#define _stackplots_

#include "Common.hh"

#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "THStack.h"
#include "TLegend.h"
#include "TColor.h"
#include "TGaxis.h"
#include "TLine.h"
#include "TLatex.h"
#include "TString.h"

#include <utility>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

typedef std::vector<TFile*>   TFileVec;
typedef std::vector<TH1D*>    TH1DVec;
typedef std::vector<TH1DVec>  TH1DVecVec;
typedef std::vector<THStack*> THStackVec;
typedef std::vector<TLine*>   TLineVec;
typedef std::vector<TLegend*> TLegVec;
typedef std::vector<TCanvas*> TCanvVec;
typedef std::vector<TPad*>    TPadVec;
typedef std::vector<TGaxis*>  TGaxisVec;

class StackPlots
{
public:
  StackPlots(const SamplePairVec Samples, const TString selection, const Int_t njetsselection, const Double_t lumi, const TString outname, const ColorMap & colorMap, const TString outtype);

  void InitTH1DNamesAndSubDNames();
  void OpenInputFiles();
  void InitInputPlots();
  void InitOutputPlots();
  void InitOutputLegends();
  void InitRatioPlots();
  void InitRatioLines();
  void InitOutputCanvPads();
  void InitUpperAxes();

  void DoStacks(std::ofstream & yields);

  void MakeStackPlots(std::ofstream & yields);
  void MakeRatioPlots();
  void MakeOutputCanvas();

  void DrawUpperPad(const UInt_t th1d, const Bool_t isLogY);
  Double_t GetMaximum(const UInt_t th1d);
  Double_t GetMinimum(const UInt_t th1d);
  void SetUpperAxes(const UInt_t th1d);
  void DrawLowerPad(const UInt_t th1d);
  void SetLines(const UInt_t th1d);
  void SaveCanvas(const UInt_t th1d, const Bool_t isLogY);
  void CMSLumi(TCanvas *& pad, const Int_t iPosX);

  ~StackPlots();

private:
  TStrVec fDataNames;
  TStrVec fMCNames;
  TStrVec fTH1DNames;
  TStrMap fTH1DSubDMap;
  TString fSelection;
  Int_t   fNJetsSeln;

  TString fNJetsStr;

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
  TH1DVec    fOutRatioTH1DHists;  
  TH1DVec    fOutRatioMCErrs;  
  TLegVec    fTH1DLegends;
  TLineVec   fOutTH1DRatioLines;

  TGaxisVec  fOutTH1DTGaxes;

  TCanvVec   fOutTH1DCanvases;
  TPadVec    fOutTH1DStackPads;
  TPadVec    fOutTH1DRatioPads;

  TString fOutDir;
  TString fOutName;
  TFile * fOutFile;
  TString fOutType;

  TStrMap  fSampleTitleMap;
  ColorMap fColorMap;
};

#endif
