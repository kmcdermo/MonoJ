#ifndef _analysis_
#define _analysis_

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

#include <iostream>
#include <fstream>

// use this for setting up plots
struct PlotOptStruct 
{
public:
  Int_t    nbins;
  Double_t xmin;
  Double_t xmax;
  Double_t binw;
};
  
typedef std::map<TString,PlotOptStruct> TStrPlotOptStructMap;
  
typedef std::vector<UInt_t> UIntVec;
typedef std::vector<Int_t>  IntVec;

typedef std::map<TString,TH1D*> TH1DMap;
typedef TH1DMap::iterator       TH1DMapIter;

class Analysis
{
public:

  Analysis(const SamplePair SamplePair, const TString selection, const Int_t njetsselection, const DblVec puweights, const Double_t lumi, const Int_t nBins_vtx, const TString outdir, const ColorMap & colorMap, const TString outType, const Bool_t runLocal, const TString run);
  
  void SetBranchAddresses();
  void DoAnalysis(std::ofstream & yields);
  void SetUpPlotOptions();
  void SetUpPlots();
  TH1D * MakeTH1DPlot(const TString hname, const TString htitle, const Int_t nbins, const Double_t xlow, const Double_t xhigh, const TString xtitle, const TString ytitle);
  void SaveHists();
  void DeleteHists();
  
  void DeleteBranches();
  
  ~Analysis();

private:
  // initialized in constructor
  TString fSample;
  TString fRun;
  Bool_t fIsMC;
  TString fSelection;
  Int_t   fNJetsSeln;
  TString fNJetsStr;
  DblVec   fPUWeights;
  Double_t fLumi;
  Int_t    fNBins_vtx;

  TFile * fInFile;
  TTree * fInTree;

  TString fOutDir;
  TString fOutName;
  TFile * fOutFile;
  TString fOutType;

  Double_t fNSelected;

  TStrPlotOptStructMap fPlotOptMap;
  TH1DMap fTH1DMap;

  ColorMap fColorMap;
  TStrMap  fTH1DSubDMap;

  Bool_t fRunLocal;

