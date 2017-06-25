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

#include "M3CcdClusterMaker.hh" 
#include "M3CcdCalibMaker.hh" 
#include "M3JanEvent.hh" 

ClassImp(dmtpc::ped::M3CcdClusterMaker); 

//=======================================
//=======================================
dmtpc::ped::M3CcdClusterMaker::M3CcdClusterMaker() {
  // printf(" M3CcdClusterMaker() cnstr\n"); 
  jEve=0;
  camId=-1;
  setDebug(0);
  can=0;
  fdCSV=0;
  nPromise=0;
  pdfName="noPDF";
  h2Dlight=hPedStat=h2DlightRot=0;
  setNsigThres12(1,2);   
  par_fiduBandPix=0; // no fiducial cut

  // fixed cuts
  par_starSparkLight=1e6;
  par_piSparkLtDens=210; par_piSparkDiag=60; par_piSparkNcell=200;
  listPiSpark="pi-spark frames ";
  listStarSpark="star-spark frames ";
}


 
//=======================================
//=======================================
void dmtpc::ped::M3CcdClusterMaker::clear() {
 
  abortFrame=frmHealthy;
 
  hUtil->SetNameTitle(coreName+Form("_clUt_fr%d",jEve->frameId), Form("util clustFind info cam%d  frame=%d ",camId,jEve->frameId));
  hUtil->Reset();
  if(can) can->Clear();
  if(dbg>0) printf(" M3CcdClusterMaker::clear() done\n"); 
  //  if(dbg>0) jEve->print(0);
}



 //=======================================
