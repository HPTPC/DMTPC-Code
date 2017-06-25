#include <assert.h>
#include "TH2.h"
#include "TH2I.h"

#include "TF1.h"
#include "TFile.h"
#include "TString.h"
#include "TText.h"
#include "TStyle.h"
#include "TProfile.h"
#include "TTimeStamp.h"

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

#include "CcdCalibMaker.hh" 
#include "M3JanUtil.hh"

ClassImp(dmtpc::jana::CcdCalibMaker); 

//=======================================
//=======================================
dmtpc::jana::CcdCalibMaker::CcdCalibMaker() {
  // printf(" CcdCalibMaker() cnstr\n"); 
  NtotEve=0;
  quadId=-1;
  camDaq=-1;
  coreName="fixMe";
  par_fixRawHistDims=1; // apply raw data corrections if necessary

  hPedAvr=0;
  hPedStat=0;
  h2Dlight=h2DlightRot=0;
  setUserRebin(1);
  agregRebin=0;
  h2GarbCol=0;
}


//=======================================
//=======================================
dmtpc::jana::CcdCalibMaker::~CcdCalibMaker() {
  printf(" ~CcdCalibMaker() done, processed %d frames\n",NtotEve); 
}

//=======================================
//=======================================
int dmtpc::jana::CcdCalibMaker::xyPix2region(float xPix, float yPix){
  assert(quadId==3); // other not implmeneted
  float sum=xPix+yPix;
  if( fabs(xPix-yPix)<6) return -2;
  if( sum <1800) {
    if ( xPix>yPix) return 0;
    else return 1;
  }

  if( sum >3000) {
    if ( xPix>yPix) return 2;
    else return 3;
  }
  return -1;
}

//=======================================
//=======================================
void dmtpc::jana::CcdCalibMaker::clear() {
  ccdData.frameId=-1;

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
  // printf(" CcdCalibMaker::clear() done\n"); 
}

