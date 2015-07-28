#ifndef _typedef_
#define _typedef_

#include "TString.h"
#include "TColor.h"
#include "TSystem.h"
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"

#include <iostream>
#include <vector>
#include <utility>
#include <map>

typedef std::map<TString,Color_t> ColorMap;
typedef std::vector<TString>      TStrVec;

typedef std::pair<TString,Bool_t> SamplePair;
typedef std::vector<SamplePair>   SamplePairVec;
typedef SamplePairVec::iterator   SamplePairVecIter;

// global functions

void MakeOutDirectory(TString outdir);
void CheckValidFile(TFile *& file, const TString fname);
void CheckValidTree(TTree*& tree, const TString tname, const TString fname);
void CheckValidTH1D(TH1D*& plot, const TString pname, const TString fname);

#endif
