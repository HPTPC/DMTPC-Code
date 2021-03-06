#include <assert.h>
#include "TH2.h"
#include "TH2I.h"

#include "TF1.h"
#include "TText.h"
#include "TStyle.h"

#include "Event.hh"
#include "Image.hh"
#include "CameraInfo.hh"

#include "TCanvas.h"
#include "TLine.h"
#include "TList.h"
#include "TLine.h"
#include "TList.h"
#include "TRandom.h" // for PDF sampling

#include "M3CcdPedMaker.hh" 
#include "M3CcdImageUtil.hh"

ClassImp(dmtpc::ped::M3CcdPedMaker); 

//=======================================
//=======================================
dmtpc::ped::M3CcdPedMaker::M3CcdPedMaker() {
  // printf(" M3CcdPedMaker() cnstr\n"); 
  NtotEve=0;
  coreName="fixMe";
  par_fixRawHistDims=1; // apply raw data corrections if necessary
  h2Dtempl=0;
  nchX=nchY=0;
  camId=-1;
  camSrc=-1;

  // pedestal determination
  par_pedWindowHalf=80; // in ADU

  // ped QA cuts
  cut_peakSum=0.90; // frames, initilized as a fraction, scaled to # input frames in  adjustCuts()
  
  /* cut_pedRmsL=10; // ADU, run 732
  cut_pedRmsH=20; // ADU
  cut_pedValH=2220; // ADU , run 732  */

  // changes for  run 198
  cut_pedRmsL=10;
  cut_pedRmsH=30; // ADU,
  // cut_pedValH=1650; // ADU , run 198

  hAvr=hStat=hRms=0;
  h2GarbCol=0;
  can=0;// default disable plotting
}


//=======================================
//=======================================
dmtpc::ped::M3CcdPedMaker::~M3CcdPedMaker() {
  printf(" ~M3CcdPedMaker() done\n"); 
  if(can==0) return;
  can->Clear();
  
  TText mesg(0.3,0.5,"last page is blank");
  mesg.Draw();
  for(int i=0;i<mxPedStat;i++) { // skip all on input
    if(i==0) continue;
    if(i>=6) continue;
    can->Print( pdfName[i]+")","pdf");

  }
} 



//=======================================
//=======================================
void dmtpc::ped::M3CcdPedMaker::initMask(){
  assert(hStat);
  printf("M3CcdPedMaker::initMask() maxBin=%d\n",maxMbin);
  //.. At first initialize all channels  to bad 
  for(int mBin=1; mBin<=maxMbin; mBin++) {
    hStat->SetBinContent(mBin,bad_isOff);
    // if (mBin>10) continue;
    // printf("mBin=%d, stat=%d\n", mBin, bad_isOff);
  }
  

  //unmask active pixels using  (counting 1 to N)
  // return;
  int nAct=0;
  int mBin;
  for(int jchX=actLim.loX; jchX<=actLim.hiX; jchX++)  // range 1,N
    for(int jchY=actLim.loY; jchY<=actLim.hiX; jchY++) { // range 1,N
      mBin= hStat->GetBin(jchX, jchY); //   convert 2D to 1D index
      hStat->SetBinContent(mBin,isGood);
      nAct++;
      if(nAct==1)  hStat->SetBinContent(mBin,2); // tmp : bottom-left of image
    }
  hStat->SetBinContent(mBin,2); // tmp : upper-right of image
  hA[0]->Fill("active",nAct);
  
}

