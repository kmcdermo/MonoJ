#include "TFile.h"
#include "TH1D.h"
#include "TH1I.h"
#include "TString.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TColor.h"
#include "THStack.h"

#include <vector>
#include <utility>
#include <iostream>

typedef std::pair<TString,Bool_t> SamplePair;
typedef std::vector<SamplePair> SampleVec;
typedef std::vector<THStack*> TStackVec;

class StackPlots
{
public:
  StackPlots(SampleVec,TString,TString);
  ~StackPlots();
  
  void DoStacking();

private:
  SampleVec fSamples;
  TString fOutDir;
  TString fOutType;

  THStackVec fStacks;
};


