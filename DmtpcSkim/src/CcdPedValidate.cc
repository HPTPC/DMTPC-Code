//This code generates random numbers to make 'fake' events. serach for 'Gaus' 
//to go to the distribution settings. there are 2 places where 'Gaus' is called
//after changing the distribution, do -make in DmtpcSkim folder. 
//then run $DMTPC_HOME/ccd_characterisation/rdRaw2PedValidate.C
#include <assert.h>
#include "TH2.h"
#include "TH2I.h"
#include "TText.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TStyle.h"
#include "TList.h"

#include "TRandom3.h"

#include "CcdPedMaker.hh" 


//=======================================
//=======================================
  void dmtpc::skim::CcdPedMaker::initMaskValidate(){
  assert(hPedStat);
  printf("CcdPedMaker::initMask() maxBin=%d\n",maxMbin);
  //.. At first initialize all channels  to good as all channels are active
  for(int mBin=1; mBin<=maxMbin; mBin++) {
    hPedStat->SetBinContent(mBin,isGood);
    // if (mBin>10) continue;
    // printf("mBin=%d, stat=%d\n", mBin, bad_isOff);
  }
  

  //unmask active pixels using  (counting 1 to N)
  // return;
  int nAct=0;
  int mBin;
  for(int jchX=actLim.loX; jchX<=actLim.hiX; jchX++)  // range 1,N
    for(int jchY=actLim.loY; jchY<=actLim.hiX; jchY++) { // range 1,N
      mBin= hPedStat->GetBin(jchX, jchY); //   convert 2D to 1D index
      hPedStat->SetBinContent(mBin,isGood);
      nAct++;
      if(nAct==1)  hPedStat->SetBinContent(mBin,2); // tmp : bottom-left of image
    }
  hPedStat->SetBinContent(mBin,2); // tmp : upper-right of image
  hA[0]->Fill("active",nAct);
  
}

//=========================================
//=========================================
  void dmtpc::skim::CcdPedMaker::accumulatePedValidate(Int_t nbins) {
  NtotEve++;
  assert( NtotEve>0);  
  hA[0]->Fill("FRAME",1.);
  TRandom3 ran(0);
 
  
  // add frame to  big-ped histogram
  for(int jchX=1; jchX<=nchX; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N
      int mBin=hPedStat->GetBin(jchX, jchY); //  pick any 2D histo for this
      if (  hPedStat->GetBinContent(mBin) ) continue; // drop masked channels
      hBigPed->Fill(mBin,(Short_t)ran.Gaus(2500,20));
    }
}

