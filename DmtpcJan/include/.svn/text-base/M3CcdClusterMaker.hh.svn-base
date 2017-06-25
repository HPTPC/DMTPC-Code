#ifndef M3CcdClusterMaker_HH
#define M3CcdClusterMaker_HH

#include "TObjString.h" 
#include "TVector2.h"

class TH1F;
class TH2I;
class TH1;
class TFile;
class TCanvas;
class M3JanEvent;
class  M3ImageCluster;
class  M3ImageCell;
#include <stdio.h>//FILE *fd;

namespace dmtpc {
  namespace ped  {
    class M3CcdCalibMaker;
    class M3CcdClusterMaker  : public TObject     {

      enum FrameType {frmHealthy=0, frmStarSpark=-1, frmPiSpark=-2,frmPromise=1};
    public:       
      ~M3CcdClusterMaker(){};
      M3CcdClusterMaker();
      void saveHisto(TFile *);
      void initHisto(); 
      int  findSeedClusters();
      int  hasPiSpark(); 
      int  measureClusters(); // cell-count & diameter
      void bestTrack();
      void display();
      int  isFiducial(float xPix, float yPix);
      void attachSkinToClusters();
      void setDebug(int x){ dbg=x;}
      void setCuts(float x, float y, float z, float u) { 
	cut_cellCountL=x; cut_clustLightL=y; cut_clustDiagL=z; cut_pcaRatioH=u;}
      void setHadrRecCut(float x) {  cut_factHadrRec=x;};
      
      void setDrawings(TCanvas *can, TString outPath);
      void setCSVreport(TString outFname);
      void setJanEve( M3JanEvent * x, int iCam) {jEve=x; camId=iCam;}
      void setNsigThres12(float x,float y) {par_nSigThr1=x;par_nSigThr2=y;}
      void setFiducial(float bandPix); // >0 in, <0 out, =0 off

    private:
      void clear();
      // M3CcdCalibMaker *calibData;
      TString coreName;
      int  camId;
      int dbg;
      TCanvas *can; TString pdfName;
      FILE *fdCSV;
      M3JanEvent * jEve;
      TH2I *h2Dlight, *hPedStat, *h2DlightRot;
    
      int abortFrame;
      TString listPiSpark,listStarSpark;
      int nPromise;
      

      double createCluster(int jchX, int jchY);
      double  addCell(int jxi, int jyi, M3ImageCluster *cluster, int cellType, int clustId); // was M3ImageCell::CellType

      TH2I* hUtil; // helping build clusters

      const static int mxH=32;
      TH1 *hA[mxH]; //

      // thresholds
      float par_nSigThr1, par_nSigThr2; // defines pixel threshold = ped+nSig*rms
      // cuts
      float cut_cellCountL, cut_clustLightL, cut_clustDiagL, cut_pcaRatioH;
     
      TVector2 cut_fiduC;
      double cut_fiduPhi1, cut_fiduPhi2, cut_fiduR2max, par_fiduBandPix;
      
      // hadron recoil cut
      double cut_factHadrRec; // set >0 to activate 

      // bad frames filtering
      float  par_starSparkLight;
      float  par_piSparkLtDens, par_piSparkDiag , par_piSparkNcell;
      ClassDef(M3CcdClusterMaker,1);       
    }; 
  }
}
#endif