//=======================================
//=======================================
void dmtpc::jana::CcdCalibMaker::calibrateImage(const dmtpc::core::Event * eve) {

  assert(hPedStat);// to verify calibration was read-in

  assert(eve);
  clear();
  NtotEve++;
  
  int frameId=eve->ev(); 
  int runId=eve->run(); 

  ccdData.frameId=frameId;

  h2Dlight->SetNameTitle(coreName+Form("_fr%d_light",frameId),coreName+Form(" R%d  fr%d, Light (ADU), nBinX=%d",runId,frameId,h2Dlight->GetNbinsX()));  
 
  int ncam_in =eve->nccd();  assert(camDaq<ncam_in);

  if(frameId%10==0 || NtotEve==1) {
    const dmtpc::core::CameraInfo *info=eve->getImage(camDaq)->getInfo();
    printf(" calibrateImage reading4ped  frame=%d image camDaq=%d quadId=%d SN=%s   temp/C=%.1f\n", eve->ev(),camDaq,quadId, info->serialNumber.Data(), info->ccdTemp);
  }
  
  TH2I * h2Raw=(TH2I * )eve->ccdData(camDaq); assert(h2Raw);

  if(h2Raw->GetNbinsX()==764)  h2GarbCol=h2Raw=M3JanUtil::convert764toOS( h2Raw,quadId);  

  //....  further rebin image if requested
  if(par_userRebin!=1) {
    h2Raw->Rebin2D(par_userRebin,par_userRebin);
  }

  // basic sanity check
  assert(h2Raw->GetNbinsX() == hPedStat->GetNbinsX());
  assert(h2Raw->GetNbinsY() == hPedStat->GetNbinsY());
  assert(h2Raw->GetXaxis()->GetXmax()==hPedStat->GetXaxis()->GetXmax());

  if(0) {
    h2Raw->Print();
    hPedAvr->Print();
    h2Dlight->Print();
    printf(" xMax raw=%f  avr=%f\n", h2Raw->GetXaxis()->GetXmax(),hPedAvr->GetXaxis()->GetXmax());
  }

  int kb=100/par_userRebin;
  if(NtotEve==1)
    printf("  e.g. uReb data :kb=%d  raw=%.1f, ped=%.1f rms=%f  light=%.1f\n",kb, h2Raw->GetBinContent(kb,kb),hPedAvr->GetBinContent(kb,kb), hPedAvr->GetBinError(kb,kb), h2Dlight->GetBinContent(kb,kb));

  // printf(" calib sumA data=%.3e  ped=%.3e  light=%.3e \n", h2Raw->Integral(),   hPedAvr->Integral(), h2Dlight->Integral());

  //.... subtract pedestals  & propagate error of pedestals
  // ...  and clear masked pixels
  int nchX=hPedStat->GetNbinsX();
  int nchY=hPedStat->GetNbinsY();
  float totPix=nchX*nchY;
  for(int jchX=1; jchX<=nchX; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N
      if (  hPedStat->GetBinContent(jchX,jchY) ) continue;
      float val=h2Raw->GetBinContent(jchX,jchY); 
      float ped=hPedAvr->GetBinContent(jchX,jchY);
      float light=val-ped;
      float ltErr=hPedAvr->GetBinError(jchX,jchY);
      h2Dlight->SetBinContent(jchX,jchY,light);
      h2Dlight->SetBinError(jchX,jchY,ltErr); // set err to pedRMS
      hA[1]->Fill(frameId,light);

      //... compute common ped
      float xPix= h2Raw->GetXaxis()->GetBinCenter(jchX);
      float yPix= h2Raw->GetYaxis()->GetBinCenter(jchY);
      int iReg=xyPix2region(xPix,yPix);
      
      if(iReg>=0) ((TH2F*) hA[9])->Fill(xPix,yPix,light);

      if(iReg>=0) {
	hA[10+iReg]->Fill(ccdData.frameId,light);
      }

      if(iReg==0 && ccdData.frameId==0) hA[14]->Fill(light);
    }// end of X,Y

   double totLight=h2Dlight->Integral();
   hA[2]->Fill(frameId,totLight/totPix);
   hA[3]->Fill(totLight/totPix);
  // printf(" calib sumB data=%.3e  ped=%.3e  light=%.3e fr=%d \n", h2Raw->Integral(),   hPedAvr->Integral(), h2Dlight->Integral(), ccdData.frameId);

}


//=======================================
//=======================================
void dmtpc::jana::CcdCalibMaker::saveHisto(TFile *fd) {

  printf(" %s histos  are written  to '%s'" ,coreName.Data(),fd->GetName());
  for(int i=0;i<mxH;i++) {
    if(hA[i]) hA[i]->Write();
  }

  printf(" , save Ok\n");

}



//=======================================
//=======================================
void dmtpc::jana::CcdCalibMaker::load_pedStat(TString fName) {

  TFile *fd=new TFile(fName);
  printf("CcdCalibMaker::load_pedStat opening %s for quadId=%d\n",fName.Data(), quadId);
  assert(fd->IsOpen());

  hPedStat=(TH2S*) fd->Get(Form("quad%d_pedStat",quadId)); assert( hPedStat);
  hPedAvr =(TH2F*) fd->Get(Form("quad%d_pedAvr",quadId)); assert( hPedAvr);

  int kb=100;
  printf("e.g.:kb=%d  ped=%f, rms=%f\n",kb, hPedAvr->GetBinContent(kb,kb), hPedAvr->GetBinError(kb,kb));

  printf(" Read-in calibration  from '%s'  \n" ,fd->GetName());
}


