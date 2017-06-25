#ifndef CcdPedMaker_HH
#define CcdPedMaker_HH

#include "TObjString.h" 

class TH1F;
class TH2F;
class TH2I;
class TH2S;
class TH1D;
class TH1;
class TFile;
class TCanvas;

namespace dmtpc {
  namespace core  {    class Event; class Dataset;}
  namespace jana  {

    class  PedInfo {
    public:
      PedInfo() {clear();}
      float avr,rms; // pedestal (ADU)
      float peakSum,peakAmpl; // frame count
      int   mBin; // counts 1 to M 
      int   ccdX,ccdY; // pixel coordinates in full scale CCD coordinates
      void clear() { 
	memset(this,0,sizeof(*this)); 
	//printf("peakAmpl clear size=%d %d\n", sizeof(this),sizeof(*this));
      }
      void print() {
	printf(" pedInfo: mBin=%d CCDxy(%d,%d) peak: sum=%.0f ampl=%.0f avr=%.1f rms=%.1f\n",mBin,ccdX,ccdY, peakSum,peakAmpl, avr,rms); 
      }
    };
    
    // status  value for CCD pixels
    enum { isGood=0,bad_isOff=1, bad_lowSum, bad_pedRmsLo, bad_pedRmsHi, bad_pedAvrHi};

    class CcdPedMaker  : public TObject     {
    public:       
      ~CcdPedMaker();
      CcdPedMaker();
      void accumulatePed(const dmtpc::core::Event * eve );

      void initDims(const dmtpc::core::Dataset *dsx , int quadIdx, int camDaqx);

      void saveHisto(TFile *, int flag);
      void ingest_pedSpecBig(TString fName); // back door
      int  computePed();
      void setUserRebin(int x) {par_userRebin=x;}

    private:
      //naming convention: pix=highest recolution CCD channel: bin in histogram
      int NtotEve, runId;
      TString coreName, camSN;
      int nchX, nchY, quadId, camDaq, maxMbin;//,fullNumPix;
      int agregReb;
      TH2I * h2GarbCol; // garbage collector for re-formated images

      void  initHisto(); 
      void  initMask();
      void  setPar(float x, float y) {par_aduLo=x; cut_pedAvrHi=y;}
      void  clear();

      const static int mxH=32;
      TH1 *hA[mxH]; //
      TH2S *hBigPed; // the accumulator of ped spectra
      TH1D *h1Dped; // full range ADU spectrum for 0-th CCD frame
 
      // output calibration
      TH2S *hPedStat;
      TH2F *hPedAvr, *hPedRms, *hPedPeakSum;

      // usable image range
      class ActiveImageLimit1N {
      public:
	int loX,hiX,loY,hiY;
      } actLim;
      
      // pedestal determination parameters
      float par_pedWindowHalf, par_aduLo, cut_pedAvrHi;
      float cut_peakSum, cut_pedRmsL, cut_pedRmsH, cut_pedValH;
      int par_userRebin; // additional rebinning

      void adjustCuts();
      void findPedPeak(TH2S *h2, PedInfo *item, int flag);
      const static int mxPedStat=7;
      TString pdfName[mxPedStat];
      int pdfFract[mxPedStat];

    public:
      TCanvas  *can; // single canvas , servs also as a flag

      ClassDef(CcdPedMaker,1);       
    }; 
  }
}
#endif
