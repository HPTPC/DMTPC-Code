#include <assert.h>
#include "TH2.h"
#include "TH2I.h"

#include "TF1.h"
#include "TFile.h"
#include "TString.h"
#include "TText.h"
#include "TStyle.h"

#include "Dataset.hh"
#include "Event.hh"
#include "Image.hh"
#include "CameraInfo.hh"


#include "TCanvas.h"
#include "TLine.h"
#include "TList.h"
#include "TLine.h"
#include "TList.h"
#include "TRandom.h" // for PDF sampling

#include "M3CcdCalibMaker.hh" 

#include "M3CcdImageUtil.hh" 
#include "M3JanEvent.hh" 

ClassImp(dmtpc::ped::M3CcdCalibMaker); 

//=======================================
//=======================================
dmtpc::ped::M3CcdCalibMaker::M3CcdCalibMaker() {
  // printf(" M3CcdCalibMaker() cnstr\n"); 
  NtotEve=0;
  jEve=0;
  camId=-1;
  camSrc=-1;
  coreName="fixMe";
  par_fixRawHistDims=1; // apply raw data corrections if necessary

  par_bin2mm_4shutter_1x1=4; // 1 bin=250 um

  hPedAvr=hPedStat=0;
  h2Dtempl=h2Dlight=h2GarbCol=h2DlightRot=0;
  setUserRebin(1);

}


//=======================================
//=======================================
dmtpc::ped::M3CcdCalibMaker::~M3CcdCalibMaker() {
  printf(" ~M3CcdCalibMaker() done\n"); 
}
 
//=======================================
//=======================================
void dmtpc::ped::M3CcdCalibMaker::clear() {
  h2Dlight->Reset("M"); //"M" is specified, resets also Minimum and Maximum
  h2Dlight->SetAxisRange(1,-1,"x");
  h2Dlight->SetAxisRange(1,-1,"y");

  h2DlightRot->Reset("M"); 
  h2DlightRot->SetAxisRange(1,-1,"x");
  h2DlightRot->SetAxisRange(1,-1,"y");

  if(h2GarbCol) {
    delete h2GarbCol;
    h2GarbCol=0;
  }
  // printf(" M3CcdCalibMaker::clear() done\n"); 
}