//=======================================
void dmtpc::ped::M3CcdClusterMaker::display() {

  double pedRms=jEve->light[camId].avrPedRms;
  int agregRebin=jEve->light[camId].agregRebin;

  h2Dlight->SetMinimum(pedRms);
  h2Dlight->SetMaximum(4*pedRms);

  h2DlightRot->SetMinimum(pedRms);
  h2DlightRot->SetMaximum(4*pedRms);

  h2Dlight->Draw("colz");

  printf("display good clusters from nCl=%d\n",jEve->clusterV[camId].size());
  double x0c=600,y0c=600, maxLight=0;
  for(int k=0;k<jEve->clusterV[camId].size(); k++){
    M3ImageCluster &cluster= jEve->clusterV[camId][k];
    if( cluster.type<0 ) continue; //  already discarded
    if(dbg>0) cluster.print(0);
    // revert position to full-rang CCD bins
    double  x0=cluster.cmJx*agregRebin;
    double  y0=cluster.cmJy*agregRebin;
    double thetaDeg=cluster.Ialpha/3.1416*180.;
    //if( cluster.Ialpha<0) thetaDeg=90+thetaDeg;
    double R1=0.5*cluster.getDiagonal_bin()*agregRebin;

    double beta=30;
    TEllipse *el=new TEllipse(x0,y0,R1,R1/2.5,beta,360-beta,thetaDeg);
    el->SetFillStyle(0); el->SetLineColor(kMagenta);
    el->Draw();

    // draw label
    TString txL= Form("L=%.0f kADU",cluster.light/1000.);
    if (cluster.light<1800) txL= Form("L=%.0f ADU",cluster.light);
    double offY=10*(sin(cluster.Ialpha) -cos(cluster.Ialpha) );    
    double offX=10*(cos(cluster.Ialpha) + sin(cluster.Ialpha));
    TText *tt=new TText( x0+offX, y0+offY,txL);
    tt->Draw();
    tt->SetTextSize(0.03);   tt->SetTextColor(kMagenta);
    tt->SetTextAngle(thetaDeg-90);

    // ..... populate all used cells
    for(int ic=0;ic<cluster.cellV.size();ic++) {
      M3ImageCell &cell=cluster.cellV[ic];
      float xPix=cell.jx*agregRebin;
      float yPix=cell.jy*agregRebin;
      hA[16]->Fill(xPix,yPix);
    }


    //......  draw extrapolation lines to the source
    float x0M3=2800, y0M3=2800;
    if(camId==2) { x0M3=300;  y0M3=2800;}
    //cam0, cam1 - not implemented
    float x0Hall= (x0 -x0M3) /40.;// in cm
    float y0Hall= (y0 -y0M3) /40.;// in cm
    int max_cm=200;
    for(int u=-max_cm; u<max_cm; u+=2 ) {     
      float x,y;
      if(fabs( cluster.Ialpha -1.7) > 0.8 ) { // ~horizontal track
	x=u+x0Hall;
	y=y0Hall + (x-x0Hall) * tan( cluster.Ialpha );
      } else { // ~ vertical track
	y=u+y0Hall;
	x=x0Hall + (y-y0Hall) * tan( 3.1416/2.-cluster.Ialpha );
      }
      hA[19]->Fill(x,y);
    }
 

    // tag  good cluster with most light
    if(maxLight> cluster.light) continue;
    maxLight=cluster.light;
    x0c=x0; y0c=y0;
  }// end of cluster loop
  
  float del=100;

  // if(can)  can->Print( pdfName,"pdf"); // full frame
 
#if 0 // test fiducial mask
  // REMEMBER to disable cluster evaluation

  int nchX=hPedStat->GetNbinsX();
  int nchY=hPedStat->GetNbinsY(); 
  
  for(int jchX=1; jchX<=nchX; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N
      float xPix=h2Dlight->GetXaxis()->GetBinCenter(jchX);
      float yPix=h2Dlight->GetYaxis()->GetBinCenter(jchY);
      if( !isFiducial(xPix,yPix) ) h2Dlight->SetBinContent(jchX,jchY,0);
    }

  return;
#endif
  hUtil->Draw("text same");

  h2Dlight->SetAxisRange(x0c-del,x0c+del,"x");
  h2Dlight->SetAxisRange(y0c-del,y0c+del,"y");
  
  // draw scale
  float bar_mm=2*cut_clustDiagL;
  float pixLen=bar_mm*jEve->light[camId].bin2mm*agregRebin;
  float x0l=x0c-del+1.5, y0l=y0c-del+1.5;
  if( x0l<0) x0l=0;
  if( y0l<0) y0l=0;
  TBox *box=new TBox( x0l,  y0l,  x0l+pixLen, y0l+pixLen/3);
  box->Draw();
  box->SetFillStyle(0); box->SetLineColor(kRed);

  // draw light 
  TText *tt=new TText( x0l+3., y0l+pixLen/6., Form("%.1f mm",bar_mm));
  tt->Draw();
  tt->SetTextSize(0.02);   tt->SetTextColor(kRed);

  if(can)  can->Print( pdfName,"pdf");

  //  printf(" %s->Draw(\"lego1 0\") \n",h2Dlight->GetName());
  //  printf("end nClust=%d\n",clusterV.size());

}
//=======================================
//=======================================
int dmtpc::ped::M3CcdClusterMaker::hasPiSpark(){ // return 1 if spark found
  int numClust=jEve->clusterV[camId].size();
  printf(" ClusterMaker::hasPiSpark() nCl=%d   start..\n",numClust);

  if(numClust<=0) return 0;
  //............................. START ............
  
  for(int k=0;k<numClust; k++){
    M3ImageCluster &cluster= jEve->clusterV[camId][k];
    int clustId=k; // for clarity
    double diagMM=cluster.getDiagonal_mm();
    double ltDens=cluster.light/diagMM;

    if(dbg>1)printf("clId=%d diag=%.1f L/diag=%.1f\n", clustId,diagMM,ltDens);
    hA[6]->Fill(diagMM,ltDens);

    // detect pathological frames: pi-spark
    if(cluster.cellV.size()>200) cluster.print(0);

    if (ltDens > par_piSparkLtDens && diagMM > par_piSparkDiag ||
	cluster.cellV.size()> par_piSparkNcell	) {
      abortFrame=frmPiSpark;  // kkkkkk  K I L L    E V E N T  kkkkkkk
      hA[1]->Fill("piSpark",1.);
      listPiSpark+=Form("%d,",jEve->frameId);
      if(fdCSV) fprintf(fdCSV,"%d , %d, pi-spark-%d , %.3e, %.1f\n",jEve->frameId,camId,clustId,cluster.light,cluster.getDiagonal_mm());
      printf(" ClusterMaker::hasPiSpark_TRUE, abort frame %d  camId=%d\n", jEve->frameId,camId);
      return 1;  // does not matter 
    }
  }
  return 0 ; // no spark
}