  // branch stuff
  UInt_t          event;
  UInt_t          run;
  UInt_t          lumi;
  Double_t        xsec;
  Double_t        wgt;
  Double_t        kfact;
  Double_t        puwgt;
  Int_t           puobs;
  Int_t           putrue;
  UInt_t          nvtx;
  UChar_t         hltmet90;
  UChar_t         hltmet120;
  UChar_t         hltmetwithmu90;
  UChar_t         hltmetwithmu120;
  UChar_t         hltmetwithmu170;
  UChar_t         hltmetwithmu300;
  UChar_t         hltjetmet90;
  UChar_t         hltjetmet120;
  UChar_t         hltphoton165;
  UChar_t         hltphoton175;
  UChar_t         hltdoublemu;
  UChar_t         hltsinglemu;
  UChar_t         hltdoubleel;
  UChar_t         hltsingleel;
  UChar_t         flagcsctight;
  UChar_t         flaghbhenoise;
  UChar_t         flaghcallaser;
  UChar_t         flagecaltrig;
  UChar_t         flageebadsc;
  UChar_t         flagecallaser;
  UChar_t         flagtrkfail;
  UChar_t         flagtrkpog;
  UChar_t         flaghnoiseloose;
  UChar_t         flaghnoisetight;
  UChar_t         flaghnoisehilvl;
  UInt_t          nmuons;
  UInt_t          nelectrons;
  UInt_t          ntightmuons;
  UInt_t          ntightelectrons;
  UInt_t          ntaus;
  UInt_t          njets;
  UInt_t          nbjets;
  UInt_t          nfatjets;
  UInt_t          nphotons;
  Double_t        pfmet;
  Double_t        pfmetphi;
  Double_t        t1pfmet;
  Double_t        t1pfmetphi;
  Double_t        pfmupt;
  Double_t        pfmuphi;
  Double_t        mumet;
  Double_t        mumetphi;
  Double_t        phmet;
  Double_t        phmetphi;
  Double_t        t1mumet;
  Double_t        t1mumetphi;
  Double_t        t1phmet;
  Double_t        t1phmetphi;
  Double_t        fatjetpt;
  Double_t        fatjeteta;
  Double_t        fatjetphi;
  Double_t        fatjetprmass;
  Double_t        fatjetsdmass;
  Double_t        fatjettrmass;
  Double_t        fatjetftmass;
  Double_t        fatjettau2;
  Double_t        fatjettau1;
  Double_t        fatjetCHfrac;
  Double_t        fatjetNHfrac;
  Double_t        fatjetEMfrac;
  Double_t        fatjetCEMfrac;
  Double_t        fatjetmetdphi;
  Double_t        signaljetpt;
  Double_t        signaljeteta;
  Double_t        signaljetphi;
  Double_t        signaljetbtag;
  Double_t        signaljetCHfrac;
  Double_t        signaljetNHfrac;
  Double_t        signaljetEMfrac;
  Double_t        signaljetCEMfrac;
  Double_t        signaljetmetdphi;
  Double_t        secondjetpt;
  Double_t        secondjeteta;
  Double_t        secondjetphi;
  Double_t        secondjetbtag;
  Double_t        secondjetCHfrac;
  Double_t        secondjetNHfrac;
  Double_t        secondjetEMfrac;
  Double_t        secondjetCEMfrac;
  Double_t        secondjetmetdphi;
  Double_t        thirdjetpt;
  Double_t        thirdjeteta;
  Double_t        thirdjetphi;
  Double_t        thirdjetbtag;
  Double_t        thirdjetCHfrac;
  Double_t        thirdjetNHfrac;
  Double_t        thirdjetEMfrac;
  Double_t        thirdjetCEMfrac;
  Double_t        thirdjetmetdphi;
  Double_t        jetjetdphi;
  Double_t        jetmetdphimin;
  Double_t        incjetmetdphimin;
  Double_t        ht;
  Double_t        dht;
  Double_t        mht;
  Double_t        alphat;
  Double_t        apcjetmetmax;
  Double_t        apcjetmetmin;
  Int_t           mu1pid;
  Double_t        mu1pt;
  Double_t        mu1eta;
  Double_t        mu1phi;
  Int_t           mu1id;
  Int_t           mu2pid;
  Double_t        mu2pt;
  Double_t        mu2eta;
  Double_t        mu2phi;
  Int_t           mu2id;
  Int_t           el1pid;
  Double_t        el1pt;
  Double_t        el1eta;
  Double_t        el1phi;
  Int_t           el1id;
  Int_t           el2pid;
  Double_t        el2pt;
  Double_t        el2eta;
  Double_t        el2phi;
  Int_t           el2id;
  Double_t        zmass;
  Double_t        zpt;
  Double_t        zeta;
  Double_t        zphi;
  Double_t        wmt;
  Double_t        emumass;
  Double_t        emupt;
  Double_t        emueta;
  Double_t        emuphi;
  Double_t        zeemass;
  Double_t        zeept;
  Double_t        zeeeta;
  Double_t        zeephi;
  Double_t        wemt;
  Double_t        phpt;
  Double_t        pheta;
  Double_t        phphi;
  Double_t        loosephpt;
  Double_t        loosepheta;
  Double_t        loosephphi;
  Double_t        loosephsieie;
  Double_t        loosephrndiso;
  Int_t           wzid;
  Double_t        wzmass;
  Double_t        wzmt;
  Double_t        wzpt;
  Double_t        wzeta;
  Double_t        wzphi;
  Int_t           l1id;
  Double_t        l1pt;
  Double_t        l1eta;
  Double_t        l1phi;
  Int_t           l2id;
  Double_t        l2pt;
  Double_t        l2eta;
  Double_t        l2phi;
  Int_t           i1id;
  Double_t        i1pt;
  Double_t        i1eta;
  Double_t        i1phi;
  Int_t           i2id;
  Double_t        i2pt;
  Double_t        i2eta;
  Double_t        i2phi;
  Int_t           i3id;
  Double_t        i3pt;
  Double_t        i3eta;
  Double_t        i3phi;
  UChar_t         cflagcsctight;
  UChar_t         cflaghbhenoise;
  UChar_t         cflaghcallaser;
  UChar_t         cflagecaltrig;
  UChar_t         cflageebadsc;
  UChar_t         cflagecallaser;
  UChar_t         cflagtrkfail;
  UChar_t         cflagtrkpog;
  Double_t        wgtsum;