//=======================================
//=======================================
void dmtpc::ped::M3CcdCalibMaker::calibrateImage(const dmtpc::core::Event * eve) {
  assert(jEve); // to veryfy output is set
  assert(jEve->light[camId].hPedStat);// to verify calibration was read-in
  assert(eve);
  clear();
  NtotEve++;
  
  jEve->frameId=eve->ev(); 

  h2Dlight->SetNameTitle(coreName+Form("_fr%d_light",jEve->frameId),coreName+Form(" fr%d R%d, Light (ADU), nBinX=%d",jEve->frameId,jEve->runId,h2Dlight->GetNbinsX()));  
 
  int ncam_in =eve->nccd();  assert(camSrc<ncam_in);

  if(jEve->frameId%10==0 || NtotEve==1) {
    const dmtpc::core::CameraInfo *info=eve->getImage(camSrc)->getInfo();
    printf(" calibrateImage reading4ped  frame=%d image camSrc=%d camId=%d SN=%s   temp/C=%.1f\n", eve->ev(),camSrc,camId, info->serialNumber.Data(), info->ccdTemp);
  }
  
  TH2I * h2Raw=(TH2I * )eve->ccdData(camSrc); assert(h2Raw);
  
  //... handle other data formats from/before 2015
  if (par_fixRawHistDims) {
    if(h2Raw->GetNbinsX()==764)  h2GarbCol=h2Raw=M3CcdImageUtil::convert764toOS( h2Raw,camId);

  }

  //....  further rebin image if requested

  if(par_userRebin!=1) {
    h2Raw->Rebin2D(par_userRebin,par_userRebin);
  }

  // basic sanity check
  assert(h2Raw->GetNbinsX() == h2Dtempl->GetNbinsX());
  assert(h2Raw->GetNbinsX() == hPedStat->GetNbinsX());
  assert(h2Raw->GetNbinsX() == h2Raw->GetNbinsY());
  assert(h2Raw->GetXaxis()->GetXmax()==hPedAvr->GetXaxis()->GetXmax());
  if(0) {
    h2Raw->Print();
    hPedAvr->Print();
    h2Dlight->Print();
    printf(" xMax raw=%f  avr=%f\n", h2Raw->GetXaxis()->GetXmax(),hPedAvr->GetXaxis()->GetXmax());
  }
  //.... subtract pedestals  & propagate error of pedestals
  h2Dlight->Add(h2Raw,hPedAvr, 1., -1.);  // WARN: do not use light->error() is worong
  int kb=100/par_userRebin;
  if(NtotEve==1)
    printf("  e.g. uReb data :kb=%d  raw=%f, ped=%f rms=%f  light=%f\n",kb, h2Raw->GetBinContent(kb,kb),hPedAvr->GetBinContent(kb,kb), hPedAvr->GetBinError(kb,kb), h2Dlight->GetBinContent(kb,kb));

  // ... clear masked pixels
  int nchX=hPedStat->GetNbinsX();
  int nchY=hPedStat->GetNbinsY();
  for(int jchX=1; jchX<=nchX; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N
      if (  hPedStat->GetBinContent(jchX,jchY) ) { // drop masked channels
	h2Dlight->SetBinContent(jchX,jchY,0.); 
	h2Dlight->SetBinError(jchX,jchY,0.); // set error to 0
	continue; // never use them for anything
      } else { 
	float ltErr=hPedAvr->GetBinError(jchX,jchY);
	h2Dlight->SetBinError(jchX,jchY,ltErr); // set err to pedRMS
      }
      
      // ... code below is only for monitoring, drop it later

      //... print pixels w/  huge ADU
      float val=h2Raw->GetBinContent(jchX,jchY); 
      float xPix= h2Raw->GetXaxis()->GetBinCenter(jchX);
      float yPix= h2Raw->GetYaxis()->GetBinCenter(jchY);

      //      if(val>60000) 
      //printf("huge_raw val=%.3e xPix=%.0f yPix=%.0f\n",val,xPix,yPix);
      //	printf("calMk.addTrace(%d,%d,nExpo);\n",val,xPix,yPix);
      
      //format: calMk.addPixTrace(500,500,nExpo);
#if 0
      float ped=hPedAvr->GetBinContent(jchX,jchY); // ADU value from CCD
      float light=val-ped;
      if(jchX%77==0 && jchY%99==0 )
	printf("light=%.3e val=%.3e  ped=%.3e  xPix=%.0f yPix=%.0f\n",light,val,ped,xPix,yPix);
#endif
    }// end of X,Y


  // trace light in selected pixels
  for (int ip=0; ip<traceV.size();ip++) {
    traceV[ip].fillFrom(h2Dlight,jEve->frameId);
  }

  // correlation of light in selected pixels
  for (int ip=0; ip<correlV.size();ip++) {
    correlV[ip].fillFrom(h2Dlight);
  }


}


//=======================================
//=======================================
void dmtpc::ped::M3CcdCalibMaker::saveHisto(TFile *fd) {

  printf(" %s histos  are written  to '%s'" ,coreName.Data(),fd->GetName());
  for(int i=0;i<mxH;i++) {
    if(hA[i]) hA[i]->Write();
  }

  // trace selected pixels
  for (int ip=0; ip<traceV.size();ip++) {
    traceV[ip].saveHisto(fd);
  }

  // corel selected pixels
  for (int ip=0; ip<correlV.size();ip++) {
    correlV[ip].saveHisto(fd);
  }

  hPedStat->Write();
  printf(" , save Ok\n");

}



//=======================================
//=======================================
void dmtpc::ped::M3CcdCalibMaker::load_pedStat(TString fName) {

  TString tit="cal_";
  TFile *fd=new TFile(fName);
  printf("M3CcdCalibMaker::load_pedStat opening %s for camId=%d\n",fName.Data(), camId);
  assert(fd->IsOpen());

  h2Dtempl=(TH2I*) fd->Get(Form("cam%d_2dTempl",camId)); assert( h2Dtempl);

  hPedStat=(TH2I*) fd->Get(Form("cam%d_pedStat",camId)); assert( hPedStat);
  hPedAvr =(TH2I*) fd->Get(Form("cam%d_pedAvr",camId)); assert( hPedAvr);

  int kb=100;
  printf("e.g.:kb=%d  ped=%f, rms=%f\n",kb, hPedAvr->GetBinContent(kb,kb), hPedAvr->GetBinError(kb,kb));

  printf(" Read-in calibration  from '%s'  \n" ,fd->GetName());
}


