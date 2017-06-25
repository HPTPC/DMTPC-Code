#include <assert.h>
#include "TH2.h"
#include "TH2I.h"
#include "TF1.h"
#include "TFile.h"
#include "TString.h"
#include "TSystem.h" // for ->Exec


#include "M3StarSparkDet.hh" 
#include "M3FrameQA.hh" 

ClassImp(dmtpc::jana::M3FrameQA); 

//=======================================
//=======================================
dmtpc::jana::M3FrameQA::M3FrameQA() {  
  runId= mxFrame=0;
  hFrameStat=0;
  coreName="fixMe";
  quadId=0;
  runId=0;
  inpFname="none1";
  // printf(" M3FrameQA() cnstr R%d  coreName=%s\n",runId,coreName.Data()); 
  
 }


//=======================================
//=======================================
void dmtpc::jana::M3FrameQA::createStatus( dmtpc::jana::M3StarSparkDet *spDet) {
  createStatus( spDet->runId, spDet->quadId, spDet->mxFrame);
}

 
//=======================================
//=======================================
void dmtpc::jana::M3FrameQA::createStatus(int runIdX, int quadIdX, int mxFrameX){
  assert(runId==0);
  quadId=quadIdX;
  runId=runIdX;
  coreName=Form("statR%dQ%d",runIdX,quadId);
  mxFrame=mxFrameX;
  printf("M3FrameQA::createStatus run=%d, quad=%d mxFrame=%d\n",runId, quadId,mxFrame);
  hFrameStat=new TH1I( coreName, Form("frame status R%d quad%d; frame Id",runId,quadId), mxFrame, -0.5,  mxFrame-0.5);
}

//=======================================
//=======================================
void dmtpc::jana::M3FrameQA::saveHisto(TFile *fd) {
  assert(hFrameStat);
  printf(" M3FrameQA histos  are written  to  %s" ,fd->GetName());
  hFrameStat->Write();
  printf(" , save Ok\n");

}

//=======================================
//=======================================
void dmtpc::jana::M3FrameQA::maskFrame( int frameId, float val){
  assert(frameId>=0);
  assert(frameId<mxFrame);
  hFrameStat->Fill(frameId,val);
}

//=======================================
//=======================================
int dmtpc::jana::M3FrameQA::isBad(int runIdX, int frameId){
  assert(runIdX==runId);
  assert(frameId>=0);
  assert(frameId<mxFrame);
  return (int) hFrameStat->GetBinContent(frameId+1);
}

//=======================================
//=======================================
void dmtpc::jana::M3FrameQA::print() {
  printf(" M3FrameQA::print() run=%d, quad=%d masked frameId: ",runId,quadId);
  if( hFrameStat==0) { printf("Error: no histo loaded\n"); assert(8==90);}
  int nBad=0;
  for(int k=1; k<=mxFrame; k++) {
    if( hFrameStat->GetBinContent(k) ) {
      printf(" %d,",k-1);
      nBad++;
    }
  }
  printf( " nBad=%d of %d\n",nBad,mxFrame);
}

//=======================================
//=======================================
void dmtpc::jana::M3FrameQA::printCSV() {
  printf("#M3FrameQA,R%d,Q%d,  ",runId,quadId);
  int nBad=0;
  for(int k=1; k<=mxFrame; k++) {
    if( hFrameStat->GetBinContent(k) ) {
      printf(" %d,",k-1);
      nBad++;
    }
  }
  printf( " b%d,f%d\n",nBad,mxFrame);
}

//=======================================
//=======================================
int  dmtpc::jana::M3FrameQA::loadFrameQA(int runIdX, int quadIdX,TString fName) {
  assert(runId==0);
  quadId=quadIdX;
  runId=runIdX;
  coreName=Form("statR%dQ%d",runIdX,quadId);
 
  assert(runIdX>0);
  runId=runIdX;
  TFile *fd=new TFile(fName);
  assert(fd->IsOpen());
  inpFname=fName;
  printf(" loadFrameQA  for R%d  hName=%s\n",runId,coreName.Data());
  hFrameStat=(TH1I*) fd->Get(coreName);
  if(hFrameStat==0) fd->ls();
  assert(hFrameStat);
  mxFrame= hFrameStat->GetNbinsX();
  return  mxFrame;
}


//=======================================
//=======================================
void dmtpc::jana::M3FrameQA::producePatcher(TString fName, TString badFrameST){
  printf("M3FrameQA::producePatcher(%s)\n",coreName.Data());
  TString strClone=badFrameST, strOut="";

  char *item=strtok((char*)strClone.Data(),","); // init 'strtok'
  int i=0;
   do {
     //printf("i=%d, item=%s=\n",i,item);
     if(i>0)strOut+=Form(" h->Fill(%s,val);\n",item);
     i++;
   } while (item=strtok(0,","));  // advance by one name    nTotPulse++;
   int nBad=i-1; 
   
   printf("produceFramePatcher  %d fixes, exec:  %s\n",nBad,fName.Data()); 


   FILE *fd=fopen(fName.Data(),"w"); assert(fd);

   fprintf(fd,"#!/bin/bash\n# .... edit status tables of given run, inplace\n");
   if(nBad<=0)   fprintf(fd," echo no spark frames to patch in  R%d; exit \n",runId);

   fprintf(fd,"set -u ;  # exit  if you try to use an uninitialized variable\nset -e ;    #  bash exits if any statement returns a non-true return value\nset -o errexit ;  # exit if any statement returns a non-true return value\nroot -b   <<EOF\n");

  fprintf(fd," TFile *fd=new TFile(\"%s\",\"UPDATE\");\n", inpFname.Data());
  fprintf(fd," if(!fd->IsOpen()) { printf(\" bad input file=%cs=, skip\\n\",fd->GetName()); exit(); }\n  fd->ls(); \n",37);
  fprintf(fd,"  TString hName=\"\%s\"; \n",coreName.Data()); 
  fprintf(fd,"  TH1* h=(TH1*) fd->Get(hName);\n if(h==0) {  printf(\" missing histo for %cs, skip\\n\",hName.Data());  exit(); } \n  h->Print(); \n float val=2;\n",37);

  fprintf(fd,"// h->Fill(,val); // manual\n\nif(%d) { // ... now masking new frames starts \n",nBad);
  fprintf(fd,"%s",strOut.Data());
  fprintf(fd,"}\n");
 
  fprintf(fd,"  h->Print(); fd->Write();\n.q\nEOF\n  echo done paching %d bad frames in  R%d\n",nBad,runId);
  fclose(fd);
  gSystem->Exec(Form("chmod +x %s", fName.Data()));

}


//=======================================
//=======================================
void dmtpc::jana::M3FrameQA::addTo( std::set<int>  * sparkSet) {
  //printf("addTo:  otherSparks=%d\n", sparkSet->size());
  for(int k=1; k<=mxFrame; k++) {
    if( hFrameStat->GetBinContent(k) ) {
      sparkSet->insert(k);
    }
  }
}
