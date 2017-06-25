#include <assert.h>
#include "TH2.h"
#include "TH2I.h"

#include "TF1.h"
#include "TFile.h"
#include "TString.h"
#include "TText.h"
#include "TStyle.h"
#include "TBox.h"
#include "TEllipse.h"

#include "TCanvas.h"
#include "TLine.h"
#include "TList.h"
#include "TList.h"
#include "TRandom.h" // for PDF sampling

#include "TMinuit.h"

#include "CcdTrackMaker.hh" 
#include "CcdCalibMaker.hh" 
#include "M3Event.hh" 


ClassImp(dmtpc::jana::CcdTrackMaker); 


//=======================================
//=======================================
dmtpc::jana::CcdTrackMaker::CcdTrackMaker() {
  // printf(" CcdTrackMaker() cnstr\n"); 
  m3Eve=0;  ccd=0;
  setDebug(0);
  can=0;
  fdCSV=0;
  pdfName="noPDF";
  setCuts(80.,400.);

  par_bin2mm_mesh_1x1=4; // 1 bin=250 um
  par_bin2um_sensor_1x1=1./12.; // never changes
}


//=======================================
//=======================================
void dmtpc::jana::CcdTrackMaker::dumpFitPar(){
  printf("dumpFitPar(%d):\n", numPar);
  for(int i=0;i< numPar;i++) printf("fitPar[%d] set=%.3e  out=%.3e err=%.3e\n",i,fitPar[i],fitParOut[i],fitParErr[i]);
}

 
//=======================================
//=======================================
void dmtpc::jana::CcdTrackMaker::clear() {
  ccdBinV.clear();

  numPar=0;
  memset(fitPar,0,sizeof(fitPar));
  

  hUtil->SetNameTitle(coreName+Form("_clUt_fr%d",m3Eve->frameId), Form(" clustFit rotated slice,  quad%d frame=%d ",ccd->quadId,m3Eve->frameId));
  hUtil->Reset("M");

  hUtil2-> Reset("M");
  hUtil3-> Reset("M");

  if(can)  { 
    can->Clear();
    can->Divide(2,2);
  }


  if(dbg>0) printf(" CcdTrackMaker::clear() done\n"); 
  //  if(dbg>0) m3Eve->print(0);
}

//=======================================
//=======================================
void dmtpc::jana::CcdTrackMaker::drawFitShape( Double_t *par){
  printf("drawFitShape()\n"); 
  
  double R=3*par[2];
  TEllipse *el=new TEllipse(par[0],par[1],R,R);
  el->SetFillStyle(0); el->SetLineColor(kMagenta);
  el->Draw();

  el=new TEllipse(par[3],par[4],R,R);
  el->SetFillStyle(0); el->SetLineColor(kMagenta);
  el->Draw();


}

