#ifndef _common_
#define _common_

#include "TString.h"
#include "TColor.h"
#include "TSystem.h"
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"
#include "TStyle.h"

#include <iostream>
#include <vector>
#include <utility>
#include <map>

typedef std::vector<Double_t> DblVec;

typedef std::map<TString,Color_t> ColorMap;

typedef std::vector<TString>      TStrVec;

typedef std::pair<TString,Bool_t> SamplePair;
typedef std::vector<SamplePair>   SamplePairVec;
typedef SamplePairVec::iterator   SamplePairVecIter;

typedef std::map<TString,TString> TStrMap;

// global functions

void MakeOutDirectory(TString outdir);
void CheckValidFile(TFile *& file, const TString fname);
void CheckValidTree(TTree*& tree, const TString tname, const TString fname);
void CheckValidTH1D(TH1D*& plot, const TString pname, const TString fname);
void Hadd(SamplePairVec samples, const TString outdir, const TString selection, const Int_t njetsselection, const TString combinedName);
void SetTDRStyle(TStyle *& tdrStyle);

#endif