//=======================================
//=======================================
int dmtpc::ped::M3CcdClusterMaker::measureClusters(){
  int nCluOK=0;
  int agregRebin=jEve->light[camId].agregRebin;

  // computes: diagonal, moments of inertia (aka PCA)
  int numClust=jEve->clusterV[camId].size();
  printf(" M3CcdClusterMaker::measureClusters nCl=%d   start..\n",numClust);

  if(numClust<=0) return nCluOK;
  hA[1]->Fill("hasClust",1.);
  
  //............................. START ............
  for(int k=0;k<numClust; k++){
    M3ImageCluster &cluster= jEve->clusterV[camId][k];
    if( cluster.type<0 ) continue; //  already discarded
    int nBodyCell=cluster.nBody;
    int clustId=k; // for clarity
    if(cluster.cellV.size() < cut_cellCountL) {
      cluster.type=M3ImageCluster::cluTiny;
      continue;
    }
    hA[0]->Fill("nCell",1.);

    double diagMM=cluster.getDiagonal_mm();
    double ltDens=cluster.light/diagMM;

    hA[9]->Fill(diagMM);
    if(diagMM <cut_clustDiagL) {
      cluster.type=M3ImageCluster::cluDiag;
      continue; // cccccccc   C U T   ccccccccccc
    }


    hA[0]->Fill("diag",1.);
   

    hA[15]->Fill(diagMM, sqrt(cluster.cellV.size()));
      
    double xs=0, ys=0, lts=0;
    for(int m=0; m<nBodyCell; m++){     // CM computation 
      M3ImageCell &cell=cluster.cellV[m];
      float chX=cell.jx;
      float chY=cell.jy;
      float  lt=cell.light; 
      lts+=lt;
      xs+=chX*lt;
      ys+=chY*lt;
    }

    float cmJx=xs/lts;
    float cmJy=ys/lts;

    float xPixCM=cmJx*agregRebin;
    float yPixCM=cmJy*agregRebin;

    // fiducial cut of requested
    if(par_fiduBandPix) {
      if( !isFiducial(xPixCM,yPixCM) ) {
	cluster.type=M3ImageCluster::cluFidu;
	continue; // cccccccc   C U T   ccccccccccc	
      }
      // for accepted there are 2 options
      if(par_fiduBandPix>0) hA[0]->Fill("fiduIn",1.);
      else  hA[0]->Fill("fiduOut",1.);

    }


    double logLt=cluster.log10Light();
    hA[11]->Fill(logLt);
    if(cluster.light < cut_clustLightL ){
      cluster.type=M3ImageCluster::cluLight;
      continue; // cccccccc   C U T   ccccccccccc
    }
    hA[0]->Fill("light",1.);

    
    if(dbg>0) printf(" clust CM: %.1f, %.1f (pix)  Light=%.1f nCell=%d\n",cmJx*agregRebin,cmJy*agregRebin,lts,cluster.cellV.size());

    ( (TH2D*)hA[10])->Fill(xPixCM,yPixCM,lts);


    // drop if cam3 and at 500,500 where pi-spark shows up
    
    double dxPix=(xPixCM-500)/150., dyPix=(yPixCM-1500)/150.;
    double dx2=dxPix*dxPix+dyPix*dyPix;

    if(dx2<1. && camId==3){ // cam3 there is often a spark at this region
      cluster.type=M3ImageCluster::cluCam3Edge;
      continue; // cccccccc   C U T   ccccccccccc
    }
    hA[0]->Fill("no3Edge",1.);


    cluster.cmJx=cmJx;
    cluster.cmJy=cmJy;

    double Ixx=0, Iyy=0, Ixy=0;   
    for(int m=0; m<nBodyCell; m++){  // Principle axis of inertia (PCA)
      M3ImageCell &cell=cluster.cellV[m];
      float chX=cell.jx-cmJx;
      float chY=cell.jy-cmJy;
      float lt=cell.light;
      Ixx+=chX*chX*lt;
      Iyy+=chY*chY*lt;
      Ixy+=chX*chY*lt;
    }// end of m-loop
    
    //    printf("moments: Ixx=%e Iyy=%e, Ixy=%e\n", Ixx,Iyy,Ixy);
    if(fabs(Ixx/Iyy -1.) <1e-3) {
      printf("WARN, perfecty round cluster, strange: Ixx=%f Iyy=%f, Ixy=%f\n", Ixx,Iyy,Ixy);
    }

    /* there are 2 solutions fo alpha, for us this  transormation 
       applied to x-axis will alling it with 'longer' direction o fthe cluster
    */
    double tg2al=2.*Ixy/(Iyy-Ixx);
    double alpha=0.5*atan(tg2al); // rad

    if(Iyy>Ixx ) { 
      alpha=3.1416/2.-alpha; 
    } else  { 
      alpha=-alpha;
    }
    // shift angle to be  in range [0,pi/2]
    if(alpha<0 ) alpha+=3.1416;
    if(alpha>3.1416 ) alpha-=3.1416;



    double aa=(Ixx-Iyy)/2.;
    double delta=aa*aa +Ixy*Ixy;
    double sdelta=sqrt(delta);
    double Imin= (Ixx+Iyy)/2. -sdelta;
    double Imax= (Ixx+Iyy)/2. +sdelta;
    double Iratio=Imin/Imax;
 
    hA[14]->Fill( log10(cluster.light), log10(Imin));
    hA[12]->Fill(  Iratio);

    if(dbg>0) printf("Primary moments clId=%d: Imin=%.2e Imax=%.2e alpha/deg=%.1f aa=%e delta=%e \n",clustId,Imin, Imax,alpha/3.1416*180,aa,delta);

    if(Iratio >cut_pcaRatioH) {
      cluster.type=M3ImageCluster::cluPca;
      continue; // cccccccc   C U T   ccccccccccc
    }
    hA[0]->Fill("pcaR",1.);

    hA[18]->Fill(cluster.light/1000.,cluster.getDiagonal_mm());
    if(cut_factHadrRec>0) { // optional cut
      if( cluster.getDiagonal_mm() < cluster.light/cut_factHadrRec ) {
	cluster.type=M3ImageCluster::cluHadrRec;
	continue; // cccccccc   C U T   ccccccccccc
      }
      hA[0]->Fill("isHR",1.);
    }

    hA[0]->Fill("isGood",1.);
    
    cluster.Imin=Imin;
    cluster.Imax=Imax;
    cluster.Ialpha=alpha;

    hA[13]->Fill( log10(Imax),  log10(Imin));
    hA[17]->Fill(cluster.light);
    hA[20]->Fill(cluster.Ialpha);
    hA[21]->Fill(cluster.light/1000.,cluster.Ialpha);
    hA[22]->Fill(cluster.getDiagonal_mm(),cluster.Ialpha);

    nCluOK++;
    abortFrame=frmPromise; 

    if(fdCSV) fprintf(fdCSV,"%d , %d, clust-%d , %.3e, %.1f, %.0f, %.0f, %.3e, %.3e, %.3f\n",jEve->frameId,camId,clustId,lts,cluster.getDiagonal_mm(),xPixCM,yPixCM,Imin,Imax,alpha);


    if(dbg>1 && nBodyCell<10) cluster.print(0);
  }// end of k-loop
  
  if(nCluOK>0) hA[1]->Fill("goodClust",1.);
  if(nCluOK>1) hA[1]->Fill("manyGood",1.);

  if ( abortFrame==frmPromise) nPromise++; 
  return nCluOK;
}
 