//=======================================
void dmtpc::jana::CcdTrackMaker::make(){
  clear();
  int agrRebCluSeed=m3Eve->agregRebin;
  int quadId=ccd->quadId;    
  double maxVal=0;
  int kClust=-1;
  // elect best 'valid' clusters 
  for(int k=0;k<m3Eve->clusterV[quadId].size(); k++){
    CcdCluster &cluster= m3Eve->clusterV[quadId][k];
    if( cluster.type!=0 ) continue; //  already discarded
    if(dbg>1) cluster.print(0);
    double diag_pix=cluster.getDiagonal_bin()*agrRebCluSeed;
    if (maxVal>diag_pix) continue;
    maxVal=diag_pix;
    kClust=k;
  }


  CcdCluster &cluster=  m3Eve->clusterV[quadId][kClust];

  printf("CTM::make(quadId=%d) runId=%d   frameId=%d  nClust=%d  kClust=%d  diag/pix=%.1f\n",quadId,m3Eve->runId,ccd->frameId,cluster.cellV.size(),kClust,maxVal);

  if(kClust<0) return; // no suitable cluster seed

  int agrRebImage=ccd->agregRebin;
  // init fit params & pointers
 
  numPar=7;
  TString parNameA[]={"xL","yL","sig","xR","yR","aqAL","aqAR"};
  ccdBinVp=&ccdBinV;
  hUtil2p=hUtil2;
  hUtil3p=hUtil3;
  seedFitParams(cluster);
  // fitPar[5]=11;
  //fitPar[0]=1695-20;;
  //fitPar[1]=1527-20;
  //fitPar[3]=966-10;
  //fitPar[4]=1698-10;

  dumpFitPar(); 

  extract2Dbins(cluster );
  if(ccdBinV.size()<10) return; // too few bins to fit

  //  TVector2 pos(1700,1600); //1) u=58.2 |v|=73.0
  TVector2 pos(1700,1580); 
  // TVector2 pos(1800,1600); //2) u=-39.1 |v|=96.0
  //TVector2 pos(1000,1800); //3) u=785.5 |v|=106.5
  //TVector2 pos(fitPar[3],fitPar[4]+100); //4)
  fitFunc(pos,fitPar);
  //return;
  Double_t chi2_start=-1;
  chi2Func(numPar,0,chi2_start,fitPar,0);
  printf("START chi2=%.2e  nBin=%d  chi2/DOF=%.1f\n",chi2_start, ccdBinV.size(), chi2_start/ccdBinV.size());
 
  // initialize TMinuit with a maximum of N params
  TMinuit *myMinuit = new TMinuit(numPar);
  myMinuit->SetFCN(chi2Func);

   Double_t arglist[10];
   Int_t ierflg = 0;

   arglist[0] = 1;
   myMinuit->mnexcm("SET ERR", arglist ,1,ierflg);

   // Set starting values and step sizes for parameters
  
   for(int i=0;i<numPar;i++) {
     double val=fitPar[i] ;
     double step=0.1;
     if(fabs(val)>1.e-5) step=fabs(val)*0.05;
     // if(i!=5 && i!=2) step=-1;
     //if(i<2) step=-1;
     //     if(i==4) step=-1;
    myMinuit->mnparm(i, parNameA[i],val,step, 0,0,ierflg);
   }

   // Now ready for minimization step
   arglist[0] = 500;
   arglist[1] = 1.;
   myMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);
   
   // Print results
   Double_t amin,edm,errdef;
   Int_t nvpar,nparx,icstat;
   myMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
   //void mnstat(Double_t &fmin, Double_t &fedm, Double_t &errdef, Int_t &npari, Int_t &nparx, Int_t &istat) 
   //*-*-*-*-*Returns concerning the current status of the minimization*-*-*-*-*
   //*-*      =========================================================
   //*-*       User-called
   //*-*          Namely, it returns:
   //*-*        FMIN: the best function value found so far
   //*-*        FEDM: the estimated vertical distance remaining to minimum
   //*-*        ERRDEF: the value of UP defining parameter uncertainties
   //*-*        NPARI: the number of currently variable parameters
   //*-*        NPARX: the highest (external) parameter number defined by user
   //*-*        ISTAT: a status integer indicating how good is the covariance
   //*-*           matrix:  0= not calculated at all
   //*-*                    1= approximation only, not accurate
   //*-*                    2= full matrix, but forced positive-definite
   //*-*                    3= full accurate covariance matrix
   //*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   myMinuit->mnprin(2,amin);
   //*-*-*-*Prints the values of the parameters at the time of the call*-*-*-*-*
   //*-*    ===========================================================
   //*-*        also prints other relevant information such as function value,
   //*-*        estimated distance to minimum, parameter errors, step sizes.
   //*-*
   //*-*         According to the value of IKODE, the printout is:
   //*-*    IKODE=INKODE= 0    only info about function value
   //*-*                  1    parameter values, errors, limits
   //*-*                  2    values, errors, step sizes, internal values
   //*-*                  3    values, errors, step sizes, first derivs.
   //*-*                  4    values, parabolic errors, MINOS errors
   //*-*    when INKODE=5, MNPRIN chooses IKODE=1,2, or 3, according to ISW(2)
   //*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   
  printf("oMinuitFit() END, ISTAT=%d\n",icstat);


  chi2Func(numPar,0,chi2_start,fitPar,1);
  printf("FINAL chi2=%.2e\n",chi2_start);

  for(int i=0;i<numPar;i++) 
    myMinuit->GetParameter(i,fitParOut[i],fitParErr[i]);

  dumpFitPar();
  chi2Func(numPar,0,chi2_start,fitParOut,3);
  printFitTrack(fitParOut);
  cluster.print(1);
}

