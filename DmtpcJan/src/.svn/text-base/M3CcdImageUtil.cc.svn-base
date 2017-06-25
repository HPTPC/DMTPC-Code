#include <assert.h>
#include "TH2.h"
#include "TH2I.h"

#include "TF1.h"
#include "TText.h"
#include "TStyle.h"

#include "Event.hh"
#include "Image.hh"
#include "CameraInfo.hh"

#include "TCanvas.h"
#include "TLine.h"
#include "TList.h"
#include "TLine.h"
#include "TList.h"
#include "TRandom.h" // for PDF sampling

#include "M3CcdImageUtil.hh" 

ClassImp(dmtpc::ped::M3CcdImageUtil); 

//=======================================
//=======================================
dmtpc::ped::M3CcdImageUtil::M3CcdImageUtil() {
}


//=======================================
//=======================================
dmtpc::ped::M3CcdImageUtil::~M3CcdImageUtil() {
} 


//=======================================
//=======================================
TH2I * dmtpc::ped::M3CcdImageUtil::convert764toOS(TH2I * h2inp, int camId){
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