//=======================================
//=======================================
void dmtpc::jana::CcdCalibMaker::initHisto(int mxFrame) {
  assert(hPedStat); //  an event was already used to define dimensions

  memset(hA,0,sizeof(hA)); // input spectra, unweighted
  TH1* h;


  hA[1]=h=new TProfile(coreName+"_binPedRes","profile of pedestal residua , all input; frame Id; light=raw-ped (ADU)",mxFrame,-0.5,mxFrame-0.5, -30,30);
  hA[2]=h=new TH2D(coreName+"_frmLtRes","frame avr pix light/frame , all input; frame Id; (raw-ped)/nPix ( ADU)",mxFrame,-0.5,mxFrame-0.5,50, -1.1,1.1);

  hA[3]=h=new TH1D(coreName+"_frmLtSum","frame avr pix /frame, all input; (raw-ped)/nPix ( ADU)",100, -3,3);

  // free; 4-6

 // 1D   pedestal as used
 int aduLo=1900*par_userRebin*par_userRebin, aduHi=2900*par_userRebin*par_userRebin;
 TString coreNameR=Form("quad%dR%dcal",quadId,ccdData.runId);

 hA[7]=h=new TH1D(coreNameR+"_UpedAvr",coreNameR+Form(" profile of pedestal value, userRebin=%d; pedestal value(ADU); pixel count",par_userRebin), 200,aduLo,aduHi);
 hA[8]=h=new TH1D(coreNameR+"_UpedRms",coreNameR+Form(" profile of pedestal RMS, userRebin=%d; pedestal RMS (ADU); pixel count",par_userRebin), 100,0,40*par_userRebin);


 // monitor ADU sum for valid pixles
 hA[9]=(TH2F *) hPedAvr->Clone();
 hA[9]->SetNameTitle(coreNameR+"_sumLg",coreNameR+" light sum, all frames, good pixels");
 hA[9]->Reset("M");
 hA[9]->SetMinimum(0);


  //---- output: common pedestals, 4 areas of interest
 for(int jt=0;jt <4; jt++) {
   float yy=10;
   hA[10+jt]=h=new TProfile(coreName+Form("_comPed%c",'A'+jt),coreName+Form(" ped residua , good pixels, region %c; frame Id; light=raw-ped (ADU) ",'A'+jt),mxFrame,-0.5,mxFrame-0.5, -yy,yy);
 }
  
 hA[14]=h=new TH1D(coreName+"_pedRes1",coreName+ " Ped res, frame=0, region=A; light = raw-ped(ADU); pixel count", 200,-50,50);
 

}