//=======================================
//=======================================
int dmtpc::ped::M3CcdClusterMaker::findSeedClusters(){
  clear();
  int nCluOK=0;
  assert(jEve);

  assert(h2Dlight);
  assert(hPedStat);
  double totLight=h2Dlight->Integral();

  printf("% ClusterMaker::findSeedClusters() start.. frameId=%d  totLight=%.3e\n",coreName.Data(),jEve->frameId,totLight); 

 
  hA[1]->Fill("inp",1.);  

  int nchX=hPedStat->GetNbinsX();
  int nchY=hPedStat->GetNbinsY(); 

  assert(nchX==h2Dlight->GetNbinsX());  // you never know what may go wrong

  hA[4]->Fill(totLight/1000.);
 
  // detect patologoical frames: star-spark
  if(0) // no-spark2
  if (totLight > par_starSparkLight ) {
    abortFrame=frmStarSpark;
    hA[1]->Fill("starSpark",1.);  // kkkkkk  K I L L    E V E N T  kkkkkkk
    listStarSpark+=Form("%d,",jEve->frameId);
    if(fdCSV) fprintf(fdCSV,"%d , %d, star-spark , %3e \n",jEve->frameId,camId,totLight);
    return -1;  
  }

  double sumClustLight=0;
  printf(" (A) start cluster vector on %d^2 image  totLight=%.2e (ADU)\n",nchX,totLight);
 

  for(int jchX=1; jchX<=nchX; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N
      int mBin=hPedStat->GetBin(jchX, jchY);  //   convert 2D to 1D index

      if (  hPedStat->GetBinContent(mBin) ) continue; // drop masked channels
      if (  hUtil->GetBinContent(mBin) ) continue; // drop used pixels
      float rms=h2Dlight->GetBinError(mBin); 
      float val=h2Dlight->GetBinContent(mBin); 
      if (val <  rms*par_nSigThr2 ) continue;


      //      printf("aa jchX=%d jchY=%d  xPix=%f\n", jchX,jchY,xPix);
      // now pixel has enough energy to trigger creation of new cluster
      sumClustLight+=createCluster(jchX,jchY);
      if(abortFrame) return -2;  // cccccccc   C U T   ccccccccccc
    }
  //off  h2Dlight->Draw("colz");  h2Dlight->SetMinimum(10);  h2Dlight->SetMaximum(50); return -1;
  printf("(A) FRAME=%d end, numClust=%d  sumClustLight=%.3e (ADU)  frameLight=%.3e  CL/FL=%.2f\n",jEve->frameId, jEve->clusterV[camId].size(),sumClustLight,h2Dlight->Integral(),sumClustLight/h2Dlight->Integral());
  
  double logCL=TMath::Log10(sumClustLight);
  double logFL=TMath::Log10(h2Dlight->Integral());
  
  hA[8]->Fill(logCL,logFL);
  hA[0]->Fill("any",jEve->clusterV[camId].size());
  return jEve->clusterV[camId].size();

}