//==========================================
//==========================================
  int dmtpc::skim::CcdPedMaker::computePedValidate(){
  printf("CcdPedMaker::evalPed start\n");
  adjustCuts(); // some depend on number of input frames

  int nOK=0, nBad=0;
  for(int jchX=1; jchX<=nchX; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N
      int mBin=hPedStat->GetBin(jchX, jchY);  //   convert 2D to 1D index
      if (  hPedStat->GetBinContent(mBin) ) continue; // drop masked channels
      PedInfo item;
      item.ccdX=(jchX-1)*agregReb;
      item.ccdY=(jchY-1)*agregReb;
      item.mBin=mBin;
      findPedPeak(hBigPed,&item,0);  // flag controls if page is plotted
      bool outBad=nBad%5==0;
      //======  QA  of pedestals =======
      // ---- integral -----
      hA[11]->Fill(item.peakSum); 
      if ( item.peakSum <cut_peakSum ) {
	hA[0]->Fill("lowSum",1.);
	hPedStat->SetBinContent(mBin,bad_lowSum);
	if (outBad)
	  printf("%d bad ped lowSum iX=%d iY=%d mB=%d\n", nBad,jchX-1,jchY-1, mBin);
	nBad++;
	
	float  xx=gRandom->Uniform(pdfFract[1]);
	if (xx<1.) findPedPeak(hBigPed,&item,1); // generate PDF	  
	continue;
      }
      hPedPeakSum->SetBinContent(mBin,item.peakSum);
 
      //-----  peak amplitude -----
      hA[12]->Fill(item.peakAmpl); 

      //----- ped width -----
      hA[13]->Fill(item.rms);
      if ( item.rms <cut_pedRmsL ) {
	hA[0]->Fill("pedRmsLo",1.);
	hPedStat->SetBinContent(mBin,bad_pedRmsLo);
	if (outBad)
	  printf("%d bad=low ped RMS iX=%d iY=%d mB=%d \n", nBad,jchX-1,jchY-1, mBin); 
	nBad++;
	float  xx=gRandom->Uniform(pdfFract[2]);
	if (xx<1.) findPedPeak(hBigPed,&item,2); // generate PDF	  
	continue;
      }

      if ( item.rms >cut_pedRmsH ) {
	hA[0]->Fill("pedRmsHi",1.);
	hPedStat->SetBinContent(mBin,bad_pedRmsHi);
	if (outBad)
	  printf("%d bad=high ped RMS iX=%d iY=%d mB=%d \n", nBad,jchX-1,jchY-1, mBin); 
	nBad++;
	float  xx=gRandom->Uniform(pdfFract[3]);
	if (xx<1.) findPedPeak(hBigPed,&item,3); // generate PDF	  
	continue;
      }

      //----- ped value -----
      hA[14]->Fill(item.avr);
      if ( item.avr > cut_pedAvrHi ) {
	hA[0]->Fill("pedAvrHi",1.);
	hPedStat->SetBinContent(mBin,bad_pedAvrHi);
	if (outBad)
	  printf("%d bad-hi ped AVR iX=%d iY=%d mB=%d \n", nBad,jchX-1,jchY-1, mBin); 
	nBad++;
	float  xx=gRandom->Uniform(pdfFract[4]);
	if (xx<1.) findPedPeak(hBigPed,&item,4); // generate PDF	  
	continue;
      }
      hPedAvr->SetBinContent(mBin,item.avr);
      hPedAvr->SetBinError(mBin,item.rms);
      hPedRms->SetBinContent(mBin,item.rms);

    // now pixel is declared as good 
    hA[0]->Fill("good",1.);
    float  xx=gRandom->Uniform(pdfFract[5]);
    if (xx<1.) findPedPeak(hBigPed,&item,5); // generate PDF	      
    nOK++;
    }
  return nOK;
}

 void dmtpc::skim::CcdPedMaker::initDimsValidate(Int_t nbins) {
     TRandom3 ran(0);
 
   runId=10000;
   TString coreName="Validate";
   //dimension hard coded
   const Int_t dim = 3084;
   Int_t xmin=0,
         ymin=0;
   Int_t xmax=dim,
         ymax=dim;
   int nReb=(int)((float)dim/(float)nbins+0.1);
 
  //--------------- save values needed elsewere ------------
    nchX=nbins;
    nchY=nbins;
    agregReb=nReb;
   
// find  gross range of pedestal, including masked channel for now
h1Dped=new TH1D("aduLo","raw ADU spectrum lower end, all input; ADU",1000,0,50000);
  for(int jchX=0; jchX<nchX; jchX++)
    for(int jchY=0; jchY<nchY; jchY++) { // range 1,N
          h1Dped->Fill(ran.Gaus(2500,20));
        }
int peakBin=h1Dped->GetMaximumBin();
  float peakAbscia= h1Dped->GetBinCenter(peakBin);
    setPar(peakAbscia-150*sqrt(agregReb),peakAbscia+350); // aduLo, pedAvrHi

      // pedestal determination
par_pedWindowHalf=60*sqrt(agregReb); // in ADU

  // define sensitive pixel limits, note Hbook convention for the output
if (nchX==1542) { // 2x2 bining
    actLim.loX=actLim.loY=11;
    actLim.hiX=actLim.hiY=1538;
  } else if (nchX==1028) { // 3x3 bining
      actLim.loX=actLim.loY=7;
      actLim.hiX=actLim.hiY=1026;
  } else if (nchX==771) { // 4x4 bining
    actLim.loX=actLim.loY=6;
    actLim.hiX=actLim.hiY=769;
  } else if (nchX==385) { // 8x8 bining
    actLim.loX=actLim.loY=4;
    actLim.hiX=actLim.hiY=384;
  } else {
    assert(3==98); // implement other binnings
  }

  TH2S *h2raw=new TH2S("h2raw","RAW",nbins,0,dim,nbins,0,dim);
  maxMbin=h2raw->GetBin(nchX, nchY); // the largest 1D bin index of interest
  delete h2raw;
  
  // create containers for output calibration
  hPedStat=new TH2S("pedStat","pedestal status",nbins,xmin,xmax,nbins,ymin,ymax);
  hPedAvr=new TH2F("pedAvr","pedestal AVR err=RMS",nbins,xmin,xmax,nbins,ymin,ymax);
  
  hPedRms=new TH2F("pedRms","pedestal RMS",nbins,xmin,xmax,nbins,ymin,ymax);  hPedRms->SetMinimum(7);

  // for convenience this monitoring hist is created here as well
  hPedPeakSum=new TH2F("pedPeakSum","ped peak ampl, accepted so far, ",nbins,xmin,xmax,nbins,ymin,ymax);

  printf( "CcdPedMaker()::initDims end, pedWindowHalf=%.1f (ADU) , CUTS:  peakSumFrac=%.2f,  rms=[%.1f,%.1f] ADU, valH=%.1f ADU,  userRbin=%d\n", par_pedWindowHalf,cut_peakSum, cut_pedRmsL, cut_pedRmsH, cut_pedValH,par_userRebin);
  initHisto();
  initMask();
 }
 void  dmtpc::skim::CcdPedMaker::initHistoValidate() {
   memset(hA,0,sizeof(hA)); // input spectra, unweighted
   TH1* h;
   int mxFrames=600; // no impact on performance
   int nCase=mxPedStat;
   hA[0]=h=new TH1D("case","QA of pedestals; ; channel count",nCase,0,nCase);  h->GetXaxis()->SetTitleOffset(0.4);  h->GetXaxis()->SetLabelSize(0.06);  h->GetXaxis()->SetTitleSize(0.05); h->SetMinimum(0.8);  h->SetLineColor(kBlue); h->SetFillColor(42);h->SetLineWidth(2);  h->SetMarkerSize(2);//<-- large text // no-W
     
   TText mesg(0.3,0.5,"fixMe17");
   TString nameA[mxPedStat]={"active","lowSum","pedRmsLo","pedRmsHi","pedAvrHi","good","FRAME"};
 
   // prescale random rejection factor for different types of error
 int  xpdfFract[mxPedStat]={ -1, 1, 1,1,100,10000,-1};
   for(int i=0;i<mxPedStat;i++) {
     hA[0]->Fill(nameA[i],0.);
     if(i==0) continue;
     if(i>=6) continue;
     if(can==0) continue; // no plotting of individual pixels
     can->Clear();
     pdfName[i]=Form("pedQA_%s.pdf",nameA[i].Data());
     //  open output PSDF files
     pdfFract[i]=xpdfFract[i];
     TString aa=Form("CCD pixels type=%s, selection: 1/%d",nameA[i].Data(),pdfFract[i]);
     printf("aa=%s=\n",aa.Data());
     mesg.SetText(0.05,0.5,aa);
     mesg.Draw();
     can->Print( pdfName[i]+"(","pdf");
   }
   int aduHi=par_aduLo+400*sqrt(agregReb), nAduBins=60; // defines range of pedestals spectra considered as reasonable
   // histos 1...9 
   hA[1]=0;
   hA[9]=0;
   // 1D histos 10 ..19 :  QA of pedestal determination
   hA[10]=0;
   hA[11]=h=new TH1D("peakSum_cut","profile of ped peak sum, accepted so far; frame count; pixel count", mxFrames,-0.5 ,mxFrames-0.5);
   hA[12]=h=new TH1D("peakAmpl","profile of ped peak amplitude, accepted so far; frame count; pixel count", mxFrames/2,-0.5,mxFrames/2.-0.5);
   hA[13]=h=new TH1D("pedRms_cut","profile of pedestal width, accepted so far; pedestal RMS (ADU); pixel count", 100,0,100);
   { TList *tl=h->GetListOfFunctions(); assert(tl);
     TLine *ln=new TLine(cut_pedRmsH, 0, cut_pedRmsH, 1e6);    tl->Add(ln);
     ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
     ln=new TLine(cut_pedRmsL, 0, cut_pedRmsL, 1e6);    tl->Add(ln);
     ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
   }
 hA[14]=h=new TH1D("pedVal_cut","profile of pedestal average, accepted so far; pedestal value(ADU); pixel count", 100,par_aduLo,aduHi);
 { TList *tl=h->GetListOfFunctions(); assert(tl);
   TLine *ln=new TLine( cut_pedAvrHi, 0, cut_pedAvrHi, 1e6);    tl->Add(ln);
   ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
 }
 // 2D histos 20 ..29 :  QA of pedestal determination
 hA[20]=hPedStat;
 h=hA[21]=hPedAvr;
 hA[22]=hPedRms;
 hA[23]=hPedPeakSum;
 h->SetMinimum(par_aduLo);
 h->SetMaximum(aduHi);

 //TH1S : histograms with one short per channel. Maximum bin content = 32767
 hBigPed=new TH2S("bigPed",Form("pedestal spectra ; unified mBin maps to (iX,iY*NX) ; ADU"), maxMbin,0.5,maxMbin+0.5,nAduBins,par_aduLo,aduHi);
 printf("pedMaker::initHisto  works w/ %d pixels, aduLo=%.0f pedAvrHi=%.0f\n", maxMbin,par_aduLo, cut_pedAvrHi);

 }
