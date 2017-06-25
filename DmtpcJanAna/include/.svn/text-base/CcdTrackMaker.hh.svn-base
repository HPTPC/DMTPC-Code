#ifndef CcdTrackMaker_HH
#define CcdTrackMaker_HH

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
#include <stdio.h>
#include "../DmtpcJanEve/include/M3Event.hh"
#include "GlobalJanFit.hh"
namespace dmtpc {
  namespace jana  {

    class CcdTrackMaker  : public TObject     {     

    public:       
      ~CcdTrackMaker(){};
      CcdTrackMaker();
      void saveHisto(TFile *);
      void initHisto(); 
      void setDebug(int x){ dbg=x;}
      void setJanEve( M3Event * x, CcdRunData  *y )  {m3Eve=x; ccd=y;}
      void make();
      void display();
      void setDrawings(TCanvas *can, TString outPath);
      void setCuts(float x, float y) { 
	par_bandHalfWidthPix=x;  par_bandHalfLengthPix=y; 
      }
      void dumpFitPar();
      void printFitTrack( Double_t *par);

    private:
      void clear();
      void extract2Dbins(CcdCluster &cluster);
      void seedFitParams(CcdCluster &cluster );
      void drawFitShape( Double_t *par);

      TString coreName;
      int dbg;
      M3Event * m3Eve;
      CcdRunData  *ccd;
      std::vector <CcdBin> ccdBinV;

      //  threshold and cut
      float  par_bandHalfWidthPix, par_bandHalfLengthPix; 
      float  par_bin2mm_mesh_1x1, par_bin2um_sensor_1x1;

      // Minuit
      enum {maxFitPar=8}; 
      int numPar;
      Double_t fitPar[maxFitPar],  fitParOut[maxFitPar],fitParErr[maxFitPar];

      // OLD ....    
      void setCSVreport(TString outFname);
  
    private:
      TCanvas *can; TString pdfName;
      FILE *fdCSV;
      
      enum { mxH=32 };
      TH1 *hA[mxH]; //
      TH2F *hUtil, *hUtil2, *hUtil3;

      ClassDef(CcdTrackMaker,1);       
    }; 
  }
}
#endif