//=======================================
//=======================================
void dmtpc::ped::M3CcdClusterMaker::attachSkinToClusters(){
#if 0
  printf(" M3CcdClusterMaker:: attachSkinToClusters() start..\n"); 
  int nchX=hPedStat->GetNbinsX();
  int nchY=hPedStat->GetNbinsY(); 
  assert(nchX==h2Dlight->GetNbinsX());  // you never know what may go wrong

  double sumLight=0; // total from all skin cells
  int nCell=0;
  for(int k=0;k<clusterV.size(); k++){
    M3ImageCluster &cluster= clusterV[k];
    if( cluster.type<0 ) continue; //  already discarded
    int nBodyCell=cluster.nBody;
    int clustId=k; // for clarity
    for(int m=0; m<nBodyCell; m++){
      int mchX=cluster.cellV[m].jx;
      int mchY=cluster.cellV[m].jy;
    
      // printf(" clust=%d cell=%d of %d\n", k,m,nBodyCell);
      for(int ix=-1;ix<=1;ix++) // loopp over 3x3 around current cell
	for(int iy=-1;iy<=1;iy++) {
	  if(ix==0 && iy==0)continue; // skip itself
	  
	int kchX=mchX+ix; // absolute X location
	int kchY=mchY+iy; // absolute Y location

	if (kchX<=0) continue; // left edge
	if (kchX>nchX) continue; // right edge

	if (kchY<=0) continue; // bottom edge
	if (kchY>nchY) continue; // top edge

	int mBin=hPedStat->GetBin(kchX, kchY);  //   convert 2D to 1D index
	if (  hUtil->GetBinContent(mBin) ) continue; // ignore used cells
	// add any not-used cell, even masked cells (which have energy 0, but complete topology)
	sumLight+=addCell( kchX,kchY, &cluster, M3ImageCell::celSkin,clustId);
	nCell++;
	}// end of 3x3 double loop
    }// end of cell m-loop, cluster is done
    cluster.nSkin=cluster.cellV.size()-cluster.nBody;
  }// end of cluster k-loop
  // printf("(B) end, total numSkinCell=%d  sumSkinLight=%.1f (kADU)\n",nCell ,sumLight/1000.);
#endif
}