//=======================================
//=======================================
void dmtpc::jana::CcdCalibMaker::initDims(const dmtpc::core::Dataset *ds ) {
  assert( hPedStat);
  int nExpo=ds->nevents();
  printf(" CcdCalibMaker()::initDims  quadId=%d  camDaq=%d  \n", quadId,camDaq);
  coreName=Form("quad%dcal",quadId);

  const dmtpc::core::Event * eve=ds->event();
  int ncam_in =eve->nccd();
  assert(camDaq>=0); 
  assert(camDaq<ncam_in);

  const TTimeStamp * eveTime=eve->getTimestamp();
  const dmtpc::core::Image *image=eve->getImage(camDaq); assert(image);
  const dmtpc::core::CameraInfo *info=image->getInfo(); assert(info);
  
  M3JanUtil::verifyQuadId(quadId,info->serialNumber);

  ccdData.quadId=quadId;
  ccdData.camDaq=camDaq;
  ccdData.camSN=info->serialNumber;
  ccdData.runId=eve->run();
  ccdData.mxFrame= nExpo;
  ccdData.timeStamp=*(eve->getTimestamp());  // returned is just a pointer
  ccdData.exposureTime=info->exposureTime;


  printf(" First frame=%d image camDaq=%d  SN=%s   temp/C=%.1f recorded at:\n", eve->ev() ,camDaq, info->serialNumber.Data(), info->ccdTemp);
  eveTime->Print();
  printf(" timeStamp sec=%d , exposure length =%d (msec)\n", eveTime->GetSec(),info->exposureTime);

 
  TH2I * h2raw=(TH2I * )eve->ccdData(camDaq); assert(h2raw);

  if(h2raw->GetNbinsX()==764)  h2GarbCol=h2raw=M3JanUtil::convert764toOS( h2raw,quadId);

  //....  further rebin image if requested
  if(par_userRebin!=1) {
    h2raw->Rebin2D(par_userRebin,par_userRebin);
    hPedStat->Rebin2D(par_userRebin,par_userRebin);
    hPedAvr->Rebin2D(par_userRebin,par_userRebin); 
  }
  
  const int nbinsx =  h2raw->GetNbinsX();
  const int nbinsy =  h2raw->GetNbinsY();
  const double xmax = h2raw->GetXaxis()->GetXmax();
  int  daqRebin=(int)( xmax/nbinsx+0.1);
  printf("Frame header info: ncam_in=%d,nbinsx=%d,nbinsy=%d daqRebin=%d\n", ncam_in, nbinsx,nbinsy,daqRebin);

 
  initHisto(nExpo); //<<-------------------  INIT   HISTOS ---------

  // I'm not dealing with a non-square image format
  assert(nbinsx==nbinsy);
  
  assert(hPedStat->GetNbinsX()==nbinsx);
  assert(hPedStat->GetNbinsY()==nbinsy);
  
  printf(" CcdCalibMaker()::initDims userRebin(%d)   \n",par_userRebin);
  
  agregRebin=daqRebin;
  ccdData.agregRebin=agregRebin;


  int kb=100/par_userRebin;
  printf("e.g.:new kbkb=%d  ped=%.1f, rms=%.1f adu\n",kb, hPedAvr->GetBinContent(kb,kb), hPedAvr->GetBinError(kb,kb));
  
  // compute average ped and pedRMS - needed for some eve-displays
  double rmsSum=0;
  int nOK=0;
  int nchX=hPedStat->GetNbinsX();
  int nchY=hPedStat->GetNbinsY(); 
  for(int jchX=1; jchX<=nchX; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N
      int mBin=hPedStat->GetBin(jchX, jchY); 
      if (  hPedStat->GetBinContent(mBin) ) continue; // drop masked channels
      float ped=hPedAvr->GetBinContent(mBin); 
      float rms=hPedAvr->GetBinError(mBin); 
      hA[7]->Fill(ped);
      hA[8]->Fill(rms);
      rmsSum+=rms;
      nOK++;
    }

   ccdData.avrPedRms=rmsSum/nOK;
  
  //.... create container for destination image,
  //  for now assume error of light is identical to RMS of pedestal 
  h2Dlight=(TH2F *) hPedAvr->Clone();
  h2Dlight->SetNameTitle(coreName+"_light",coreName+" ped subtr w/o threshold");

  // create 2D image space for rotated image
  int diagBin=(int)sqrt(nchX*nchX + nchY*nchY);
  float maxDiagPix=diagBin* agregRebin;
  printf(" create h2DlightRot: diagBin=%d  maxDiagPix=%.1f\n",diagBin, maxDiagPix);
  h2DlightRot=new TH2F(coreName+"_lightRot",coreName+" light, rotated axis;u-axis (pix);  v-axis (pix)",2*diagBin,0,2*maxDiagPix,2*diagBin,0,2*maxDiagPix);


  // fill static info in the m3Eve
  ccdData.hPedStat=hPedStat;
  ccdData.hLight=h2Dlight; // connect event to current frame

 
  printf("CalibMaker()::initDims  quadId=%d  agregRebin=%d    avrPedRms=%.1f ADU\n", quadId,agregRebin,ccdData.avrPedRms); 

     CcdRunData &x=ccdData;
      if(x.agregRebin>0) {
	printf("  quadId=%d camSrc=%d SN=%s pedRms=%.1f \n",x.quadId,x.camDaq ,x.camSN.Data(),x.avrPedRms);
    }
}