//=======================================
void dmtpc::jana::CcdTrackMaker::extract2Dbins(CcdCluster &cluster){
  int agrRebCluSeed=m3Eve->agregRebin;
  int quadId=ccd->quadId;    
  float xPix0=(cluster.kxBinCM-0.5)*agrRebCluSeed;
  float yPix0=(cluster.kyBinCM-0.5)*agrRebCluSeed;
  double phi=cluster.pcaPhi; //rad
  //phi-=1.7/4.;//tmp=22 deg
  if(dbg>0) cluster.print(0);
  printf("CTM::extract2DLight at CM: x,y =%.1f,%.1f (pix)  phi/rad=%.3f  \n",
	 xPix0,yPix0,phi);

  double ctPhi=cos(phi),stPhi=sin(phi);

  // shift & rotate light to center it on the cluster
  int nchX=ccd->hPedStat->GetNbinsX();
  int nchY=ccd->hPedStat->GetNbinsY();
  float xL=5000,xH=-xL, yL=xL, yH=xH;
  for(int jchX=1; jchX<=nchX; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N
      int mBin=ccd->hPedStat->GetBin(jchX, jchY);  //   convert 2D to 1D index
      if (  ccd->hPedStat->GetBinContent(mBin) ) continue; // drop masked channels
      float rms=ccd->hLight->GetBinError(mBin);
      float val=ccd->hLight->GetBinContent(mBin);
      assert(rms>5);// (adu) sth went wrong

      // transformx,y coordinates
      double xPix=ccd->hLight->GetXaxis()->GetBinCenter(jchX);
      double yPix=ccd->hLight->GetYaxis()->GetBinCenter(jchY);

      double xPixM=xPix-xPix0, yPixM=yPix-yPix0;
      double uPix=xPixM*ctPhi + yPixM*stPhi ;
      double vPix=-xPixM*stPhi + yPixM*ctPhi;
      
      if(fabs(vPix) > par_bandHalfWidthPix ) continue;
      if(fabs(uPix) > par_bandHalfLengthPix) continue; 

      if(xL>xPix) xL=xPix;
      if(xH<xPix) xH=xPix;
      if(yL>yPix) yL=yPix;
      if(yH<yPix) yH=yPix;
      
      hUtil->SetBinContent(mBin,val);
      CcdBin bin(xPix,yPix,mBin,val,rms);
      ccdBinV.push_back(bin);
      // printf("mB=%d\n",mBin);
      //tmp
      // double valFit=fitFunc(bin.posPix,fitPar);
      //hUtil2->Fill(uPix,vPix,valFit);
    }
  
  double aMin=ccd->avrPedRms;
  
  hUtil->SetAxisRange(xL,xH,"x");
  hUtil->SetAxisRange(yL,yH,"y");
  hUtil->SetMinimum(-aMin);

  hUtil2->SetAxisRange(xL,xH,"x");
  hUtil2->SetAxisRange(yL,yH,"y");
  hUtil2->SetMinimum(-aMin);
  
  hUtil3->SetAxisRange(xL,xH,"x");
  hUtil3->SetAxisRange(yL,yH,"y");
  hUtil3->SetMinimum(-aMin);

  printf("CTM::extract2DLight ccdBinV.size=%d\n",ccdBinV.size());
}
//=======================================
void dmtpc::jana::CcdTrackMaker::seedFitParams(CcdCluster &cluster) {
  int agrRebCluSeed=m3Eve->agregRebin;
  printf("CTM::seedFitParams() agregReb=%d\n",agrRebCluSeed); cluster.print(1);
  assert(numPar<=maxFitPar);

  assert(cluster.cellV.size()>5);

  double cluWidthPix=sqrt(cluster.pcaMin/cluster.light) * agrRebCluSeed;
  double diag_pix=cluster.getDiagonal_bin()*agrRebCluSeed;
  double lightDens1x1=cluster.light/cluster.cellV.size()/agrRebCluSeed/agrRebCluSeed;

  float xPix0=(cluster.kxBinCM-0.5)*agrRebCluSeed;
  float yPix0=(cluster.kyBinCM-0.5)*agrRebCluSeed;
  double phi=cluster.pcaPhi; //rad
  double cosPhi=cos(phi),sinPhi=sin(phi);
  

  fitPar[0]=xPix0-diag_pix*cosPhi/2.; //par xL (pix)
  fitPar[1]=yPix0-diag_pix*sinPhi/2.; //par yL (pix)
  fitPar[2]=  cluWidthPix;  // par: sig (pix)
  fitPar[3]=xPix0+diag_pix*cosPhi/2.; //par xR (pix)
  fitPar[4]=yPix0+diag_pix*sinPhi/2.; //par yR (pix)
  fitPar[5]=ccd->avrPedRms/3.; //par sqAL (adu)
  fitPar[6]= fitPar[5]; //par sqAR (adu)
}