//=======================================
//=======================================
void dmtpc::ped::M3CcdCalibMaker::initHisto() {
  assert(h2Dtempl); //  an event was already used to define dimensions

  memset(hA,0,sizeof(hA)); // input spectra, unweighted
  TH1* h;
 
  enum {mxCase=8};
  int nCase=mxCase;
  hA[0]=h=new TH1D(coreName+"_calCase","QA of pedestals; ; channel count",nCase,0,nCase);  h->GetXaxis()->SetTitleOffset(0.4);  h->GetXaxis()->SetLabelSize(0.06);  h->GetXaxis()->SetTitleSize(0.05); h->SetMinimum(0.8);  h->SetLineColor(kBlue); h->SetFillColor(42);h->SetLineWidth(2);  h->SetMarkerSize(2);//<-- large text // no-W
    
  
  TString nameA[mxCase]={"FRAME","XlowSum","XlowAmpl","XpedRmsHi","XpedAvrHi","good","xx","yy"};
 for(int i=0;i<mxCase;i++) {
   hA[0]->Fill(nameA[i],0.);
 }


 // 1D histos 10 ..19 :  pedestal as used
 int aduLo=1900*par_userRebin*par_userRebin, aduHi=2900*par_userRebin*par_userRebin;
 hA[10]=h=new TH1D(coreName+"_UpedAvr",coreName+Form(" profile of pedestal value, userRebin=%d; pedestal value(ADU); pixel count",par_userRebin), 200,aduLo,aduHi);
 hA[11]=h=new TH1D(coreName+"_UpedRms",coreName+Form(" profile of pedestal RMS, userRebin=%d; pedestal RMS (ADU); pixel count",par_userRebin), 100,0,40*par_userRebin);

}

//=======================================
//=======================================
void dmtpc::ped::M3CcdCalibMaker::initDims(const dmtpc::core::Dataset *ds ) {
  assert( h2Dtempl);
  assert(jEve);
  printf(" M3CcdCalibMaker()::initDims  camId=%d  camSrc=%d  \n", camId,camSrc);
  coreName=Form("cam%dcal",camId);

  const dmtpc::core::Event * eve=ds->event();

  const dmtpc::core::Image *image=eve->getImage(camSrc); assert(image);
  const dmtpc::core::CameraInfo *info=image->getInfo(); assert(info);
  

  jEve->light[camId].camSrc=camSrc;
  jEve->light[camId].camSN=info->serialNumber;

  int ncam_in =eve->nccd();
  printf(" First frame=%d image camSrc=%d  SN=%s   temp/C=%.1f\n", eve->ev() ,camSrc, info->serialNumber.Data(), info->ccdTemp);

  assert(camSrc>=0); 
  assert(camSrc<ncam_in);
 
  TH2I * h2raw=(TH2I * )eve->ccdData(camSrc); assert(h2raw);

  // handle other data formats from 2015
  if (par_fixRawHistDims) {
    if(h2raw->GetNbinsX()==764)  h2GarbCol=h2raw=M3CcdImageUtil::convert764toOS( h2raw,camId);    
  }
  
  const int nbinsx =  h2raw->GetNbinsX();
  const int nbinsy =  h2raw->GetNbinsY();
  const double xmax = h2raw->GetXaxis()->GetXmax();
  int  daqRebin=(int)( xmax/nbinsx+0.1);
  printf("Frame header info: ncam_in=%d,nbinsx=%d,nbinsy=%d daqRebon=%d\n", ncam_in, nbinsx,nbinsy,daqRebin);

  initHisto();  

  // I'm not dealing with a non-square image format
  assert(nbinsx==nbinsy);
  
  assert(h2Dtempl->GetNbinsX()==nbinsx);
  assert(h2Dtempl->GetNbinsY()==nbinsy);
  
  printf(" M3CcdCalibMaker()::initDims userRebin(%d)   \n",par_userRebin);
  
  int agregRebin=daqRebin*par_userRebin;
  float bin2mm=par_bin2mm_4shutter_1x1/agregRebin;
  if(par_userRebin!=1) {
    h2Dtempl->Rebin2D(par_userRebin,par_userRebin);
    hPedStat->Rebin2D(par_userRebin,par_userRebin);
    hPedAvr->Rebin2D(par_userRebin,par_userRebin);

    int kb=100/par_userRebin;
    printf("e.g.:new kb=%d  ped=%f, rms=%f\n",kb, hPedAvr->GetBinContent(kb,kb), hPedAvr->GetBinError(kb,kb));
  }

  // compute average ped and pedRMS - needed for some displayes
  double rmsSum=0;
  int nOK=0;
  int nchX=hPedStat->GetNbinsX();
  int nchY=hPedStat->GetNbinsY(); 
  for(int jchX=1; jchX<=nchX; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N
      int mBin=hPedStat->GetBin(jchX, jchY); 
      if (  hPedStat->GetBinContent(mBin) ) continue; // drop masked channels
      float rms=hPedAvr->GetBinError(mBin); 
      rmsSum+=rms;
      nOK++;
    }
  jEve->light[camId].avrPedRms=rmsSum/nOK;
  
  //.... create container for destination image,

  //  for now error of light is identical to RMS of pedestal 
  h2Dlight=(TH2I *) h2Dtempl->Clone();
  h2Dlight->SetNameTitle(coreName+"_light",coreName+" ped subtr w/o threshold");

  // create 2D image space for rotated image
  int diagBin=(int)sqrt(nchX*nchX + nchY*nchY);
  float maxDiagPix=diagBin* agregRebin;
  printf(" h2DlightRot: diagBin=%d  maxDiagPix=%.1f\n",diagBin, maxDiagPix);
  h2DlightRot=new TH2I(coreName+"_lightRot",coreName+" light, rotated axis;u-axis (pix);  v-axis (pix)",2*diagBin,0,2*maxDiagPix,2*diagBin,0,2*maxDiagPix);

  // fill static info in the jEve
  jEve->runId=eve->run();
  jEve->light[camId].hPedStat=hPedStat;
  jEve->light[camId].h2Dlight=h2Dlight; // connect event to current frame
  jEve->light[camId].bin2mm =bin2mm;
  jEve->light[camId].agregRebin =agregRebin;
  jEve->light[camId].h2DlightRot=h2DlightRot; 
  jEve->light[camId].pixOffsetRot=maxDiagPix;
  
 
  printf("CalibMaker()::initDims  camId=%d  agregRebin=%d  bin2mm=%.2f  avrPedRms=%.1f ADU\n", camId,agregRebin,bin2mm, jEve->light[camId].avrPedRms); 
}