//=======================================
//=======================================
double dmtpc::ped::M3CcdClusterMaker::addCell(int jxi, int jyi, M3ImageCluster *cluster,  int cellType, int clustId) {
  
  //M3ImageCell::CellType type
  M3ImageCell cell;
  cell.jx=jxi;
  cell.jy=jyi;
  cell.light=h2Dlight->GetBinContent(jxi,jyi);
  cell.sigLight=h2Dlight->GetBinError(jxi,jyi);
  cluster->cellV.push_back(cell); 
  hUtil->SetBinContent(jxi,jyi,cellType);
  hUtil->SetBinError(jxi,jyi,clustId);

#if 0
  // just debugging below ...
  float x=h2Dlight->GetXaxis()->GetBinCenter(jxi);
  float y=h2Dlight->GetYaxis()->GetBinCenter(jyi);
  printf(" ADD Cell:  XY=(%.0f,%.0f)  cnstr\n",x,y);
  cell.print();
#endif

  return cell.light;
}

//=======================================
//=======================================
double dmtpc::ped::M3CcdClusterMaker::createCluster(int jchX, int jchY){

  // remember jchX,Y count 1 to N
  int nchX=hPedStat->GetNbinsX();
  int nchY=hPedStat->GetNbinsY(); 
  assert(nchY==nchX);
  

  // this calibration is only true for 4-shutter
  // float fullRebin=hPedStat->GetXaxis()->GetXmax()/nchX;
  float bin2mm= jEve->light[camId].bin2mm;
  
  M3ImageCluster cluster(bin2mm); // working container
  int clustId=jEve->clusterV[camId].size();
  // printf(" M3CcdClusterMaker::createCluster(%d) @ x=%0f, y=%.0f light=%.0f adu\n",clustId,x,y,light);

  // bounding-box
  int  jxL=9999, jxH=-jxL, jyL=jxL, jyH=jxH; // limits of cluster (diagonal)

  // seed cluster w/ trigger cell 
  double sumLight=addCell( jchX,jchY, &cluster, M3ImageCell::celTrig,clustId);

  for(int ic=0;ic<cluster.cellV.size(); ic++) {
    
    // printf("\ncoalescent more cells around seed cell=%d of %d\n", ic ,cluster.cellV.size());
    //    cluster.cellV[ic].print();
    int mchX=cluster.cellV[ic].jx;
    int mchY=cluster.cellV[ic].jy;
    if( jxL > mchX) jxL=mchX;
    if( jxH < mchX) jxH=mchX;
    if( jyL > mchY) jyL=mchY;
    if( jyH < mchY) jyH=mchY;
      
    for(int ix=-1;ix<=1;ix++) // loopp over 3x3 around current cell
      for(int iy=-1;iy<=1;iy++) {
	if(ix==0 && iy==0){	  
	  //  printf("cell cnt=%d  uVal=%.0f\n",cluster.cellV.size(), hUtil->GetBinContent(jchX,jchY));
	  continue; // that is all for current cell
	}
	int kchX=mchX+ix; // absolute X location
	int kchY=mchY+iy; // absolute Y location

	if (kchX<=0) continue; // left edge
	if (kchX>nchX) continue; // right edge

	if (kchY<=0) continue; // bottom edge
	if (kchY>nchY) continue; // top edge

	int mBin=hPedStat->GetBin(kchX, kchY);  //   convert 2D to 1D index
	if (  hUtil->GetBinContent(mBin) ) continue; // ignore used cells
	float val=h2Dlight->GetBinContent(mBin); 
	float rms=h2Dlight->GetBinError(mBin); 
	if (val <  rms*par_nSigThr1 ) continue;

	//printf(" bb ix=%d iy=%d   kx=%d ky=%d val=%.0f  thr1=%.0f\n",ix,iy,kchX,kchY,val, hPedThr1->GetBinContent(mBin));
	if (  hPedStat->GetBinContent(mBin) ) continue; // ignore masked channels
	// now pixel has enough energy to add to a cluster
	sumLight+=addCell( kchX,kchY, &cluster, M3ImageCell::celBody,clustId);
	
      }// END OF  3x3 block
  }// end of cluster growth
  
      
  cluster.nBody=cluster.cellV.size();// record it, before skin is added
  cluster.jxL=jxL;
  cluster.jyL=jyL;
  cluster.jxH=jxH;
  cluster.jyH=jyH;
  cluster.light=sumLight;
  // cluster.print();

  // monitoring & QA
  double logNc=cluster.log10CellCount();
  double logLt=cluster.log10Light();

  hA[5]->Fill(logNc);
  hA[7]->Fill(logNc,logLt);
    
  jEve->clusterV[camId].push_back(cluster);
  return sumLight;

  return 0;
}


