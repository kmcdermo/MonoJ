#include "TFile.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TString.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TColor.h"

#include <vector>
#include <utility>
#include <map>
#include <iostream>

typedef std::vector<TString>  TStrVec;
typedef std::vector<UInt_t>   UIntVec;
typedef std::vector<Int_t>    IntVec;
typedef std::vector<Double_t> DblVec;

typedef std::pair<TH1D*,Bool_t> TH1DBoolPair;
typedef std::map<TString,TH1DBoolPair> TH1DMap;
typedef TH1DMap::iterator TH1DMapIter;

typedef std::pair<TH1I*,Bool_t> TH1IBoolPair;
typedef std::map<TString,TH1IBoolPair> TH1IMap;
typedef TH1IMap::iterator TH1IMapIter;

typedef std::map<TString,Color_t> ColorMap;

class Analysis
{
public:

  Analysis(const TString sample, const Bool_t isMC, const TString outType);
  
  void SetBranchAddresses();
  void DoAnalysis();
  void SetUpPlots();
  TH1DBoolPair MakeTH1DPlot(const TString hname, const TString htitle, const Int_t nbins, const Double_t xlow, const Double_t xhigh, const TString xtitle, const TString ytitle, const Bool_t logy);
  TH1IBoolPair MakeTH1IPlot(const TString hname, const TString htitle, const Int_t nbins, const Int_t xlow, const Int_t xhigh, const TString xtitle, const TString ytitle, const Bool_t logy);
  void SaveHists();
    
  ~Analysis();
  void DeleteHists();

private:
  // initialized in constructor
  TString fSample;
  Bool_t fIsMC;
  
  TFile * fInFile;
  TTree * fInTree;

  TString fOutName;
  TFile * fOutFile;

  TString fOutType;

  ColorMap fColorMap;

  TH1IMap fTH1IMap;
  TH1DMap fTH1DMap;

  // branch stuff
  UInt_t          event;
  UInt_t          run;
  UInt_t          lumi;
  Double_t        wgt;
  Double_t        puwgt;
  //  Double_t        weight;
  Int_t           puobs;
  Int_t           putrue;
  UInt_t          nvtx;
  UInt_t          hltmet120;
  //  UInt_t          hltmet95jet80;
  //  UInt_t          hltmet105jet80;
  UInt_t          hltdoublemu;
  UInt_t          hltsinglemu;
  UInt_t          hltdoubleel;
  //  UInt_t          hltphoton135;
  //  UInt_t          hltphoton150;
  UInt_t          nmuons;
  UInt_t          nelectrons;
  UInt_t          nphotons;
  UInt_t          ntightmuons;
  UInt_t          ntightelectrons;
  //  UInt_t          ntightphotons;
  UInt_t          ntaus;
  UInt_t          njets;
  //  UInt_t          ncentjets;
  UInt_t          nbjets;
  UInt_t          nfatjets;
  //  UInt_t          ncentfatjets;
  //  UInt_t          njetsnotfat;
  //  UInt_t          ncentjetsnotfat;
  //  UInt_t          nbjetsnotfat;
  Double_t        pfmet;
  Double_t        pfmetphi;
  Double_t        t1pfmet;
  Double_t        t1pfmetphi;
  //  Double_t        calomet;
  //  Double_t        calometphi;
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
  //  Double_t        fatjetmass;
  //  Double_t        fatjetprunedmass;
  //  Double_t        fatjetmassdrop;
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
  Double_t        signaljetCHfrac;
  Double_t        signaljetNHfrac;
  Double_t        signaljetEMfrac;
  Double_t        signaljetCEMfrac;
  Double_t        signaljetmetdphi;
  Double_t        secondjetpt;
  Double_t        secondjeteta;
  Double_t        secondjetphi;
  Double_t        secondjetCHfrac;
  Double_t        secondjetNHfrac;
  Double_t        secondjetEMfrac;
  Double_t        secondjetCEMfrac;
  Double_t        secondjetmetdphi;
  Double_t        jetjetdphi;
  Double_t        jetmetdphimin;
  Double_t        thirdjetpt;
  Double_t        thirdjeteta;
  Double_t        thirdjetphi;
  Double_t        thirdjetCHfrac;
  Double_t        thirdjetNHfrac;
  Double_t        thirdjetEMfrac;
  Double_t        thirdjetCEMfrac;
  Double_t        thirdjetmetdphi;
  Double_t        ht;
  Double_t        dht;
  Double_t        mht;
  Double_t        alphat;
  //  Double_t        apcjet;
  //  Double_t        apcmet;
  //  Double_t        apcjetmet;
  //  Double_t        apcjetmax;
  Double_t        apcjetmetmax;
  //  Double_t        apcjetmin;
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