//=======================================
void dmtpc::jana::CcdTrackMaker::display() {
  if(can==0) return;
  can->cd(1);
  double pedRms=ccd->avrPedRms;
  int agrRebCluSeed=m3Eve->agregRebin;
  int quadId=ccd->quadId;

  ccd->hLight->SetMinimum(pedRms);
  ccd->hLight->SetMaximum(4*pedRms);

  //1  ccd->hLightRot->SetMinimum(pedRms);
  //1 ccd->hLightRot->SetMaximum(4*pedRms);
  hUtil->Draw("colz");
  drawFitShape(fitPar);
  gPad->SetGrid();

  printf("displayFit best cluster from nCl=%d\n",m3Eve->clusterV[quadId].size());

  can->cd(2);
  hUtil->Draw("lego2");
  //  ccd->hLight->Draw("colz");
  // drawFitShape(fitPar);



  can->cd(3);
  hUtil3->Draw("colz");
  drawFitShape(fitPar);


  can->cd(4);
  hUtil3->Draw("lego2");
  //  hUtil2->Draw("colz");
  //drawFitShape(fitPar);

  //  if(can)  can->Print( pdfName,"pdf"); // full frame
 
  if(can)  can->Print( pdfName,"pdf");

  // activate to produce 1 png per cluster
  // TString pngName=Form("clust_R%d_fr%d_quad%d_reb%d.png",ccd->runId,ccd->frameId,quadId, agregRebin);  can->Print( pngName);

}

//=======================================
//=======================================
void dmtpc::jana::CcdTrackMaker::printFitTrack( Double_t *par){
  TVector2 L(par[0],par[1]);
  TVector2 RmL=(TVector2(par[3],par[4])-L);
  // TVector2 avrRL=(TVector2(par[3],par[4])-L);
  double d=RmL.Mod();
  double light=hUtil3->Integral();
  double width=par[2];
  double asy=par[5]/par[6];
  printf("fitTrack:  range=%.0fpix  light=%.2fkAdu  width=%.1fpix phi/deg=%.1f ampAsy=%.2f agrReb=%d\n",RmL.Mod(), light/1000.,width, RmL.Phi()/3.1416*180,asy,ccd->agregRebin);

}


