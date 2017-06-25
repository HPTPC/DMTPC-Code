//*-- Author : Jan Balewski, MIT
//*-- February 2016
#ifndef M3Event_HH
#define M3Event_HH
#include <assert.h>

#include <TObject.h>
#include <TVector2.h>
#include <TH2.h>
#include <TMath.h>
#include <TTimeStamp.h>


class CcdRunData { // transient, not saved to TTRee
public:
  Int_t agregRebin;  // also flag valid cameras
  Float_t avrPedRms; // after all rebins 
  int quadId, camDaq, frameId, runId, mxFrame, exposureTime;
  TTimeStamp timeStamp;
  TString camSN;
  TH2S *hPedStat;// 
  TH2F *hLight;//  CCD image corrected for pedestal &mask
};


//---------------
class CcdCell  : public TObject { // single cell belonging to a cluster
 public:
  enum CellType{celTrig=4, celBody=3, celSkin=1};
  Int_t kxBin,kyBin; // in TH2
  Float_t light, sigLight; // (ADU) 
  CcdCell();
  ~CcdCell();
  void clear();  
  void print( int flag=0); 
 private:
  ClassDef(CcdCell,2);
};

//---------------
class CcdBin  : public TObject { // single high resolution bin
 public:
  TVector2 posPix; // (pix) location, after rotation  
  Float_t light, sigLight; // (ADU) 
  Int_t mBin; // helper, location of bin in 2D Light histo
  CcdBin();
  CcdBin(Float_t uPix, Float_t vPix, Int_t mBinx, Float_t val, Float_t err);
  ~CcdBin();
  void clear();  
  void print( int flag=0); 
 private:
  ClassDef(CcdBin,1);
};


//---------------
class CcdCluster : public TObject { //  collection of bins
 public:
  Int_t type; // classification scheme : 0=good,   or rejected
  std::vector <CcdCell> cellV;
  Int_t kxBinL,kyBinL,kxBinH,kyBinH; // (bin:1) box containing cluster
  Float_t kxBinCM, kyBinCM; //(bin:1) Light-weighed CM, in bin-units
  Float_t light; // (adu) sum over cluster
  Float_t pcaMin,pcaMax,pcaPhi; // princiapl inertia moments (PCA) and axis rotation

  enum ClusterType{cluGood=0, cluTiny=-1, cluLight=-2, 
		   cluDiag=-3, cluPca=-4, cluCam3Edge=-5, cluFidu=-6 };
  CcdCluster();  
  void clear();  
  void print(int flag=0);
  
  Double_t getDiagonal_bin(){
    int mdx= kxBinH - kxBinL+1;
    int mdy= kyBinH - kyBinL+1;
    return  sqrt(mdx*mdx+mdy*mdy);
  }
  

 private:
  ClassDef(CcdCluster,1);
};

  
//---------------
class M3Event : public TObject { 
 public:
  enum { mxCam=4};
  Int_t runId;// elements not changing withn a run
  Int_t agregRebin;  // also flag valid cameras
  Int_t frameId;// elements changing with each frame
  TTimeStamp timeStamp;
  std::vector <CcdCluster> clusterV[mxCam];
   
  M3Event();
  ~M3Event();
  void clear() { clearHeader();clearFrame(); }
  void clearHeader();
  void clearFrame();
 
  void print(int flag=0);
  void assert_quadId(int x) { assert(x>=0); assert(x<mxCam);}
  int getNumClust(int x){  assert_quadId(x); return clusterV[x].size();}

 private:
  ClassDef(M3Event,1);
};

#endif
