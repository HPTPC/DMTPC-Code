//*-- Author : Jan Balewski, MIT
//*-- January 2016
#ifndef M3JanEvent_HH
#define M3JanEvent_HH

#include <TObject.h>
#include <TVector2.h>
#include <TH2.h>
#include <TMath.h>

//---------------
class M3ImageCell  : public TObject{ // single cell belonging to a cluster
 public:
  enum CellType{celTrig=4, celBody=3, celSkin=1};
  Int_t jx,jy; // in TH2
  Double_t light, sigLight; // (ADU) signal
  ~M3ImageCell(){};
  M3ImageCell() { clear();}
  
  void clear() {
    jx=jy=0;
    light=sigLight=0;
  } 
  
  void print( int flag=0){
    printf("m3ImageCell:  jx=%d jy=%d   light=%.1f , sigLight=%.1f (ADU)\n",jx,jy,light, sigLight);
  }
  
 private:
  ClassDef(M3ImageCell,1);

};


//---------------
class M3ImageCluster : public TObject { //  collection of pixels
 public:
  Int_t type; // classification scheme : 0=good,   or rejected
  std::vector <M3ImageCell> cellV;
  Int_t nBody,nSkin; // nBody cells are stored as first
  Int_t jxL,jyL,jxH,jyH; // box containing whole cluster body
  Float_t bin2mm; // calibration: x(mm)= jx(bin)/bin2mm
  Float_t cmJx,cmJy; // Light-weighed CM, in bin-units
  Float_t light;
  Float_t Imin,Imax,Ialpha; // princiapl inertia moments (PCA) and axis rotation

  enum ClusterType{cluGood=0, cluTiny=-1, cluLight=-2, 
		   cluDiag=-3, cluPca=-4, cluCam3Edge=-5, cluFidu=-6, cluHadrRec=-7 };
  M3ImageCluster(float x) { bin2mm=x;  clear();}
  M3ImageCluster() { bin2mm=0;  clear();} // cint want this constructor
  
  void clear() {
    type=cluGood;
    nBody=nSkin=0;
    cmJx=cmJy=0;
    light=0;
    jxL=jyL=jxH=jyH=0;
    Imin=Imax=Ialpha=0; 
    cellV.clear();
  } 
  
  void print( int flag=0){
    printf("m3ImageCluster: type=%d, CM  X,Y=%.0f,%.0f (bin),  nCell=%d nBody=%d nSkin=%d bin/mm=%.2f\n",type,cmJx,cmJy,(int)cellV.size(),nBody,nSkin,bin2mm);
    printf("  CM=(%.1f,%.1f)  in bins, light/adu=%.0f, box:BL(%d,%d) TR(%d,%d) diag/mm=%.1f\n", cmJx,cmJy,  light, jxL,jyL, jxH,jyH, getDiagonal_mm()); 
    printf(" Princ Inert.  Imax=%.3e Imin=%.3e alpha/deg=%.1f\n", Imax,Imin,Ialpha/3.1416*180.);  
    if(flag<=0) return;
    for(unsigned int i=0;i<cellV.size() ;i++) {
      cellV[i].print();
    }
  }
  
  Double_t getDiagonal_mm(){
    return  getDiagonal_bin()/bin2mm;
  }

  Double_t getDiagonal_bin(){
  int mdx=jxH-jxL+1;
  int mdy=jyH-jyL+1;
  return  sqrt(mdx*mdx+mdy*mdy);
  }

  Double_t log10CellCount() { return TMath::Log10(cellV.size());}
  Double_t log10Light() { return TMath::Log10(light);}

 private:
  ClassDef(M3ImageCluster,1);

};

class CcdLight {
public:
  TH2I *hPedStat;
  TH2I *h2Dlight;//  CCD image corrected for pedestal &mask
  TH2I *h2DlightRot; // CCD image centered & alligned with best cluster
  Int_t pixOffsetRot; // offset for ??x, y-axis  for center of best cluster
  Float_t bin2mm; // after full rebin
  Int_t agregRebin; 
  Float_t avrPedRms; // after all rebins 
  int camId, camSrc;
  TString camSN;
};
  
//---------------
class M3JanEvent : public TObject { 
 public:
  enum { mxCam=4};

  M3JanEvent(){ 
    frameId=runId=0;
    memset(light,0, sizeof(light));
    clearFrame(); 
  }

  ~M3JanEvent(){};
  Int_t frameId, runId;
  std::vector <M3ImageCluster> clusterV[mxCam];
  CcdLight  light[mxCam];

  void clearFrame(){
    frameId=-1;
    for(int ica=0; ica<mxCam; ica++)
      clusterV[ica].clear(); 
  }
  
  void print(int flag=0) {
    printf("jEve dump : frameId=%d  runId=%d\n  cam:nClust ", frameId, runId);
    for(int ica=0; ica<mxCam; ica++) {
      CcdLight  &lg=light[ica];
      printf("camId=%d camSrc=%d SN=%s nClust=%d",lg.camId,lg.camSrc ,lg.camSN.Data(),clusterV[ica].size());
    printf("\n");
    }
  }
 

 private:
  ClassDef(M3JanEvent,1);
};

#endif
