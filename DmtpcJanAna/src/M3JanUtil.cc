#include <assert.h>
#include <TH2.h>

#include "M3FrameQA.hh"
#include "M3JanUtil.hh"

//=======================================
//=======================================
dmtpc::jana::M3JanUtil::M3JanUtil() {
   printf(" M3JanUtil() cnstr\n"); 
}

//=======================================
//=======================================
void dmtpc::jana::M3JanUtil::verifyQuadId(int quadId, TString camSN){
  printf("verifyQuadId=%d  daq camSN=%s\n",quadId,camSN.Data());
  switch (quadId) {
  case 0: assert(camSN=="PL0141514"); break;
  case 1: assert(camSN=="PL0251514"); break;
  case 2: assert(camSN=="PL0261514"); break;
  case 3: assert(camSN=="PL0544710"); break;
  default:
    assert(8765==9875); // not implemented
  }
}


//=======================================
//=======================================
TH2I *dmtpc::jana::M3JanUtil::convert764toOS(TH2I * h2inp, int camId){
  //printf(" M3CcdImageUtil::convert764toOS( ), inp=4x4 binning w/o OS data \n");
  assert( h2inp);
  const int nbinsx =  h2inp->GetNbinsX();
  const int nbinsy =  h2inp->GetNbinsY();
  const double xmin = h2inp->GetXaxis()->GetXmin();
  const double xmax = h2inp->GetXaxis()->GetXmax();
  const double ymin = h2inp->GetYaxis()->GetXmin();
  const double ymax = h2inp->GetYaxis()->GetXmax();

  // just to be sure check some values
  assert(nbinsx==764);
  assert(nbinsx==nbinsy);
  assert(xmax==3056);
  assert(xmax==ymax);
  // h2inp->Draw("colz");

  // this image needs OS around it
  TString core=Form("cam%d",camId);
  TH2I * h2out= new TH2I(core+"_fixed_raw4x4", core+"_fixed raw 4x4  with OS bands",
                         771,0,3084,771,0,3084);

  // copy data, adding 5 bins of dark pixles

  int mBin;
  for(int jchX=1; jchX<=nbinsx; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nbinsy; jchY++) { // range 1,N
      int val= h2inp->GetBinContent(jchX,jchY);
      h2out->SetBinContent(jchX+5,jchY+5, val);
      if(val<=0)
        printf("jchX,jchY=%d,%d,  val%d\n", jchX,jchY,val);
    }

  return h2out; // there is memory leak, you must delet this pointer after use
  // junk:
  int jchX=6;
  for(int jchY=1+5; jchY<=nbinsy+5; jchY++) { // range 1,N
      int val= h2out->GetBinContent(jchX,jchY);
        printf("jchX,jchY=%d,%d,  val%d\n", jchX,jchY,val);
    }


}

#include "../DmtpcQuality/include/RunQuality.hh"

//=======================================
//=======================================
void dmtpc::jana::M3JanUtil::testRicSparkTool(int runId, TString dbName){
  //---setup RunQuality  
  dmtpc::Quality::RunQuality runQ = dmtpc::Quality::RunQuality(runId,dbName);
  int noInfo= runQ.noInfo();
  printf("extract spark list for run=%d from %s, noInfo=%d, wiat ...\n",runQ.runid(),dbName.Data(),noInfo);

  if(noInfo)return ;
  printf("Ric's sparks are loaded and ready\n");
}


