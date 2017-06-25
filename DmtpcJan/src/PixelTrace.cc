#include <assert.h>

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TLine.h"

#include "PixelTrace.hh" 


ClassImp(dmtpc::ped::PixelTrace); 

//=======================================
//=======================================
dmtpc::ped::PixelTrace::PixelTrace(TString pixName,int mBinW, float valErrW,int numFrame) {

  mBin=mBinW;
  valErr=valErrW;
  coreName=pixName;
 
  hT=new TH1I(coreName,"trace pixel "+coreName+ "; frame ID; light (ADU)",numFrame,0.5,numFrame+0.5);
  hT->SetMarkerStyle(4);
  hT->Sumw2();

  { TList *tl=hT->GetListOfFunctions(); assert(tl);
    float yy=3*valErr;
    TLine *ln=new TLine( 0, yy, numFrame,yy);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
    ln=new TLine( 0, -yy, numFrame,-yy);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
    ln=new TLine( 0, 0, numFrame,0);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(3);
  }

}


//=======================================
//=======================================
dmtpc::ped::PixelTrace::~PixelTrace() {
  //printf(" ~PixelTrace() done\n"); 

} 




//=======================================
//=======================================
void dmtpc::ped::PixelTrace::fillFrom(TH2I *h2Dlight, int frameId){
  float val=h2Dlight->GetBinContent(mBin);
  // printf("%s fr=%d mBin=%d val=%.0f err=%.0f\n", coreName.Data(),frameId,mBin,frameId,val,valErr);
  hT->SetBinContent(frameId,val);
  hT->SetBinError(frameId,valErr);
}


//=======================================
//=======================================
void dmtpc::ped::PixelTrace::saveHisto(TFile *fd){
  fd->cd();
  hT->Write();
  printf("saving trace for %s\n",coreName.Data());
}