  // List of branches
  TBranch        *b_event;   //!
  TBranch        *b_run;   //!
  TBranch        *b_lumi;   //!
  TBranch        *b_xsec;   //!
  TBranch        *b_wgt;   //!
  TBranch        *b_kfact;   //!
  TBranch        *b_puwgt;   //!
  TBranch        *b_puobs;   //!
  TBranch        *b_putrue;   //!
  TBranch        *b_nvtx;   //!
  TBranch        *b_hltmet90;   //!
  TBranch        *b_hltmet120;   //!
  TBranch        *b_hltmetwithmu90;   //!
  TBranch        *b_hltmetwithmu120;   //!
  TBranch        *b_hltmetwithmu170;   //!
  TBranch        *b_hltmetwithmu300;   //!
  TBranch        *b_hltjetmet90;   //!
  TBranch        *b_hltjetmet120;   //!
  TBranch        *b_hltphoton165;   //!
  TBranch        *b_hltphoton175;   //!
  TBranch        *b_hltdoublemu;   //!
  TBranch        *b_hltsinglemu;   //!
  TBranch        *b_hltdoubleel;   //!
  TBranch        *b_hltsingleel;   //!
  TBranch        *b_flagcsctight;   //!
  TBranch        *b_flaghbhenoise;   //!
  TBranch        *b_flaghcallaser;   //!
  TBranch        *b_flagecaltrig;   //!
  TBranch        *b_flageebadsc;   //!
  TBranch        *b_flagecallaser;   //!
  TBranch        *b_flagtrkfail;   //!
  TBranch        *b_flagtrkpog;   //!
  TBranch        *b_flaghnoiseloose;   //!
  TBranch        *b_flaghnoisetight;   //!
  TBranch        *b_flaghnoisehilvl;   //!
  TBranch        *b_nmuons;   //!
  TBranch        *b_nelectrons;   //!
  TBranch        *b_ntightmuons;   //!
  TBranch        *b_ntightelectrons;   //!
  TBranch        *b_ntaus;   //!
  TBranch        *b_njets;   //!
  TBranch        *b_nbjets;   //!
  TBranch        *b_nfatjets;   //!
  TBranch        *b_nphotons;   //!
  TBranch        *b_pfmet;   //!
  TBranch        *b_pfmetphi;   //!
  TBranch        *b_t1pfmet;   //!
  TBranch        *b_t1pfmetphi;   //!
  TBranch        *b_pfmupt;   //!
  TBranch        *b_pfmuphi;   //!
  TBranch        *b_mumet;   //!
  TBranch        *b_mumetphi;   //!
  TBranch        *b_phmet;   //!
  TBranch        *b_phmetphi;   //!
  TBranch        *b_t1mumet;   //!
  TBranch        *b_t1mumetphi;   //!
  TBranch        *b_t1phmet;   //!
  TBranch        *b_t1phmetphi;   //!
  TBranch        *b_fatjetpt;   //!
  TBranch        *b_fatjeteta;   //!
  TBranch        *b_fatjetphi;   //!
  TBranch        *b_fatjetprmass;   //!
  TBranch        *b_fatjetsdmass;   //!
  TBranch        *b_fatjettrmass;   //!
  TBranch        *b_fatjetftmass;   //!
  TBranch        *b_fatjettau2;   //!
  TBranch        *b_fatjettau1;   //!
  TBranch        *b_fatjetCHfrac;   //!
  TBranch        *b_fatjetNHfrac;   //!
  TBranch        *b_fatjetEMfrac;   //!
  TBranch        *b_fatjetCEMfrac;   //!
  TBranch        *b_fatjetmetdphi;   //!
  TBranch        *b_signaljetpt;   //!
  TBranch        *b_signaljeteta;   //!
  TBranch        *b_signaljetphi;   //!
  TBranch        *b_signaljetbtag;   //!
  TBranch        *b_signaljetCHfrac;   //!
  TBranch        *b_signaljetNHfrac;   //!
  TBranch        *b_signaljetEMfrac;   //!
  TBranch        *b_signaljetCEMfrac;   //!
  TBranch        *b_signaljetmetdphi;   //!
  TBranch        *b_secondjetpt;   //!
  TBranch        *b_secondjeteta;   //!
  TBranch        *b_secondjetphi;   //!
  TBranch        *b_secondjetbtag;   //!
  TBranch        *b_secondjetCHfrac;   //!
  TBranch        *b_secondjetNHfrac;   //!
  TBranch        *b_secondjetEMfrac;   //!
  TBranch        *b_secondjetCEMfrac;   //!
  TBranch        *b_secondjetmetdphi;   //!
  TBranch        *b_thirdjetpt;   //!
  TBranch        *b_thirdjeteta;   //!
  TBranch        *b_thirdjetphi;   //!
  TBranch        *b_thirdjetbtag;   //!
  TBranch        *b_thirdjetCHfrac;   //!
  TBranch        *b_thirdjetNHfrac;   //!
  TBranch        *b_thirdjetEMfrac;   //!
  TBranch        *b_thirdjetCEMfrac;   //!
  TBranch        *b_thirdjetmetdphi;   //!
  TBranch        *b_jetjetdphi;   //!
  TBranch        *b_jetmetdphimin;   //!
  TBranch        *b_incjetmetdphimin;   //!
  TBranch        *b_ht;   //!
  TBranch        *b_dht;   //!
  TBranch        *b_mht;   //!
  TBranch        *b_alphat;   //!
  TBranch        *b_apcjetmetmax;   //!
  TBranch        *b_apcjetmetmin;   //!
  TBranch        *b_mu1pid;   //!
  TBranch        *b_mu1pt;   //!
  TBranch        *b_mu1eta;   //!
  TBranch        *b_mu1phi;   //!
  TBranch        *b_mu1id;   //!
  TBranch        *b_mu2pid;   //!
  TBranch        *b_mu2pt;   //!
  TBranch        *b_mu2eta;   //!
  TBranch        *b_mu2phi;   //!
  TBranch        *b_mu2id;   //!
  TBranch        *b_el1pid;   //!
  TBranch        *b_el1pt;   //!
  TBranch        *b_el1eta;   //!
  TBranch        *b_el1phi;   //!
  TBranch        *b_el1id;   //!
  TBranch        *b_el2pid;   //!
  TBranch        *b_el2pt;   //!
  TBranch        *b_el2eta;   //!
  TBranch        *b_el2phi;   //!
  TBranch        *b_el2id;   //!
  TBranch        *b_zmass;   //!
  TBranch        *b_zpt;   //!
  TBranch        *b_zeta;   //!
  TBranch        *b_zphi;   //!
  TBranch        *b_wmt;   //!
  TBranch        *b_emumass;   //!
  TBranch        *b_emupt;   //!
  TBranch        *b_emueta;   //!
  TBranch        *b_emuphi;   //!
  TBranch        *b_zeemass;   //!
  TBranch        *b_zeeept;   //!
  TBranch        *b_zeeeta;   //!
  TBranch        *b_zeephi;   //!
  TBranch        *b_wemt;   //!
  TBranch        *b_phpt;   //!
  TBranch        *b_pheta;   //!
  TBranch        *b_phphi;   //!
  TBranch        *b_loosephpt;   //!
  TBranch        *b_loosepheta;   //!
  TBranch        *b_loosephphi;   //!
  TBranch        *b_loosephsieie;   //!
  TBranch        *b_loosephrndiso;   //!
  TBranch        *b_wzid;   //!
  TBranch        *b_wzmass;   //!
  TBranch        *b_wzmt;   //!
  TBranch        *b_wzpt;   //!
  TBranch        *b_wzeta;   //!
  TBranch        *b_wzphi;   //!
  TBranch        *b_l1id;   //!
  TBranch        *b_l1pt;   //!
  TBranch        *b_l1eta;   //!
  TBranch        *b_l1phi;   //!
  TBranch        *b_l2id;   //!
  TBranch        *b_l2pt;   //!
  TBranch        *b_l2eta;   //!
  TBranch        *b_l2phi;   //!
  TBranch        *b_i1id;   //!
  TBranch        *b_i1pt;   //!
  TBranch        *b_i1eta;   //!
  TBranch        *b_i1phi;   //!
  TBranch        *b_i2id;   //!
  TBranch        *b_i2pt;   //!
  TBranch        *b_i2eta;   //!
  TBranch        *b_i2phi;   //!
  TBranch        *b_i3id;   //!
  TBranch        *b_i3pt;   //!
  TBranch        *b_i3eta;   //!
  TBranch        *b_i3phi;   //!
  TBranch        *b_cflagcsctight;   //!
  TBranch        *b_cflaghbhenoise;   //!
  TBranch        *b_cflaghcallaser;   //!
  TBranch        *b_cflagecaltrig;   //!
  TBranch        *b_cflageebadsc;   //!
  TBranch        *b_cflagecallaser;   //!
  TBranch        *b_cflagtrkfail;   //!
  TBranch        *b_cflagtrkpog;   //!
  TBranch        *b_wgtsum;   //!
};

#endif
