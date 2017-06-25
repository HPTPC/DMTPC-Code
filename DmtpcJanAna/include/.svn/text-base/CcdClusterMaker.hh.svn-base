#ifndef CcdClusterMaker_HH
#define CcdClusterMaker_HH

#include "TObjString.h" 
#include "TVector2.h"

class TH1F;
class TH2I;
class TH1;
class TFile;
class TCanvas;
class M3Event;
class  CcdCluster;
class  CcdCell;
#include <stdio.h>//FILE *fd;
#include "../DmtpcJanEve/include/M3Event.hh"


namespace dmtpc {
  namespace jana  {

    class CcdClusterMaker  : public TObject     {     

    public:       
      ~CcdClusterMaker(){};
      CcdClusterMaker();
      void saveHisto(TFile *);
      void initHisto(); 
      int  findSeedClusters();
      int  hasPiSpark(); 
      int  measureClusters(); // cell-count ,  diameter, PCA, etc
      void purgeClusters(); // before writeing
      void bestTrack();
      void display();
      int  isFiducial(float xPix, float yPix);
      void attachSkinToClusters();
      void setDebug(int x){ dbg=x;}
      void setCuts(float x, float y, float z, float u, float v) { 
	cut_cellCountL=x; cut_clustLightL=y; cut_clustDiagPixL=z; cut_pcaRatioL=u;cut_pcaRatioH=v;}
      
      void setDrawings(TCanvas *can, TString outPath);
      void setCSVreport(TString outFname);
      void setJanEve( M3Event * x, CcdRunData  *y )  {m3Eve=x; ccd=y;}
      void setNsigThres12(float x,float y) {par_nSigThr1=x;par_nSigThr2=y;}
      void setFiducial(float bandPix); // >0 in, <0 out, =0 off
      TString listPiSpark;
      void setCapmaignTo(unsigned int tUnix) { campaignT0=tUnix;}

    private:
      void clear();

      TString coreName;
      int dbg;
      TCanvas *can; TString pdfName;
      FILE *fdCSV;
      M3Event * m3Eve;
      CcdRunData  *ccd;
      unsigned int campaignT0;     
           

      double createCluster(int jchX, int jchY);
      double  addCell(int jxi, int jyi, CcdCluster *cluster, int cellType, int clustId); // was M3ImageCell::CellType

      TH2S* hUtil; // helping build clusters

      enum { mxH=32 };
      TH1 *hA[mxH]; //

      // thresholds
      float par_nSigThr1, par_nSigThr2; // defines pixel threshold = ped+nSig*rms
      // cuts
      float cut_cellCountL, cut_clustLightL, cut_clustDiagPixL, cut_pcaRatioL, cut_pcaRatioH;
     
      TVector2 cut_fiduC;
      double cut_fiduPhi1, cut_fiduPhi2, cut_fiduR2max, par_fiduBandPix;
      double cut_pcaPhi1, cut_pcaPhi2; 

      float par_bin2mm_mesh_1x1; // never changes
      float par_bin2um_sensor_1x1; // never changes
      // bad frames filtering
      // float  par_starSparkLight;
      float  par_piSparkLtDens, par_piSparkDiag , par_piSparkNcell;
      ClassDef(CcdClusterMaker,1);       
    }; 
  }
}
#endif
