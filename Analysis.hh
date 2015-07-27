#include "TROOT.h"
#include "TStyle.h"

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH1I.h"

#include "TString.h"
#include "TStyle.h"
#include "TColor.h"

#include <utility>
#include <map>
#include <vector>
#include <iostream>

typedef std::vector<TString>  TStrVec;
typedef std::vector<UInt_t>   UIntVec;
typedef std::vector<Int_t>    IntVec;
typedef std::vector<Double_t> DblVec;

typedef std::map<TString,TH1D*> TH1DMap;
typedef TH1DMap::iterator TH1DMapIter;
typedef std::map<TString,TH1I*> TH1IMap;
typedef TH1IMap::iterator TH1IMapIter;

typedef std::map<TString,Color_t> ColorMap;

class Analysis
{
public:

  Analysis(const TString sample, const Bool_t isMC, const Double_t lumi, const TString outType);
  
  void SetBranchAddresses();
  void DoAnalysis();
  void SetUpPlots();
  TH1D * MakeTH1DPlot(const TString hname, const TString htitle, const Int_t nbins, const Double_t xlow, const Double_t xhigh, const TString xtitle, const TString ytitle);
  TH1I * MakeTH1IPlot(const TString hname, const TString htitle, const Int_t nbins, const Int_t xlow, const Int_t xhigh, const TString xtitle, const TString ytitle);
  void SaveHists();
    
  ~Analysis();
  void DeleteHists();

private:
  // initialized in constructor
  TString fSample;
  Bool_t fIsMC;
  Double_t fLumi;
  
  TFile * fInFile;
  TTree * fInTree;

  TString fOutName;
  TFile * fOutFile;

  TString fOutType;

  TH1IMap fTH1IMap;
  TH1DMap fTH1DMap;

