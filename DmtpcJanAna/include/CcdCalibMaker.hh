#ifndef CcdCalibMaker_HH
#define CcdCalibMaker_HH

#include "TObjString.h" 

class TH1F;
class TH2F;
class TH2S;
class TH1;
class TFile;

#include "../DmtpcJanEve/include/M3Event.hh"

namespace dmtpc {
  namespace core  {    class Event; class Dataset;} 
  namespace jana  {
    class CcdClusterMaker;
    class CcdCalibMaker  : public TObject     {
    public:       
      ~CcdCalibMaker();
      CcdCalibMaker();
      // usable calibration
      void load_pedStat(TString fName) ;
      void calibrateImage(const dmtpc::core::Event * eve );
      void initDims(const dmtpc::core::Dataset *ds);
      void saveHisto(TFile *);
      void setUserRebin(int x) {par_userRebin=x;}
      void setQuadId( int iCam, int y) { quadId=iCam; camDaq=y;}
      int  getQuadId() { return quadId;}

      CcdRunData  ccdData;
	
    private:
      int NtotEve;
      TString coreName;
      int  quadId, camDaq;
    
      TH2F *h2Dlight;//  CCD image corrected for pedestal &mask, form 1 camera
      TH2F *h2DlightRot; // CCD image centered & rotated at arbitrary

      void  initHisto(int mxFrame); 
      void clear();
      int xyPix2region(float xPix, float yPix);

      const static int mxH=16;
      TH1 *hA[mxH]; //
      int par_fixRawHistDims; // jan's swithc to rescale axis, add padding, etc
      int par_userRebin; // additional rebinning 

      // input calibration
      TH2S *hPedStat;
      TH2F  *hPedAvr;
      int agregRebin;
      TH2I * h2GarbCol; // garbage collector for re-formated images

      ClassDef(CcdCalibMaker,1);       
    }; 
  }
}
#endif