//=======================================
//=======================================
void dmtpc::ped::M3CcdClusterMaker::bestTrack(){
  assert(h2Dlight);
  int agregRebin=jEve->light[camId].agregRebin;

  printf("%s ClusterMaker::bestTrack() start.. frameId=%d frameType=%d\n",coreName.Data(),jEve->frameId, abortFrame); 
  
  assert( abortFrame==frmPromise );
  float clustLight=0;
  int  bestClustId=-1;
  for(int k=0;k<jEve->clusterV[camId].size(); k++){
    M3ImageCluster &cluster= jEve->clusterV[camId][k];
    if( cluster.type<0 ) continue; //  already discarded
    if(clustLight>cluster.light ) continue;
    bestClustId=k;
    clustLight=cluster.light ;
  }

  printf("best cluster ID=%d, light=%.1f adu dump:",bestClustId,clustLight); 
  M3ImageCluster &cluster= jEve->clusterV[camId][bestClustId];
  cluster.print();
  
  TCanvas *can2=new TCanvas();
  can2->cd();
  h2DlightRot->Draw("colz");  

  double xClustPix=cluster.cmJx*agregRebin; 
  double yClustPix=cluster.cmJy*agregRebin; 
  double theta=cluster.Ialpha; //rad
  double cth=cos(theta),sth=sin(theta);
  double uOffPix=jEve->light[camId].pixOffsetRot, vOffPix=uOffPix;


  // shift & rotate light to center it on the cluster
  int nchX=hPedStat->GetNbinsX();
  int nchY=hPedStat->GetNbinsY(); 
  for(int jchX=1; jchX<=nchX; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N
      int mBin=hPedStat->GetBin(jchX, jchY);  //   convert 2D to 1D index
      if (  hPedStat->GetBinContent(mBin) ) continue; // drop masked channels
      float rms=h2Dlight->GetBinError(mBin); 
      float val=h2Dlight->GetBinContent(mBin); 
      assert(rms>5);// (adu) sth went wrong
      
      // transformx,y coordinates
      double xPix=h2Dlight->GetXaxis()->GetBinCenter(jchX) -xClustPix;
      double yPix=h2Dlight->GetYaxis()->GetBinCenter(jchY) -yClustPix;
 
      double uPix=xPix*cth + yPix*sth  +uOffPix;
      double vPix=-xPix*sth + yPix*cth +vOffPix;
 
      // find kBin in target histo
      int jchU=h2DlightRot->GetXaxis()->FindBin(uPix);
      int jchV=h2DlightRot->GetYaxis()->FindBin(vPix);
      double oldVal=h2DlightRot->GetBinContent(jchU,jchV);
      double oldErr=h2DlightRot->GetBinError(jchU,jchV);
      double newVal=oldVal+val; // assume errors are very simular
      double newErr=sqrt(oldErr*oldErr + rms*rms);  
      h2DlightRot->SetBinContent(jchU,jchV,newVal);
      h2DlightRot->SetBinError(jchU,jchV,newErr);
    }
      
  gPad->SetGrid();

}


  
  