  // List of branches
  TBranch        *b_event;   //!
  TBranch        *b_run;   //!
  TBranch        *b_lumi;   //!
  TBranch        *b_wgt;   //!
  TBranch        *b_puwgt;   //!
  //  TBranch        *b_weight;   //!
  TBranch        *b_puobs;   //!
  TBranch        *b_putrue;   //!
  TBranch        *b_nvtx;   //!
  TBranch        *b_hltmet120;   //!
  //  TBranch        *b_hltmet95jet80;   //!
  //  TBranch        *b_hltmet105jet80;   //!
  TBranch        *b_hltdoublemu;   //!
  TBranch        *b_hltsinglemu;   //!
  TBranch        *b_hltdoubleel;   //!
  //  TBranch        *b_hltphoton135;   //!
  //  TBranch        *b_hltphoton150;   //!
  TBranch        *b_nmuons;   //!
  TBranch        *b_nelectrons;   //!
  TBranch        *b_nphotons;   //!
  TBranch        *b_ntightmuons;   //!
  TBranch        *b_ntightelectrons;   //!
  TBranch        *b_ntightphotons;   //!
  TBranch        *b_ntaus;   //!
  TBranch        *b_njets;   //!
  //  TBranch        *b_ncentjets;   //!
  TBranch        *b_nbjets;   //!
  TBranch        *b_nfatjets;   //!
  //  TBranch        *b_ncentfatjets;   //!
  //  TBranch        *b_njetsnotfat;   //!
  //  TBranch        *b_ncentjetsnotfat;   //!
  //  TBranch        *b_nbjetsnotfat;   //!
  TBranch        *b_pfmet;   //!
  TBranch        *b_pfmetphi;   //!
  TBranch        *b_t1pfmet;   //!
  TBranch        *b_t1pfmetphi;   //!
  //  TBranch        *b_calomet;   //!
  //  TBranch        *b_calometphi;   //!
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
  //  TBranch        *b_fatjetmass;   //!
  //  TBranch        *b_fatjetprunedmass;   //!
  //  TBranch        *b_fatjetmassdrop;   //!
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
  TBranch        *b_signaljetCHfrac;   //!
  TBranch        *b_signaljetNHfrac;   //!
  TBranch        *b_signaljetEMfrac;   //!
  TBranch        *b_signaljetCEMfrac;   //!
  TBranch        *b_signaljetmetdphi;   //!
  TBranch        *b_secondjetpt;   //!
  TBranch        *b_secondjeteta;   //!
  TBranch        *b_secondjetphi;   //!
  TBranch        *b_secondjetCHfrac;   //!
  TBranch        *b_secondjetNHfrac;   //!
  TBranch        *b_secondjetEMfrac;   //!
  TBranch        *b_secondjetCEMfrac;   //!
  TBranch        *b_secondjetmetdphi;   //!
  TBranch        *b_jetjetdphi;   //!
  TBranch        *b_jetmetdphimin;   //!
  TBranch        *b_thirdjetpt;   //!
  TBranch        *b_thirdjeteta;   //!
  TBranch        *b_thirdjetphi;   //!
  TBranch        *b_thirdjetCHfrac;   //!
  TBranch        *b_thirdjetNHfrac;   //!
  TBranch        *b_thirdjetEMfrac;   //!
  TBranch        *b_thirdjetCEMfrac;   //!
  TBranch        *b_thirdjetmetdphi;   //!
  TBranch        *b_ht;   //!
  TBranch        *b_dht;   //!
  TBranch        *b_mht;   //!
  TBranch        *b_alphat;   //!
  //  TBranch        *b_apcjet;   //!
  //  TBranch        *b_apcmet;   //!
  //  TBranch        *b_apcjetmet;   //!
  //  TBranch        *b_apcjetmax;   //!
  TBranch        *b_apcjetmetmax;   //!
  //  TBranch        *b_apcjetmin;   //!
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


};