//=======================================
//=======================================
void dmtpc::ped::M3CcdPedMaker::accumulatePed(const dmtpc::core::Event * eve) {

  if(h2GarbCol) {
    delete h2GarbCol;
    h2GarbCol=0;
  }

  NtotEve++;
  assert( NtotEve>0);  
  assert(eve);
  hA[0]->Fill("FRAME",1.);
  int ncam_in =eve->nccd();
  assert(camSrc<ncam_in);
  if(NtotEve%20==1 ) {
    const dmtpc::core::CameraInfo *info=eve->getImage(camSrc)->getInfo();
    printf(" reading4ped  frame=%d image camSrc=%d camId=%d SN=%s   temp/C=%.1f\n", eve->ev(),camSrc,camId, info->serialNumber.Data(), info->ccdTemp);
  }
 
  TH2I * h2Raw=(TH2I * )eve->ccdData(camSrc); assert(h2Raw);
 
  // handle other data formats from 2015
  if (par_fixRawHistDims) {
    if(h2Raw->GetNbinsX()==764)  
      h2GarbCol=h2Raw=M3CcdImageUtil::convert764toOS( h2Raw,camId);    
  }


  assert(h2Raw->GetNbinsX()==h2Dtempl->GetNbinsX()); // once I forgot to correct 'old' format
  
  // add frame to  big-ped histogram
  for(int jchX=1; jchX<=nchX; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N
      int mBin=h2Raw->GetBin(jchX, jchY); //  pick any 2D histo for this
      if (  hStat->GetBinContent(mBin) ) continue; // drop masked channels
      float rawVal=h2Raw->GetBinContent(mBin);    
      hBigPed->Fill(mBin,rawVal);
    }  
}


//=======================================
//=======================================
int dmtpc::ped::M3CcdPedMaker::computePed(){
  printf("M3CcdPedMaker::evalPed start\n");
  adjustCuts(); // some depend on number of input frames

  int nOK=0, nBad=0;
  for(int jchX=1; jchX<=nchX; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N
      int mBin=hStat->GetBin(jchX, jchY);  //   convert 2D to 1D index
      if (  hStat->GetBinContent(mBin) ) continue; // drop masked channels
      PedInfo item;
      item.ccdX=(jchX-1)*nReb;
      item.ccdY=(jchY-1)*nReb;
      item.mBin=mBin;
      findPedPeak(hBigPed,&item,0);  // flag controls if page is plotted
      bool outBad=nBad%5==0;
      //======  QA  of pedestals =======
      // ---- integral -----
      hA[11]->Fill(item.peakSum); 
      if ( item.peakSum <cut_peakSum ) {
	hA[0]->Fill("lowSum",1.);
	hStat->SetBinContent(mBin,bad_lowSum);
	if (outBad)
	  printf("%d bad ped lowSum iX=%d iY=%d mB=%d\n", nBad,jchX-1,jchY-1, mBin);
	nBad++;
	
	float  xx=gRandom->Uniform(pdfFract[1]);
	if (xx<1.) findPedPeak(hBigPed,&item,1); // generate PDF	  
	continue;
      }
      hA[21]->SetBinContent(mBin,item.peakSum);
 
      //-----  peak amplitude -----
      hA[12]->Fill(item.peakAmpl); 
      hA[22]->SetBinContent(mBin,item.peakAmpl);

      //----- ped width -----
      hA[13]->Fill(item.rms);
      if ( item.rms <cut_pedRmsL ) {
	hA[0]->Fill("pedRmsLo",1.);
	hStat->SetBinContent(mBin,bad_pedRmsLo);
	if (outBad)
	  printf("%d bad=low ped RMS iX=%d iY=%d mB=%d \n", nBad,jchX-1,jchY-1, mBin); 
	nBad++;
	float  xx=gRandom->Uniform(pdfFract[2]);
	if (xx<1.) findPedPeak(hBigPed,&item,2); // generate PDF	  
	continue;
      }

      if ( item.rms >cut_pedRmsH ) {
	hA[0]->Fill("pedRmsHi",1.);
	hStat->SetBinContent(mBin,bad_pedRmsHi);
	if (outBad)
	  printf("%d bad=high ped RMS iX=%d iY=%d mB=%d \n", nBad,jchX-1,jchY-1, mBin); 
	nBad++;
	float  xx=gRandom->Uniform(pdfFract[3]);
	if (xx<1.) findPedPeak(hBigPed,&item,3); // generate PDF	  
	continue;
      }

      hRms->SetBinContent(mBin,item.rms);

      //----- ped value -----
      hA[14]->Fill(item.avr);
      if ( item.avr > cut_pedAvrHi ) {
	hA[0]->Fill("pedAvrHi",1.);
	hStat->SetBinContent(mBin,bad_pedAvrHi);
	if (outBad)
	  printf("%d bad-hi ped AVR iX=%d iY=%d mB=%d \n", nBad,jchX-1,jchY-1, mBin); 
	nBad++;
	float  xx=gRandom->Uniform(pdfFract[4]);
	if (xx<1.) findPedPeak(hBigPed,&item,4); // generate PDF	  
	continue;
      }
      hAvr->SetBinContent(mBin,item.avr);
      hAvr->SetBinError(mBin,item.rms);

    // now pixel is declared as good 
    hA[0]->Fill("good",1.);
    float  xx=gRandom->Uniform(pdfFract[5]);
    if (xx<1.) findPedPeak(hBigPed,&item,5); // generate PDF	      
    nOK++;
    }
  return nOK;
}


