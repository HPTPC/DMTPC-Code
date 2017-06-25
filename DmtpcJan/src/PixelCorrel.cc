#include <assert.h>

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TLine.h"

#include "PixelCorrel.hh" 


ClassImp(dmtpc::ped::PixelCorrel); 

//=======================================
//=======================================
dmtpc::ped::PixelCorrel::PixelCorrel(TString pixName, int bin1, float err1, int bin2, float err2) {

  mBin1=bin1;  valErr1=err1;
  mBin2=bin2;  valErr2=err2;
  coreName=pixName;
  float mxAdu=2*(valErr1+valErr1);
  hC=new TH2I(coreName,"corr pixel "+coreName+ "; pix-1 (ADU); pix-2 (ADU)",50,-mxAdu,mxAdu,50,-mxAdu,mxAdu);
  //  hT->SetMarkerStyle(4);
  // hT->Sumw2();

#if 0
  { TList *tl=hT->GetListOfFunctions(); assert(tl);
    float yy=3*valErr;
    TLine *ln=new TLine( 0, yy, numFrame,yy);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
    ln=new TLine( 0, -yy, numFrame,-yy);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
    ln=new TLine( 0, 0, numFrame,0);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(3);
  }
#endif

}



//=======================================
//=======================================
void dmtpc::ped::PixelCorrel::fillFrom(TH2I *h2Dlight){
  float val1=h2Dlight->GetBinContent(mBin1);
  float val2=h2Dlight->GetBinContent(mBin2);
  // printf("%s fr=%d mBin=%d val=%.0f err=%.0f\n", coreName.Data(),frameId,mBin,frameId,val,valErr);
  hC->Fill(val1,val2);}


//=======================================
//=======================================
void dmtpc::ped::PixelCorrel::saveHisto(TFile *fd){
  fd->cd();
  hC->Write();
  printf("saving correl for %s\n",coreName.Data());
}