//=======================================
//=======================================
void dmtpc::jana::CcdTrackMaker::initHisto() {

  assert(m3Eve);
  assert(ccd->hPedStat);
  int quadId=ccd->quadId;
  coreName=Form("quad%dtra",quadId);

  //----- fill event header
  // write runID , but only once - wil brake if raw data for multiple runs are cncatinated

  if(m3Eve->runId==0) m3Eve->runId=ccd->runId;
  else assert( m3Eve->runId==ccd->runId);
  m3Eve->agregRebin=ccd->agregRebin;

  // create local 2D plane storing some cluster info

  hUtil=(TH2F *)ccd->hPedStat->Clone();
  hUtil->SetNameTitle("U"+coreName, coreName+" band of CCD image");

  hUtil2=(TH2F*)  hUtil->Clone();
  hUtil2->SetNameTitle("U2"+coreName, coreName+" fitFunc-start");

  hUtil3=(TH2F*)  hUtil->Clone();
  hUtil3->SetNameTitle("U2"+coreName, coreName+" fitFunc-finish");

  hUtil->Sumw2();  // allow independent errors from values
  hUtil->SetMarkerSize(0.7);//<-- large text 



  memset(hA,0,sizeof(hA)); // input spectra, unweighted
  TH1* h;
 
  enum {mxCaseC=13,mxCase=10};
  int nCase=mxCaseC;
  hA[0]=h=new TH1D(coreName+"_cluCase","XXX QA of clusters; type; CLUSTERS",nCase,0,nCase);  h->GetXaxis()->SetTitleOffset(0.4);  h->GetXaxis()->SetLabelSize(0.06);  h->GetXaxis()->SetTitleSize(0.05); h->SetMinimum(0.8);  h->SetFillColor(29);h->SetLineWidth(2);  h->SetMarkerSize(2);//<-- large text 
    

  TString nameA[mxCaseC]={"any","nCell","faint","diag","fiduIn","fiduOut","light","kill3Edge","no3Edge","pcaR","pcaPhi","isGood","-"};
  for(int i=0;i<mxCaseC;i++) {
    hA[0]->Fill(nameA[i],0.);
  }

  hA[1]=h=new TH1D(coreName+"_frmCase","QA of frames; type; FRAMES",nCase,0,nCase);  h->GetXaxis()->SetTitleOffset(0.4);  h->GetXaxis()->SetLabelSize(0.06);  h->GetXaxis()->SetTitleSize(0.05); h->SetMinimum(0.8); h->SetFillColor(42);h->SetLineWidth(2);  h->SetMarkerSize(2);//<-- large text 
    
  TString nameB[mxCase]={"inpFrm","starSpark","hasClust","piSpark","goodClust","manyGood","g","inpRun","expoHour","n"};
  for(int i=0;i<mxCase;i++) {
    hA[1]->Fill(nameB[i],0.);
  }
  hA[1]->Fill("inpRun",1.);
  
  //2,3,4 - free


  printf("CTM::initHisto end, cuts:  bandHalfWidthPix=%.0f\n",
	 par_bandHalfWidthPix);
}

//=======================================
//=======================================
void dmtpc::jana::CcdTrackMaker::setDrawings(TCanvas *canX, TString outPath){
  assert(canX);

  can=canX;
  can->cd(0);

  int runId=m3Eve->runId;
  //  int uReb=calibData->par_userRebin;
  double pedRms=ccd->avrPedRms;
  int agregRebin=ccd->agregRebin;
  int quadId=ccd->quadId;

  TText mesg(0.3,0.8,Form("quad%d run%d  cover page",quadId, runId));
  mesg.Draw();  // mesg.Print();

  TText mesg2(0.1,0.6,Form("CUTS: bandHalfWidthPix=%.0f", 
			   par_bandHalfWidthPix));
  mesg2.SetTextSize(0.025);  mesg2.Draw();  // mesg2.Print();

  pdfName=outPath+"/"+Form("anaTrack_R%d_quad%d_reb%d.pdf",runId,quadId, agregRebin);
  can->Print( pdfName+"(","pdf");

}

