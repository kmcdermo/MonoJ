#include "TPad.h"
#include "TLatex.h"

void CMS_lumi(TPad* pad, Int_t iPosX) {   
  TString cmsText      = "CMS";
  Double_t cmsTextFont = 61;  // default is helvetic-bold
  
  Bool_t writeExtraText  = true;
  TString extraText      = "Preliminary";
  Double_t extraTextFont = 52;  // default is helvetica-italics

  TString lumiText = Form("#sqrt{s} = 13 TeV, L = %f fb^{-1}", fLumi);
  
  // text sizes and text offsets with respect to the top frame
  // in unit of the top margin size
  Double_t lumiTextSize     = 0.6;
  Double_t lumiTextOffset   = 0.2;
  Double_t cmsTextSize      = 0.75;
  Double_t cmsTextOffset    = 0.1;  // only used in outOfFrame version

  Double_t relPosX    = 0.045;
  Double_t relPosY    = 0.035;
  Double_t relExtraDY = 1.2;
 
  // ratio of "CMS" and extra text size
  Double_t extraOverCmsTextSize  = 0.76;
 
  Bool_t outOfFrame    = false;
  if ( iPosX/10==0 ) {
    outOfFrame = true;
  }

  Int_t alignY_=3;
  Int_t alignX_=2;
  if( iPosX/10==0 ) alignX_=1;
  if( iPosX==0    ) alignY_=1;
  if( iPosX/10==1 ) alignX_=1;
  if( iPosX/10==2 ) alignX_=2;
  if( iPosX/10==3 ) alignX_=3;
  Int_t align_ = 10*alignX_ + alignY_;

  Double_t H = pad->GetWh();
  Double_t W = pad->GetWw();
  Double_t l = pad->GetLeftMargin();
  Double_t t = pad->GetTopMargin();
  Double_t r = pad->GetRightMargin();
  Double_t b = pad->GetBottomMargin();
  Double_t e = 0.025;

  pad->cd();
  
  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);    

  Double_t extraTextSize = extraOverCmsTextSize*cmsTextSize;

  latex.SetTextFont(42);
  latex.SetTextAlign(31); 
  latex.SetTextSize(lumiTextSize*t);    
  latex.DrawLatex(1-r,1-t+lumiTextOffset*t,lumiText);

  if (outOfFrame) {
    latex.SetTextFont(cmsTextFont);
    latex.SetTextAlign(11); 
    latex.SetTextSize(cmsTextSize*t);    
    latex.DrawLatex(l,1-t+lumiTextOffset*t,cmsText);
  }
  
  pad->cd();

  Double_t posX_;
  if (iPosX % 10 <= 1) {
    posX_ =   l + relPosX*(1-l-r);
  }
  else if (iPosX % 10 == 2) {
    posX_ =  l + 0.5*(1-l-r);
  }
  else if (iPosX % 10 == 3) {
    posX_ =  1-r - relPosX*(1-l-r);
  }

  Double_t posY_ = 1-t - relPosY*(1-t-b);

  if (!outOfFrame) {
    latex.SetTextFont(cmsTextFont);
    latex.SetTextSize(cmsTextSize*t);
    latex.SetTextAlign(align_);
    latex.DrawLatex(posX_, posY_, cmsText);
    
    if (writeExtraText) {
      latex.SetTextFont(extraTextFont);
      latex.SetTextAlign(align_);
      latex.SetTextSize(extraTextSize*t);
      latex.DrawLatex(posX_, posY_- relExtraDY*cmsTextSize*t, extraText);
    }
  }
  
  else if (outOfFrame && writeExtraText){
    if (iPosX == 0) {
	posX_ = l +  relPosX*(1-l-r)+0.05;
	posY_ = 1-t+lumiTextOffset*t;
    }
    latex.SetTextFont(extraTextFont);
    latex.SetTextSize(extraTextSize*t);
    latex.SetTextAlign(align_);
    latex.DrawLatex(posX_, posY_, extraText);      
  }
}