//=======================================
//=======================================
void dmtpc::ped::M3CcdPedMaker::findPedPeak(TH2S *h2, PedInfo *item, int flag){
  // if ( flag )printf("M3CcdPedMaker::findPedPeak mBin=%d\n",mBin);
  
  int mBin=item->mBin;
  // insolite k-th slice and find pedestal

  TH1D*  h1=h2->ProjectionY("pjADU",mBin,mBin);

  // limit range of histo to ped-peak
  int peakBin=h1->GetMaximumBin();
  float peakAbscia= h1->GetBinCenter(peakBin);

  // bracket spectrum around peak
  h1->SetAxisRange(peakAbscia- par_pedWindowHalf,  peakAbscia +par_pedWindowHalf);

  item->peakAmpl=h1->GetBinContent(peakBin);
  item->avr=h1->GetMean();
  item->rms=h1->GetRMS();
  item->peakSum=h1->Integral();
  
  if ( flag <=0  || can==0) return ; // no plotting
  can->Clear();

  // dump 
  // item.print(); // tmp

  gStyle->SetOptStat(1001100);
  can->Divide(1,2);
  can->cd(1);
  h2->SetAxisRange(mBin-8,mBin+8);  // show more surrounding channels for fun
  h2->Draw("colz"); 

  // mark the mBin you selected
  TLine *ln=new TLine(mBin-0.45,0,mBin-0.45,3000); ln->SetLineColor(kMagenta);
  ln->Draw();

  ln=new TLine(mBin+0.45,0,mBin+0.45,3000); ln->SetLineColor(kMagenta);
  ln->Draw();

  // draw ADU spectrum for selected bin
  can->cd(2);
  h1->SetFillColor(kYellow);

  h1->Draw();
  float par_nSigThr=5;
  float delAdu=par_nSigThr*item->rms;
  h1->SetTitle(Form("CCDxy(%d,%d) pedestal, mBin=%d ,  thres=%.0f ADU",item->ccdX,item->ccdY,mBin,item->avr+delAdu));

  ln=new TLine(item->avr,0,item->avr,1000); ln->SetLineColor(kBlue);  ln->Draw();
ln->SetLineWidth(2);

  ln=new TLine(item->avr+delAdu,-2,item->avr+delAdu,item->peakAmpl/2); ln->SetLineColor(kBlue);  ln->Draw(); ln->SetLineWidth(4);

  printf(" found pedAvr=%.1f  rms=%.1f  ampl=%.0f sum=%0f mBin=%d flag=%d\n", item->avr,item->rms, item->peakAmpl,item->peakSum,mBin,flag);
  
  can->Print(pdfName[flag],"pdf");
  
  return;
  // ---- drop the code below ??? ----
  // draw also threshold based on gauss fit
  h1->Fit("gaus");

  TF1* ff=h1->GetFunction("gaus");  assert(ff);
  float avrG=ff->GetParameter(1);
  float sigG=ff->GetParameter(2);
  printf(" gaus sig=%f\n",sigG);
  float delAduG=par_nSigThr*sigG;
  ln=new TLine(item->avr,0,item->avr,1000); ln->SetLineColor(kRed);  ln->Draw();
  ln->SetLineStyle(2);

  ln=new TLine(avrG+delAduG,-2,avrG+delAduG,item->peakAmpl/2); ln->SetLineColor(kRed);  ln->Draw();  ln->SetLineStyle(2);




}