  ColorMap fColorMap;

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

void Analysis::SetBranchAddresses() {
  fInTree->SetBranchAddress("event", &event, &b_event);
  fInTree->SetBranchAddress("run", &run, &b_run);
  fInTree->SetBranchAddress("lumi", &lumi, &b_lumi);
  fInTree->SetBranchAddress("xsec", &xsec, &b_xsec);
  fInTree->SetBranchAddress("wgt", &wgt, &b_wgt);
  fInTree->SetBranchAddress("kfact", &kfact, &b_kfact);
  fInTree->SetBranchAddress("puwgt", &puwgt, &b_puwgt);
  fInTree->SetBranchAddress("puobs", &puobs, &b_puobs);
  fInTree->SetBranchAddress("putrue", &putrue, &b_putrue);
  fInTree->SetBranchAddress("nvtx", &nvtx, &b_nvtx);
  fInTree->SetBranchAddress("hltmet90", &hltmet90, &b_hltmet90);
  fInTree->SetBranchAddress("hltmet120", &hltmet120, &b_hltmet120);
  fInTree->SetBranchAddress("hltmetwithmu90", &hltmetwithmu90, &b_hltmetwithmu90);
  fInTree->SetBranchAddress("hltmetwithmu120", &hltmetwithmu120, &b_hltmetwithmu120);
  fInTree->SetBranchAddress("hltmetwithmu170", &hltmetwithmu170, &b_hltmetwithmu170);
  fInTree->SetBranchAddress("hltmetwithmu300", &hltmetwithmu300, &b_hltmetwithmu300);
  fInTree->SetBranchAddress("hltjetmet90", &hltjetmet90, &b_hltjetmet90);
  fInTree->SetBranchAddress("hltjetmet120", &hltjetmet120, &b_hltjetmet120);
  fInTree->SetBranchAddress("hltphoton165", &hltphoton165, &b_hltphoton165);
  fInTree->SetBranchAddress("hltphoton175", &hltphoton175, &b_hltphoton175);
  fInTree->SetBranchAddress("hltdoublemu", &hltdoublemu, &b_hltdoublemu);
  fInTree->SetBranchAddress("hltsinglemu", &hltsinglemu, &b_hltsinglemu);
  fInTree->SetBranchAddress("hltdoubleel", &hltdoubleel, &b_hltdoubleel);
  fInTree->SetBranchAddress("hltsingleel", &hltsingleel, &b_hltsingleel);
  fInTree->SetBranchAddress("flagcsctight", &flagcsctight, &b_flagcsctight);
  fInTree->SetBranchAddress("flaghbhenoise", &flaghbhenoise, &b_flaghbhenoise);
  fInTree->SetBranchAddress("flaghcallaser", &flaghcallaser, &b_flaghcallaser);
  fInTree->SetBranchAddress("flagecaltrig", &flagecaltrig, &b_flagecaltrig);
  fInTree->SetBranchAddress("flageebadsc", &flageebadsc, &b_flageebadsc);
  fInTree->SetBranchAddress("flagecallaser", &flagecallaser, &b_flagecallaser);
  fInTree->SetBranchAddress("flagtrkfail", &flagtrkfail, &b_flagtrkfail);
  fInTree->SetBranchAddress("flagtrkpog", &flagtrkpog, &b_flagtrkpog);
  fInTree->SetBranchAddress("flaghnoiseloose", &flaghnoiseloose, &b_flaghnoiseloose);
  fInTree->SetBranchAddress("flaghnoisetight", &flaghnoisetight, &b_flaghnoisetight);
  fInTree->SetBranchAddress("flaghnoisehilvl", &flaghnoisehilvl, &b_flaghnoisehilvl);
  fInTree->SetBranchAddress("nmuons", &nmuons, &b_nmuons);
  fInTree->SetBranchAddress("nelectrons", &nelectrons, &b_nelectrons);
  fInTree->SetBranchAddress("ntightmuons", &ntightmuons, &b_ntightmuons);
  fInTree->SetBranchAddress("ntightelectrons", &ntightelectrons, &b_ntightelectrons);
  fInTree->SetBranchAddress("ntaus", &ntaus, &b_ntaus);
  fInTree->SetBranchAddress("njets", &njets, &b_njets);
  fInTree->SetBranchAddress("nbjets", &nbjets, &b_nbjets);
  fInTree->SetBranchAddress("nfatjets", &nfatjets, &b_nfatjets);
  fInTree->SetBranchAddress("nphotons", &nphotons, &b_nphotons);
  fInTree->SetBranchAddress("pfmet", &pfmet, &b_pfmet);
  fInTree->SetBranchAddress("pfmetphi", &pfmetphi, &b_pfmetphi);
  fInTree->SetBranchAddress("t1pfmet", &t1pfmet, &b_t1pfmet);
  fInTree->SetBranchAddress("t1pfmetphi", &t1pfmetphi, &b_t1pfmetphi);
  fInTree->SetBranchAddress("pfmupt", &pfmupt, &b_pfmupt);
  fInTree->SetBranchAddress("pfmuphi", &pfmuphi, &b_pfmuphi);
  fInTree->SetBranchAddress("mumet", &mumet, &b_mumet);
  fInTree->SetBranchAddress("mumetphi", &mumetphi, &b_mumetphi);
  fInTree->SetBranchAddress("phmet", &phmet, &b_phmet);
  fInTree->SetBranchAddress("phmetphi", &phmetphi, &b_phmetphi);
  fInTree->SetBranchAddress("t1mumet", &t1mumet, &b_t1mumet);
  fInTree->SetBranchAddress("t1mumetphi", &t1mumetphi, &b_t1mumetphi);
  fInTree->SetBranchAddress("t1phmet", &t1phmet, &b_t1phmet);
  fInTree->SetBranchAddress("t1phmetphi", &t1phmetphi, &b_t1phmetphi);
  fInTree->SetBranchAddress("fatjetpt", &fatjetpt, &b_fatjetpt);
  fInTree->SetBranchAddress("fatjeteta", &fatjeteta, &b_fatjeteta);
  fInTree->SetBranchAddress("fatjetphi", &fatjetphi, &b_fatjetphi);
  fInTree->SetBranchAddress("fatjetprmass", &fatjetprmass, &b_fatjetprmass);
  fInTree->SetBranchAddress("fatjetsdmass", &fatjetsdmass, &b_fatjetsdmass);
  fInTree->SetBranchAddress("fatjettrmass", &fatjettrmass, &b_fatjettrmass);
  fInTree->SetBranchAddress("fatjetftmass", &fatjetftmass, &b_fatjetftmass);
  fInTree->SetBranchAddress("fatjettau2", &fatjettau2, &b_fatjettau2);
  fInTree->SetBranchAddress("fatjettau1", &fatjettau1, &b_fatjettau1);
  fInTree->SetBranchAddress("fatjetCHfrac", &fatjetCHfrac, &b_fatjetCHfrac);
  fInTree->SetBranchAddress("fatjetNHfrac", &fatjetNHfrac, &b_fatjetNHfrac);
  fInTree->SetBranchAddress("fatjetEMfrac", &fatjetEMfrac, &b_fatjetEMfrac);
  fInTree->SetBranchAddress("fatjetCEMfrac", &fatjetCEMfrac, &b_fatjetCEMfrac);
  fInTree->SetBranchAddress("fatjetmetdphi", &fatjetmetdphi, &b_fatjetmetdphi);
  fInTree->SetBranchAddress("signaljetpt", &signaljetpt, &b_signaljetpt);
  fInTree->SetBranchAddress("signaljeteta", &signaljeteta, &b_signaljeteta);
  fInTree->SetBranchAddress("signaljetphi", &signaljetphi, &b_signaljetphi);
  fInTree->SetBranchAddress("signaljetbtag", &signaljetbtag, &b_signaljetbtag);
  fInTree->SetBranchAddress("signaljetCHfrac", &signaljetCHfrac, &b_signaljetCHfrac);
  fInTree->SetBranchAddress("signaljetNHfrac", &signaljetNHfrac, &b_signaljetNHfrac);
  fInTree->SetBranchAddress("signaljetEMfrac", &signaljetEMfrac, &b_signaljetEMfrac);
  fInTree->SetBranchAddress("signaljetCEMfrac", &signaljetCEMfrac, &b_signaljetCEMfrac);
  fInTree->SetBranchAddress("signaljetmetdphi", &signaljetmetdphi, &b_signaljetmetdphi);
  fInTree->SetBranchAddress("secondjetpt", &secondjetpt, &b_secondjetpt);
  fInTree->SetBranchAddress("secondjeteta", &secondjeteta, &b_secondjeteta);
  fInTree->SetBranchAddress("secondjetphi", &secondjetphi, &b_secondjetphi);
  fInTree->SetBranchAddress("secondjetbtag", &secondjetbtag, &b_secondjetbtag);
  fInTree->SetBranchAddress("secondjetCHfrac", &secondjetCHfrac, &b_secondjetCHfrac);
  fInTree->SetBranchAddress("secondjetNHfrac", &secondjetNHfrac, &b_secondjetNHfrac);
  fInTree->SetBranchAddress("secondjetEMfrac", &secondjetEMfrac, &b_secondjetEMfrac);
  fInTree->SetBranchAddress("secondjetCEMfrac", &secondjetCEMfrac, &b_secondjetCEMfrac);
  fInTree->SetBranchAddress("secondjetmetdphi", &secondjetmetdphi, &b_secondjetmetdphi);
  fInTree->SetBranchAddress("thirdjetpt", &thirdjetpt, &b_thirdjetpt);
  fInTree->SetBranchAddress("thirdjeteta", &thirdjeteta, &b_thirdjeteta);
  fInTree->SetBranchAddress("thirdjetphi", &thirdjetphi, &b_thirdjetphi);
  fInTree->SetBranchAddress("thirdjetbtag", &thirdjetbtag, &b_thirdjetbtag);
  fInTree->SetBranchAddress("thirdjetCHfrac", &thirdjetCHfrac, &b_thirdjetCHfrac);
  fInTree->SetBranchAddress("thirdjetNHfrac", &thirdjetNHfrac, &b_thirdjetNHfrac);
  fInTree->SetBranchAddress("thirdjetEMfrac", &thirdjetEMfrac, &b_thirdjetEMfrac);
  fInTree->SetBranchAddress("thirdjetCEMfrac", &thirdjetCEMfrac, &b_thirdjetCEMfrac);
  fInTree->SetBranchAddress("thirdjetmetdphi", &thirdjetmetdphi, &b_thirdjetmetdphi);
  fInTree->SetBranchAddress("jetjetdphi", &jetjetdphi, &b_jetjetdphi);
  fInTree->SetBranchAddress("jetmetdphimin", &jetmetdphimin, &b_jetmetdphimin);
  fInTree->SetBranchAddress("incjetmetdphimin", &incjetmetdphimin, &b_incjetmetdphimin);
  fInTree->SetBranchAddress("ht", &ht, &b_ht);
  fInTree->SetBranchAddress("dht", &dht, &b_dht);
  fInTree->SetBranchAddress("mht", &mht, &b_mht);
  fInTree->SetBranchAddress("alphat", &alphat, &b_alphat);
  fInTree->SetBranchAddress("apcjetmetmax", &apcjetmetmax, &b_apcjetmetmax);
  fInTree->SetBranchAddress("apcjetmetmin", &apcjetmetmin, &b_apcjetmetmin);
  fInTree->SetBranchAddress("mu1pid", &mu1pid, &b_mu1pid);
  fInTree->SetBranchAddress("mu1pt", &mu1pt, &b_mu1pt);
  fInTree->SetBranchAddress("mu1eta", &mu1eta, &b_mu1eta);
  fInTree->SetBranchAddress("mu1phi", &mu1phi, &b_mu1phi);
  fInTree->SetBranchAddress("mu1id", &mu1id, &b_mu1id);
  fInTree->SetBranchAddress("mu2pid", &mu2pid, &b_mu2pid);
  fInTree->SetBranchAddress("mu2pt", &mu2pt, &b_mu2pt);
  fInTree->SetBranchAddress("mu2eta", &mu2eta, &b_mu2eta);
  fInTree->SetBranchAddress("mu2phi", &mu2phi, &b_mu2phi);
  fInTree->SetBranchAddress("mu2id", &mu2id, &b_mu2id);
  fInTree->SetBranchAddress("el1pid", &el1pid, &b_el1pid);
  fInTree->SetBranchAddress("el1pt", &el1pt, &b_el1pt);
  fInTree->SetBranchAddress("el1eta", &el1eta, &b_el1eta);
  fInTree->SetBranchAddress("el1phi", &el1phi, &b_el1phi);
  fInTree->SetBranchAddress("el1id", &el1id, &b_el1id);
  fInTree->SetBranchAddress("el2pid", &el2pid, &b_el2pid);
  fInTree->SetBranchAddress("el2pt", &el2pt, &b_el2pt);
  fInTree->SetBranchAddress("el2eta", &el2eta, &b_el2eta);
  fInTree->SetBranchAddress("el2phi", &el2phi, &b_el2phi);
  fInTree->SetBranchAddress("el2id", &el2id, &b_el2id);
  fInTree->SetBranchAddress("zmass", &zmass, &b_zmass);
  fInTree->SetBranchAddress("zpt", &zpt, &b_zpt);
  fInTree->SetBranchAddress("zeta", &zeta, &b_zeta);
  fInTree->SetBranchAddress("zphi", &zphi, &b_zphi);
  fInTree->SetBranchAddress("wmt", &wmt, &b_wmt);
  fInTree->SetBranchAddress("emumass", &emumass, &b_emumass);
  fInTree->SetBranchAddress("emupt", &emupt, &b_emupt);
  fInTree->SetBranchAddress("emueta", &emueta, &b_emueta);
  fInTree->SetBranchAddress("emuphi", &emuphi, &b_emuphi);
  fInTree->SetBranchAddress("zeemass", &zeemass, &b_zeemass);
  fInTree->SetBranchAddress("zeept", &zeept, &b_zeeept);
  fInTree->SetBranchAddress("zeeeta", &zeeeta, &b_zeeeta);
  fInTree->SetBranchAddress("zeephi", &zeephi, &b_zeephi);
  fInTree->SetBranchAddress("wemt", &wemt, &b_wemt);
  fInTree->SetBranchAddress("phpt", &phpt, &b_phpt);
  fInTree->SetBranchAddress("pheta", &pheta, &b_pheta);
  fInTree->SetBranchAddress("phphi", &phphi, &b_phphi);
  fInTree->SetBranchAddress("loosephpt", &loosephpt, &b_loosephpt);
  fInTree->SetBranchAddress("loosepheta", &loosepheta, &b_loosepheta);
  fInTree->SetBranchAddress("loosephphi", &loosephphi, &b_loosephphi);
  fInTree->SetBranchAddress("loosephsieie", &loosephsieie, &b_loosephsieie);
  fInTree->SetBranchAddress("loosephrndiso", &loosephrndiso, &b_loosephrndiso);
  fInTree->SetBranchAddress("wzid", &wzid, &b_wzid);
  fInTree->SetBranchAddress("wzmass", &wzmass, &b_wzmass);
  fInTree->SetBranchAddress("wzmt", &wzmt, &b_wzmt);
  fInTree->SetBranchAddress("wzpt", &wzpt, &b_wzpt);
  fInTree->SetBranchAddress("wzeta", &wzeta, &b_wzeta);
  fInTree->SetBranchAddress("wzphi", &wzphi, &b_wzphi);
  fInTree->SetBranchAddress("l1id", &l1id, &b_l1id);
  fInTree->SetBranchAddress("l1pt", &l1pt, &b_l1pt);
  fInTree->SetBranchAddress("l1eta", &l1eta, &b_l1eta);
  fInTree->SetBranchAddress("l1phi", &l1phi, &b_l1phi);
  fInTree->SetBranchAddress("l2id", &l2id, &b_l2id);
  fInTree->SetBranchAddress("l2pt", &l2pt, &b_l2pt);
  fInTree->SetBranchAddress("l2eta", &l2eta, &b_l2eta);
  fInTree->SetBranchAddress("l2phi", &l2phi, &b_l2phi);
  fInTree->SetBranchAddress("i1id", &i1id, &b_i1id);
  fInTree->SetBranchAddress("i1pt", &i1pt, &b_i1pt);
  fInTree->SetBranchAddress("i1eta", &i1eta, &b_i1eta);
  fInTree->SetBranchAddress("i1phi", &i1phi, &b_i1phi);
  fInTree->SetBranchAddress("i2id", &i2id, &b_i2id);
  fInTree->SetBranchAddress("i2pt", &i2pt, &b_i2pt);
  fInTree->SetBranchAddress("i2eta", &i2eta, &b_i2eta);
  fInTree->SetBranchAddress("i2phi", &i2phi, &b_i2phi);
  fInTree->SetBranchAddress("i3id", &i3id, &b_i3id);
  fInTree->SetBranchAddress("i3pt", &i3pt, &b_i3pt);
  fInTree->SetBranchAddress("i3eta", &i3eta, &b_i3eta);
  fInTree->SetBranchAddress("i3phi", &i3phi, &b_i3phi);
  fInTree->SetBranchAddress("cflagcsctight", &cflagcsctight, &b_cflagcsctight);
  fInTree->SetBranchAddress("cflaghbhenoise", &cflaghbhenoise, &b_cflaghbhenoise);
  fInTree->SetBranchAddress("cflaghcallaser", &cflaghcallaser, &b_cflaghcallaser);
  fInTree->SetBranchAddress("cflagecaltrig", &cflagecaltrig, &b_cflagecaltrig);
  fInTree->SetBranchAddress("cflageebadsc", &cflageebadsc, &b_cflageebadsc);
  fInTree->SetBranchAddress("cflagecallaser", &cflagecallaser, &b_cflagecallaser);
  fInTree->SetBranchAddress("cflagtrkfail", &cflagtrkfail, &b_cflagtrkfail);
  fInTree->SetBranchAddress("cflagtrkpog", &cflagtrkpog, &b_cflagtrkpog);
  fInTree->SetBranchAddress("wgtsum", &wgtsum, &b_wgtsum);
}
