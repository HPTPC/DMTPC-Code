#ifndef M3CcdCalibMaker_HH
#define M3CcdCalibMaker_HH

#include "TObjString.h" 

class TH1F;
class TH2I;
class TH1;
class TFile;
class M3JanEvent;

#include "PixelTrace.hh" 
#include "PixelCorrel.hh" 


namespace dmtpc {
  namespace core  {    class Event; class Dataset;} // ?
  namespace ped  {
    class M3CcdClusterMaker;
    class M3CcdCalibMaker  : public TObject     {
    public:       
      ~M3CcdCalibMaker();
      M3CcdCalibMaker();
      // usable calibration
      void load_pedStat(TString fName) ;
      void calibrateImage(const dmtpc::core::Event * eve );
      void initDims(const dmtpc::core::Dataset *ds);
      void addPixTrace(int xPix, int yPix, int numFrame);
      void addPixCorrel(int xPix, int yPix, int dx, int dy); // mind rebinning
      void saveHisto(TFile *);
      void setUserRebin(int x) {par_userRebin=x;}
      void setJanEve( M3JanEvent * x , int iCam, int y) {jEve=x; camId=iCam; camSrc=y;}
	
    private:
      int NtotEve;
      TString coreName;
      int  camId, camSrc;
      M3JanEvent * jEve;
      TH2I *h2Dlight;//  CCD image corrected for pedestal &mask, form 1 camera
      TH2I *h2DlightRot; // CCD image centered & rotated at arbitrary
      
      std::vector <PixelTrace> traceV;
      std::vector <PixelCorrel> correlV;

      TH2I * h2GarbCol; // garbage collector  for event images
      void  initHisto(); 
      void clear();

      const static int mxH=16;
      TH1 *hA[mxH]; //
      float par_bin2mm_4shutter_1x1; // never changes
      int par_fixRawHistDims; // jan's swithc to rescale axis, add padding, etc
      int par_userRebin; // additional rebinning 

      // input calibration
      TH2I *hPedAvr, *hPedStat;
      TH2I *h2Dtempl; // templet of raw input spectrum

      ClassDef(M3CcdCalibMaker,1);       
    }; 
  }
}
#endif
