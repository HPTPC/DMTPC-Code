#include <assert.h>
#include "TH2.h"
#include "TH2I.h"
#include "TList.h"
#include "TLine.h"

#include "Dataset.hh"
#include "Event.hh"
#include "Image.hh"
#include "CameraInfo.hh"

#include "M3StarSparkDet.hh" 
#include "M3JanUtil.hh" 

ClassImp(dmtpc::jana::M3StarSparkDet); 

//=======================================
//=======================================
dmtpc::jana::M3StarSparkDet::M3StarSparkDet() {
  //  printf(" M3StarSparkDet() cnstr\n"); 
  runId= mxFrame=0;
  coreName="fixMe";
  camSN="fixMe88";
  quadId = camDaq=0;
  par_aduH=5000;
  par_nHotPixL=100;

  h2GarbCol=0;
 }

//=======================================
//=======================================
void dmtpc::jana::M3StarSparkDet::clear() {
  nHotPix=0; 
  if(h2GarbCol) {
    delete h2GarbCol;
    h2GarbCol=0;
  }
}
//=======================================
//=======================================
void dmtpc::jana::M3StarSparkDet::initDims(const dmtpc::core::Dataset *ds , int quadid,  int camsrc) {
  clear();
  quadId=quadid;
  camDaq=camsrc;

  printf(" M3StarSparkDet()::initDims  quadId=%d  camDaq=%d  pixThre=%.3e adu \n", quadId,camDaq,par_aduH);

  const dmtpc::core::Event * eve=ds->event();
  const dmtpc::core::Image *image=eve->getImage(camDaq); assert(image);
  const dmtpc::core::CameraInfo *info=image->getInfo(); assert(info);
  camSN=info->serialNumber; 

  mxFrame=ds->nevents();  
  runId=eve->run();
  coreName=Form("hpxR%dQ%d",runId,quadId);
  
  int ncam_in =eve->nccd();
  
  printf(" First  image camDaq=%d  SN=%s   temp/C=%.1f\n",camDaq, info->serialNumber.Data(), info->ccdTemp);
  assert(camDaq>=0); 
  assert(camDaq<ncam_in);

  M3JanUtil::verifyQuadId(quadId,camSN);

  hHotPix=new TH1I(coreName, Form("Hot pix per frame  R%d quad=%d; frame Id",runId,quadId), mxFrame, -0.5,  mxFrame-0.5);
  hHotPix->SetFillColor(kYellow);
  { TList *tl=hHotPix->GetListOfFunctions(); assert(tl);
    double cutVal=par_nHotPixL;
    TLine *ln=new TLine(0,cutVal, mxFrame, cutVal);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
  }

  

}


//=======================================
//=======================================
void dmtpc::jana::M3StarSparkDet::evalQuad(const dmtpc::core::Event * eve ){
  assert(hHotPix);
  clear();

  int frameId=eve->ev();
  TH2I * h2Raw=(TH2I*) eve->ccdData(camDaq); assert(h2Raw);
  
  if(h2Raw->GetNbinsX()==764)  h2GarbCol=h2Raw=M3JanUtil::convert764toOS( h2Raw,quadId);
  
  int nchX=h2Raw->GetNbinsX();
  int nchY=h2Raw->GetNbinsY(); 

  // printf(" M3StarSparkDet::evalQuad  nchX,Y=%d,%d\n",nchX,nchY);
  assert(1542==nchX || 1028==nchX || 771==nchX ); // cuts tuned only for 2x2, 3x3,4x4 binning w/ OS

  // add frame to  big-ped histogram
  for(int jchX=1; jchX<=nchX; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N
      float rawVal=h2Raw->GetBinContent(jchX,jchY);
      if(rawVal<par_aduH) continue;
      nHotPix++;
    }

  hHotPix->Fill(frameId,nHotPix);
}