//=======================================
//=======================================
void dmtpc::ped::M3CcdCalibMaker::addPixTrace(int xPix, int yPix, int numFrame){
  printf("CcdPedMaker::addPixTrace(xPix=%d yPix=%d)  traceId=%d\n", xPix,yPix,traceV.size());

  
  TString pixName=Form("pixR%dCam%dx%dy%d",jEve->runId,camId,xPix,yPix);

  int jchX=h2Dlight->GetXaxis()->FindBin(xPix);
  int jchY=h2Dlight->GetYaxis()->FindBin(yPix);
  int mBin=hPedStat->GetBin(jchX, jchY);  //   convert 2D to 1D index

  float valErr=hPedAvr->GetBinError(mBin);
  
  PixelTrace trPix(pixName,mBin,valErr,numFrame);
  traceV.push_back(trPix);
  

}



//=======================================
//=======================================
void dmtpc::ped::M3CcdCalibMaker::addPixCorrel(int xPix, int yPix, int dx, int dy){
  printf("CcdPedMaker::addPixCorel(xPix=%d+%d yPix=%d+%d)  itemId=%d\n", xPix,dx,yPix,dy,correlV.size());

  TString sdx=Form("p%d",dx); if (dx<0) sdx=Form("m%d",-dx);
  TString sdy=Form("p%d",dy); if (dy<0) sdy=Form("m%d",-dy);
  TString pixName=Form("corR%dCam%dx%d%sdy%d%s",jEve->runId,camId,xPix,sdx.Data(),yPix,sdy.Data());

  int jchX=h2Dlight->GetXaxis()->FindBin(xPix);
  int jchY=h2Dlight->GetYaxis()->FindBin(yPix);
  int mBin=hPedStat->GetBin(jchX, jchY);  //   convert 2D to 1D index

  int jchX2=h2Dlight->GetXaxis()->FindBin(xPix+dx);
  int jchY2=h2Dlight->GetYaxis()->FindBin(yPix+dy);
  int mBin2=hPedStat->GetBin(jchX2, jchY2);  //   convert 2D to 1D index

  assert(mBin!=mBin2);

  float valErr=hPedAvr->GetBinError(mBin);
  float valErr2=hPedAvr->GetBinError(mBin2);
  
  PixelCorrel corPix(pixName,mBin,valErr,mBin2,valErr2);
  correlV.push_back(corPix);
  

}
