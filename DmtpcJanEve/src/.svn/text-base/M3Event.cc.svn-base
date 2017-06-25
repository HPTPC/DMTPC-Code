#include "M3Event.hh"

//=========================================
//=========================================
//=========================================
ClassImp(CcdBin)
CcdBin::CcdBin() { clear();}
CcdBin::CcdBin(Float_t uPix, Float_t vPix,Int_t mBinx, Float_t val, Float_t err){
  posPix=TVector2(uPix,vPix);
  light=val; sigLight=err;
  mBin=mBinx;
}

CcdBin::~CcdBin() {}

//================================= 
void CcdBin::clear() {
  posPix=TVector2(0,0);
  light=sigLight=0;
  mBin=0;
} 
//================================= 
void CcdBin::print( int flag){
  printf("m3ImageBin:  rot pos/pix(%.0f, %.0f)  light=%.1f , sigLight=%.1f (ADU)\n", posPix.X(),posPix.Y(),light, sigLight);
}

//=========================================
//=========================================
//=========================================
ClassImp(CcdCell)
CcdCell::CcdCell() { clear();}
CcdCell::~CcdCell() {}

//================================= 
void CcdCell::clear() {
     kxBin=kyBin=0;
    light=sigLight=0;
} 
//================================= 
void CcdCell::print( int flag){
  printf("m3ImageCell:  kxBin=%d kyBin=%d   light=%.1f , sigLight=%.1f (ADU)\n", kxBin,kyBin,light, sigLight);
}

//================================= 
//================================= 
//================================= 

CcdCluster:: CcdCluster() {  clear();}
//================================= 
void CcdCluster:: clear(){
 type=cluGood;
 kxBinL=kyBinL=kxBinH=kyBinH=0;
 kxBinCM=kyBinCM=0;
 light=0;
 pcaMin=pcaMax=pcaPhi=0;
 cellV.clear();
}

//================================= 
void CcdCluster::print( int flag){
  printf("Cluster: type=%d, CM X,Y=%.0f,%.0f (bin:1),  nCell=%d \n",type,kxBinCM, kyBinCM,cellV.size());
  printf("   light/adu=%.0f, BBox(bin:1):BL(%d,%d) TR(%d,%d) diag/bin=%.1f\n",   light,  kxBinL,kyBinL,kxBinH,kyBinH, getDiagonal_bin()); 
    printf(" PCA  Imax=%.3e Imin=%.3e phi/deg=%.1f\n",  pcaMin,pcaMax,pcaPhi/3.1416*180.);  
    if(flag<=1) return;
    for(unsigned int i=0;i<cellV.size() ;i++) {
      cellV[i].print();
    }
  }

 
//================================= 
//================================= 
//================================= 

ClassImp(M3Event)
//==================================
M3Event::M3Event(){ clear();  } // 
M3Event::~M3Event(){   } 
//==================================
void  M3Event::clearHeader() { 
 runId=0;
 agregRebin=0;
}
//==================================
void  M3Event::clearFrame() { // frame elements
 frameId=-1;
 timeStamp = TTimeStamp(0,0);
 for(int ic=0; ic<mxCam; ic++)
   clusterV[ic].clear();
}
//==================================
void  M3Event::print(int flag) {
  printf("m3Eve dump : frameId=%d,  runId=%d,  agregRebin=%d\nquadId:nClust ", frameId, runId,agregRebin);
     for(int ic=0; ic<mxCam; ic++)
       printf("%d:%d  ",ic,clusterV[ic].size());
     printf("\n");
     if(flag<=0) return;
     timeStamp.Print();
     if(flag<=1) return;
     for(int ic=0; ic<mxCam; ic++) {
       if(clusterV[ic].size()<=0) continue;
       printf("  cam%d clusters: ");
       for(int cl=0;cl<clusterV[ic].size();cl++){
	 if(clusterV[ic][cl].type<0) continue;
	 clusterV[ic][cl].print(flag-1);
       }
     }
	 
}


