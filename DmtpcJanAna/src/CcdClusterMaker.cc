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

#include "CcdClusterMaker.hh" 
#include "CcdCalibMaker.hh" 
#include "M3Event.hh" 

ClassImp(dmtpc::jana::CcdClusterMaker); 


//=======================================
//=======================================
dmtpc::jana::CcdClusterMaker::CcdClusterMaker() {
  // printf(" CcdClusterMaker() cnstr\n"); 
  m3Eve=0;  ccd=0;
  setDebug(0);
  can=0;
  fdCSV=0;
  pdfName="noPDF";
  setNsigThres12(1,2);   
  par_fiduBandPix=0; // no fiducial cut

  // PCA phi orientation
  //  cut_pcaPhi1=1.48;  cut_pcaPhi2=1.66; // ~90deg
  cut_pcaPhi1=-0.1;  cut_pcaPhi2=3.2;  // wide open

  par_bin2mm_mesh_1x1=4; // 1 bin=250 um
  par_bin2um_sensor_1x1=1./12.; // never changes
  // fixed cuts
  // par_starSparkLight=1e6;
  par_piSparkLtDens=100; par_piSparkDiag=110; 
  par_piSparkNcell=440; // for 1x1, rescaled w/ agregRebin in initHisto
  listPiSpark="pi-spark frames ";
  setCuts(8,400,14,0.05,0.5);
  campaignT0=0;
}


 
//=======================================
//=======================================
void dmtpc::jana::CcdClusterMaker::clear() {
  
  hUtil->SetNameTitle(coreName+Form("_clUt_fr%d",m3Eve->frameId), Form("util clustFind info quad%d  frame=%d ",ccd->quadId,m3Eve->frameId));
  hUtil->Reset();
  
  if(can) can->Clear();
  if(dbg>0) printf(" CcdClusterMaker::clear() done\n"); 
  //  if(dbg>0) m3Eve->print(0);
}



//=======================================
//=======================================
void dmtpc::jana::CcdClusterMaker::purgeClusters(){ 
  int quadId=ccd->quadId;
  // for now only discard cell-list from not interesting clusters
  // note, this prohibits merging of some  short clusters to long clusters
  for(int k=0;k<m3Eve->clusterV[quadId].size(); k++){
    CcdCluster &cluster= m3Eve->clusterV[quadId][k];
    if( cluster.type<0 ) cluster.cellV.clear();  // size reduction: 45/68
    //if( cluster.type<0 ) cluster.clear(); // size reduction : 35/68
  }
}

//=======================================
void dmtpc::jana::CcdClusterMaker::display() {

  double pedRms=ccd->avrPedRms;
  int agregRebin=ccd->agregRebin;
  int quadId=ccd->quadId;

  ccd->hLight->SetMinimum(pedRms);
  ccd->hLight->SetMaximum(4*pedRms);

  //1  ccd->hLightRot->SetMinimum(pedRms);
  //1 ccd->hLightRot->SetMaximum(4*pedRms);

  ccd->hLight->Draw("colz");

  printf("display good clusters from nCl=%d\n",m3Eve->clusterV[quadId].size());
  double x0c=600,y0c=600, maxVal=0;
  double diag1=0, light1=0;
  for(int k=0;k<m3Eve->clusterV[quadId].size(); k++){
    CcdCluster &cluster= m3Eve->clusterV[quadId][k];
    if( cluster.type<0 ) continue; //  already discarded
    if(dbg>0) cluster.print(0);
    // revert position to full-rang CCD bins
    double  x0=cluster.kxBinCM*agregRebin;
    double  y0=cluster.kyBinCM*agregRebin;
    double phiDeg=cluster.pcaPhi/3.1416*180.;
    double R1=0.5*cluster.getDiagonal_bin()*agregRebin;

    double beta=30;
    TEllipse *el=new TEllipse(x0,y0,R1,R1/2.5,beta,360-beta,phiDeg);
    el->SetFillStyle(0); el->SetLineColor(kMagenta);
    el->Draw();

    // draw label
    TString txL= Form("L=%.0f kADU",cluster.light/1000.);
    if (cluster.light<1800) txL= Form("L=%.0f ADU",cluster.light);
    double offY=10*(sin(cluster.pcaPhi) -cos(cluster.pcaPhi) );    
    double offX=10*(cos(cluster.pcaPhi) + sin(cluster.pcaPhi));
    TText *tt=new TText( x0+offX, y0+offY,txL);
    tt->Draw();
    tt->SetTextSize(0.03);   tt->SetTextColor(kMagenta);
    tt->SetTextAngle(phiDeg-90);

    // ..... populate all used cells in given cluster
    for(int ic=0;ic<cluster.cellV.size();ic++) {
      CcdCell &cell=cluster.cellV[ic];
      float xPix=cell.kxBin*agregRebin;
      float yPix=cell.kyBin*agregRebin;
      hA[16]->Fill(xPix,yPix);
      hA[29]->Fill( cell.light/1000.);
    }


    //......  draw extrapolation lines to the source
    float x0M3=2800, y0M3=2800;
    if(quadId==2) { x0M3=300;  y0M3=2800;}
    //cam0, cam1 - not implemented
    float x0Hall= (x0 -x0M3) /40.;// in cm
    float y0Hall= (y0 -y0M3) /40.;// in cm
    int max_cm=200;
    for(int u=-max_cm; u<max_cm; u+=2 ) {     
      float x,y;
      if(fabs( cluster.pcaPhi -1.7) > 0.8 ) { // ~horizontal track
	x=u+x0Hall;
	y=y0Hall + (x-x0Hall) * tan( cluster.pcaPhi );
      } else { // ~ vertical track
	y=u+y0Hall;
	x=x0Hall + (y-y0Hall) * tan( 3.1416/2.-cluster.pcaPhi );
      }
      hA[19]->Fill(x,y);
    }
 

    // tag  good cluster with most  of ... sth
    if(maxVal> cluster.cellV.size()) continue;
    maxVal=cluster.cellV.size();
    //if(maxVal> cluster.light) continue;
    //maxVal=cluster.light;
    x0c=x0; y0c=y0;
    diag1=cluster.getDiagonal_bin() * m3Eve->agregRebin;
    light1=cluster.light;
    //cluster.print();
    //printf("WWW agrb %d %d   %f\n",ccd->agregRebin, m3Eve->agregRebin, cluster.getDiagonal_bin());
  }// end of cluster loop
  
  

  //  if(can)  can->Print( pdfName,"pdf"); // full frame
 

#if 0 // test fiducial mask
  // REMEMBER to disable cluster evaluation
  int nchX=ccd->hPedStat->GetNbinsX();
  int nchY=ccd->hPedStat->GetNbinsY(); 
  
  for(int jchX=1; jchX<=nchX; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N
      float xPix=ccd->hLight->GetXaxis()->GetBinCenter(jchX);
      float yPix=ccd->hLight->GetYaxis()->GetBinCenter(jchY);
      // if( !isFiducial(xPix,yPix) ) ccd->hLight->SetBinContent(jchX,jchY,0);
    }

  return;
#endif

  float del=55*agregRebin;
  if(del<diag1) del=diag1;
  hUtil->Draw("text same");


  ccd->hLight->SetAxisRange(x0c-del,x0c+del,"x");
  ccd->hLight->SetAxisRange(y0c-del,y0c+del,"y");
  
  //....... draw scale for mesh (mm)
  {
  float bar_mm=5*agregRebin;
  float pixLen=bar_mm*par_bin2mm_mesh_1x1;
  float x0l=x0c-del+1.5, y0l=y0c-del+1.5;
  if( x0l<0) x0l=0;
  if( y0l<0) y0l=0;
  TBox *box=new TBox( x0l,  y0l,  x0l+pixLen, y0l+pixLen/4);
  box->Draw();
  box->SetFillStyle(0); box->SetLineColor(kRed);
  TText *tt=new TText( x0l+3*agregRebin, y0l+pixLen/12., Form("%.0f mm",bar_mm));
  tt->Draw();  tt->SetTextSize(0.03);   tt->SetTextColor(kRed);
  }
  //....... draw scale for mesh (mm)
  {
  float bar_um=200*agregRebin;
  float pixLen=bar_um*par_bin2um_sensor_1x1;
  float x0l=x0c+del-30*agregRebin, y0l=y0c-del+1.5;
  if( x0l<0) x0l=5;
  if( y0l>3000) y0l=3000;
  TBox *box=new TBox( x0l,  y0l,  x0l+pixLen, y0l+pixLen/3);
  box->Draw();
  box->SetFillStyle(0); box->SetLineColor(kRed);
  TText *tt=new TText( x0l+3*agregRebin, y0l+pixLen/8., Form("%.0f um",bar_um));
  tt->Draw();  tt->SetTextSize(0.03);   tt->SetTextColor(kRed);
  }


  if(can)  can->Print( pdfName,"pdf");

  // activate to produce 1 png per cluster
  TString pngName=Form("out1/clust%.1f_R%d_fr%d_quad%d_reb%d.png",light1/1000.,ccd->runId,ccd->frameId,quadId, agregRebin);  can->Print( pngName);


}



//=======================================
//=======================================
int dmtpc::jana::CcdClusterMaker::hasPiSpark(){ // return 1 if spark found
  int quadId=ccd->quadId;
  int numClust=m3Eve->clusterV[quadId].size();
  if(dbg>1)  printf(" ClusterMaker::hasPiSpark() nCl=%d   start..\n",numClust);  
  if(numClust<=0) return 0;

  //............................. START ............
  
  for(int k=0;k<numClust; k++){
    CcdCluster &cluster= m3Eve->clusterV[quadId][k];
    int clustId=k; // for clarity
    double diagPix=cluster.getDiagonal_bin() * m3Eve->agregRebin;
    double ltDens=cluster.light/diagPix;
    int nCell=cluster.cellV.size();
    double ratD2C=diagPix/ m3Eve->agregRebin/nCell;

    if (nCell<10) continue; // pi-spark can't be that small ???

    if(dbg>0)printf("pi-spark-finder: clId=%d nCell=%d diag=%.1f (pix) L/diag=%.1f ratD2C=%.2f \n",clustId, nCell,diagPix,ltDens,ratD2C);

    // detect pathological frames: pi-spark
    //    if(cluster.cellV.size()>200) cluster.print(0);

    if ( (ltDens > par_piSparkLtDens && diagPix > par_piSparkDiag && ratD2C<0.5 ) 
	 //	|| (nCell>par_piSparkNcell && ratD2C<0.3)
	) {
      hA[1]->Fill("piSpark",1.);
      listPiSpark+=Form("%d,",m3Eve->frameId);
      if(fdCSV) fprintf(fdCSV,"%d , %d, pi-spark-%d , %.3e, %d, %.1f, , , , ,%d\n",m3Eve->frameId,quadId,clustId,cluster.light,cluster.cellV.size(),diagPix,m3Eve->runId);
      printf(" ClusterMaker::hasPiSpark_TRUE, abort frame %d  quadId=%d\n", m3Eve->frameId,quadId);
      cluster.print(0);
      return 1;  // does not matter 
    }
  }
  if(dbg>0)printf("pi-spark-finder: no-spark\n");
  return 0 ; // no spark
}



//=======================================
//=======================================
int dmtpc::jana::CcdClusterMaker::measureClusters(){
  int nCluOK=0;
  int agregRebin=ccd->agregRebin;
  int quadId=ccd->quadId;

  // computes: diagonal, moments of inertia (aka PCA)
  int numClust=m3Eve->clusterV[quadId].size();
  // printf(" CcdClusterMaker::measureClusters nCl=%d   start..\n",numClust);

  if(numClust<=0) return nCluOK;
  hA[1]->Fill("hasClust",1.);
  
  //............................. START ............
  for(int k=0;k<numClust; k++){
    CcdCluster &cluster= m3Eve->clusterV[quadId][k];
    if( cluster.type<0 ) continue; //  already discarded
    int nBodyCell=cluster.cellV.size();
    int clustId=k; // for clarity

    double logNc=log10(cluster.cellV.size());
    double logLt=log10(cluster.light);

    hA[5]->Fill(logNc);

    if( nBodyCell< cut_cellCountL) {
      cluster.type=CcdCluster::cluTiny;
      continue;  // cccccccc   C U T   ccccccccccc
    }
    hA[0]->Fill("nCell",1.);

    hA[7]->Fill(logNc,logLt);
  
#if 0 // selects low-ionization clusters typically pi-spark
    float cutVal= (logLt - 1.5 ) /(logNc );
    if( cutVal > 3./2.5 ){
      cluster.type=CcdCluster::cluTiny;
      continue;  // cccccccc   C U T   ccccccccccc
    }
    hA[0]->Fill("faint",1.);
#endif


    double diagPix=cluster.getDiagonal_bin() * m3Eve->agregRebin;
    double ltDens=cluster.light/diagPix;

    // if(diagPix>25) {printf("ZZ %f ",diagPix); cluster.print(1);}
    //    double ltDens=cluster.light/diagPix;

    hA[9]->Fill(diagPix);
    if(diagPix <cut_clustDiagPixL) {
      cluster.type=CcdCluster::cluDiag;
      continue; // cccccccc   C U T   ccccccccccc
    }
 
    hA[0]->Fill("diag",1.);
    hA[6]->Fill(diagPix,ltDens);   
    hA[15]->Fill(diagPix, sqrt(cluster.cellV.size()));
      
    double xs=0, ys=0, lts=0;
    double maxLt=0; // (adu) maximal light in a cell
    for(int m=0; m<nBodyCell; m++){     // CM computation 
      CcdCell &cell=cluster.cellV[m];
      float chX=cell.kxBin;
      float chY=cell.kyBin;
      float  lt=cell.light; 
      lts+=lt;
      xs+=chX*lt;
      ys+=chY*lt;
      if(maxLt<lt) maxLt=lt;
    }
    assert(maxLt>0);
    
    float cmJx=xs/lts;
    float cmJy=ys/lts;

    float xPixCM=cmJx*agregRebin;
    float yPixCM=cmJy*agregRebin;


    ( (TH2D*)hA[10])->Fill(xPixCM,yPixCM,lts);

    // fiducial cut of requested
    if(par_fiduBandPix) {
      if( !isFiducial(xPixCM,yPixCM) ) {
	cluster.type=CcdCluster::cluFidu;
	continue; // cccccccc   C U T   ccccccccccc	
      }
      // for accepted there are 2 options
      if(par_fiduBandPix>0) hA[0]->Fill("fiduIn",1.);
      else  hA[0]->Fill("fiduOut",1.);

    }


    hA[11]->Fill(logLt);
    if(cluster.light < cut_clustLightL ){
      cluster.type=CcdCluster::cluLight;
      continue; // cccccccc   C U T   ccccccccccc
    }

    hA[0]->Fill("light",1.);

    
    if(dbg>0) printf(" clust CM: %.1f, %.1f (pix)  Light=%.1f nCell=%d\n",cmJx*agregRebin,cmJy*agregRebin,lts,cluster.cellV.size());


#if 1
    // drop if cam3 and at 500,500 where pi-spark shows up
    double dxPix=(xPixCM-500)/150., dyPix=(yPixCM-1500)/150.;
    double dx2=dxPix*dxPix+dyPix*dyPix;
    if(dx2<1. && quadId==3){ // cam3 there is often a spark at this region
      cluster.type=CcdCluster::cluCam3Edge;
      hA[0]->Fill("kill3Edge",1.);
      continue; // cccccccc   C U T   ccccccccccc
    }
    hA[0]->Fill("no3Edge",1.);
#endif

    cluster.kxBinCM=cmJx;
    cluster.kyBinCM=cmJy;

    hA[18]->Fill(cluster.light/1000.,diagPix);

    if(diagPix <cluster.light/50.) { // set B: low-light density
    //if(diagPix >cluster.light/50. || cluster.light>9500) { // set A: intese, short camera-interactions
      cluster.type=CcdCluster::cluPca;
      continue; // cccccccc   C U T   ccccccccccc
    }

    hA[0]->Fill("Ldens",1.);

    double Ixx=0, Iyy=0, Ixy=0, slt2=0;   
    double avrLt=cluster.light/nBodyCell; // (adu)
    // Principle axis of inertia (PCA)  & RMS of light    
    for(int m=0; m<nBodyCell; m++){  
      CcdCell &cell=cluster.cellV[m];
      float chX=cell.kxBin-cmJx;
      float chY=cell.kyBin-cmJy;
      float lt=cell.light;
      float dlt=lt-avrLt;
      Ixx+=chX*chX*lt;
      Iyy+=chY*chY*lt;
      Ixy+=chX*chY*lt;
      slt2+=dlt*dlt;
    }// end of m-loop

    double rmsLt=sqrt(slt2/nBodyCell); // adu

    //    printf("moments: Ixx=%e Iyy=%e, Ixy=%e\n", Ixx,Iyy,Ixy);
    if(fabs(Ixx/Iyy -1.) <1e-3) {
      printf("WARN, perfecty round cluster, strange: Ixx=%f Iyy=%f, Ixy=%f\n", Ixx,Iyy,Ixy);
    }

    /* there are 2 solutions for phi, for us this  transormation 
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

    cluster.pcaMin=Imin;
    cluster.pcaMax=Imax;
    cluster.pcaPhi=alpha;
    
    hA[14]->Fill( log10(cluster.light), log10(Imin));
    hA[12]->Fill(  Iratio);

    if(dbg>0) printf("Primary moments clId=%d: Imin=%.2e Imax=%.2e phi/deg=%.1f aa=%e delta=%e \n",clustId,Imin, Imax,alpha/3.1416*180,aa,delta);

    if(Iratio < cut_pcaRatioL || Iratio > cut_pcaRatioH) {
      cluster.type=CcdCluster::cluPca;
      continue; // cccccccc   C U T   ccccccccccc
    }
    hA[0]->Fill("pcaR",1.);

    hA[20]->Fill(cluster.pcaPhi);
    hA[22]->Fill(diagPix,cluster.pcaPhi);
    
#if 0
    if(cluster.pcaPhi < cut_pcaPhi1 || cluster.pcaPhi > cut_pcaPhi2) { //
      cluster.type=CcdCluster::cluPca;
      continue; // cccccccc   C U T   ccccccccccc
    }
#endif
    hA[0]->Fill("pcaPhi",1.);

    hA[0]->Fill("isGood",1.);
    

    hA[13]->Fill( log10(Imax),  log10(Imin));
    hA[17]->Fill(cluster.light/1000.);

    hA[21]->Fill(cluster.light/1000.,cluster.pcaPhi);
    hA[23]->Fill(logNc,logLt);
    hA[24]->Fill(diagPix,ltDens);
    double cluWidthPix=sqrt(cluster.pcaMin/cluster.light) * m3Eve->agregRebin;
    double cluSpreadPix=sqrt(cluster.pcaMax/cluster.light)* m3Eve->agregRebin;
    hA[25]->Fill(diagPix,cluWidthPix);
    hA[26]->Fill(diagPix,cluSpreadPix);
    hA[27]->Fill(rmsLt/cluster.light,maxLt/cluster.light);
    hA[28]->Fill(diagPix);
    float wallHours=m3Eve->secondsSince(campaignT0)/3600.;// T0=March 3 noon EDT
    hA[3]->Fill(wallHours); //  good cluster vs. time

    nCluOK++;

    if(fdCSV) fprintf(fdCSV,"%d , %d, clust-%d , %.3e, %d, %.1f, %.0f, %.0f, %.3e, %.3e, %.3f, %d\n",m3Eve->frameId,quadId,clustId,lts,cluster.cellV.size(),diagPix,xPixCM,yPixCM,Imin,Imax,alpha,m3Eve->runId);

    if(dbg>1 && nBodyCell<10) cluster.print(0);
  }// end of k-loop
  
  if(nCluOK>0) hA[1]->Fill("goodClust",1.);
  if(nCluOK>1) hA[1]->Fill("manyGood",1.);


  return nCluOK;
}



//=======================================
//=======================================
int dmtpc::jana::CcdClusterMaker::findSeedClusters(){

  int quadId=ccd->quadId;
  int nCluOK=0;
  assert(m3Eve);
  m3Eve->assert_quadId(quadId);
  assert(ccd->hLight);
  assert(ccd->hPedStat);

  // write frameID but only once
  if (m3Eve->frameId<0) m3Eve->frameId=ccd->frameId;
  else  assert(m3Eve->frameId==ccd->frameId);
  m3Eve->timeStamp=ccd->timeStamp;

  clear(); // needs frameId to name util histo - not essential

  double totLight=ccd->hLight->Integral();
  printf("% ClusterMaker::findSeedClusters() start.. frameId=%d  totLight=%.3e\n",coreName.Data(),m3Eve->frameId,totLight); 

 
  hA[1]->Fill("inpFrm",1.);  
  hA[1]->Fill("expoHour",ccd->exposureTime/1000./3600.); // from msec to hours


  float wallHours=m3Eve->secondsSince(campaignT0)/3600.;// T0=March 3 noon EDT
  hA[2]->Fill(wallHours,ccd->exposureTime/1000./3600.); // from msec to hours

  int nchX=ccd->hPedStat->GetNbinsX();
  int nchY=ccd->hPedStat->GetNbinsY(); 

  assert(nchX==ccd->hLight->GetNbinsX());  // you never know what may go wrong

  double sumClustLight=0;
  printf(" (A) FRAME=%d start cluster vector on %d^2 image  totLight=%.2e (ADU)\n",ccd->frameId,nchX,totLight);
 

  for(int jchX=1; jchX<=nchX; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N
      int mBin=ccd->hPedStat->GetBin(jchX, jchY);  //   convert 2D to 1D index

      if (  ccd->hPedStat->GetBinContent(mBin) ) continue; // drop masked channels
      if (  hUtil->GetBinContent(mBin) ) continue; // drop used pixels
      float rms=ccd->hLight->GetBinError(mBin); 
      float val=ccd->hLight->GetBinContent(mBin); 
      if (val <  rms*par_nSigThr2 ) continue;
      //   printf("aa jchX=%d jchY=%d  xPix=%f\n", jchX,jchY,xPix);
      // now pixel has enough energy to trigger creation of new cluster
      sumClustLight+=createCluster(jchX,jchY);
    }
  
  //off  ccd->hLight->Draw("colz");  ccd->hLight->SetMinimum(10);  ccd->hLight->SetMaximum(50); return -1;

  float sumFrL=ccd->hLight->Integral();

  //  printf("(A) FRAME=%d end, numClust=%d  sumClustLight=%.3e (ADU)  frameLight=%.3e  sumCL/sumFL=%.2f\n",m3Eve->frameId, m3Eve->clusterV[quadId].size(),sumClustLight,sumFrL,sumClustLight/sumFrL);
  
  double logCL=TMath::Log10(sumClustLight);
  double logFL=TMath::Log10(sumFrL);
  
  hA[8]->Fill(logCL,logFL);
  hA[0]->Fill("any",m3Eve->clusterV[quadId].size());
  return m3Eve->clusterV[quadId].size();

}



//=======================================
//=======================================
double dmtpc::jana::CcdClusterMaker::addCell(int jxi, int jyi, CcdCluster *cluster,  int cellType, int clustId) {
  
  //M3ImageCell::CellType type
  CcdCell cell;
  cell.kxBin=jxi;
  cell.kyBin=jyi;
  cell.light=ccd->hLight->GetBinContent(jxi,jyi);
  cell.sigLight=ccd->hLight->GetBinError(jxi,jyi);
  cluster->cellV.push_back(cell); 
  hUtil->SetBinContent(jxi,jyi,cellType);
  hUtil->SetBinError(jxi,jyi,clustId);

#if 0
  // just debugging below ...
  float x=ccd->hLight->GetXaxis()->GetBinCenter(jxi);
  float y=ccd->hLight->GetYaxis()->GetBinCenter(jyi);
  printf(" ADD Cell:  XY=(%.0f,%.0f)  cnstr\n",x,y);
  cell.print();
#endif

  return cell.light;
}

//=======================================
//=======================================
double dmtpc::jana::CcdClusterMaker::createCluster(int jchX, int jchY){

  // remember jchX,Y count 1 to N
  int nchX=ccd->hPedStat->GetNbinsX();
  int nchY=ccd->hPedStat->GetNbinsY(); 
  int quadId=ccd->quadId;

  // this calibration is only true for 4-shutter
  // float fullRebin=hPedStat->GetXaxis()->GetXmax()/nchX;
  //  float bin2mm= m3Eve->light[quadId].bin2mm;
  
  CcdCluster cluster; // working container
  int clustId=m3Eve->clusterV[quadId].size();
  // printf(" CcdClusterMaker::createCluster(%d) @ x=%0f, y=%.0f light=%.0f adu\n",clustId,x,y,light);

  // bounding-box
  int  jxL=9999, jxH=-jxL, jyL=jxL, jyH=jxH; // limits of cluster (diagonal)

  // seed cluster w/ trigger cell 
  double sumLight=addCell( jchX,jchY, &cluster, CcdCell::celTrig,clustId);

  // here cluster is being grown using bredth-firts scheme
  for(int ic=0;ic<cluster.cellV.size(); ic++) {
    
    // printf("\ncoalescent more cells around seed cell=%d of %d\n", ic ,cluster.cellV.size());
    //    cluster.cellV[ic].print();
    int mchX=cluster.cellV[ic].kxBin;
    int mchY=cluster.cellV[ic].kyBin;
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

	int mBin=ccd->hPedStat->GetBin(kchX, kchY);  //   convert 2D to 1D index
	if (  hUtil->GetBinContent(mBin) ) continue; // ignore used cells
	float val=ccd->hLight->GetBinContent(mBin); 
	float rms=ccd->hLight->GetBinError(mBin); 
	if (val <  rms*par_nSigThr1 ) continue;

	//printf(" bb ix=%d iy=%d   kx=%d ky=%d val=%.0f  thr1=%.0f\n",ix,iy,kchX,kchY,val, hPedThr1->GetBinContent(mBin));
	if (  ccd->hPedStat->GetBinContent(mBin) ) continue; // ignore masked channels
	// now pixel has enough energy to add to a cluster
	sumLight+=addCell( kchX,kchY, &cluster, CcdCell::celBody,clustId);
	
      }// END OF  3x3 block
  }// end of cluster growth
  

  cluster.kxBinL=jxL;
  cluster.kyBinL=jyL;
  cluster.kxBinH=jxH;
  cluster.kyBinH=jyH;
  cluster.light=sumLight;
  // cluster.print();
    
  m3Eve->clusterV[quadId].push_back(cluster);
  return sumLight;

  return 0;
}

#if 0
//=======================================
//=======================================
void dmtpc::jana::CcdClusterMaker::bestTrack(){
  assert(ccd->hLight);
  int agregRebin=m3Eve->light[quadId].agregRebin;

  printf("%s ClusterMaker::bestTrack() start.. frameId=%d frameType=%d\n",coreName.Data(),m3Eve->frameId, abortFrame); 
  
  assert( abortFrame==frmPromise );
  float clustLight=0;
  int  bestClustId=-1;
  for(int k=0;k<m3Eve->clusterV[quadId].size(); k++){
    M3ImageCluster &cluster= m3Eve->clusterV[quadId][k];
    if( cluster.type<0 ) continue; //  already discarded
    if(clustLight>cluster.light ) continue;
    bestClustId=k;
    clustLight=cluster.light ;
  }

  printf("best cluster ID=%d, light=%.1f adu dump:",bestClustId,clustLight); 
  M3ImageCluster &cluster= m3Eve->clusterV[quadId][bestClustId];
  cluster.print();
  
  TCanvas *can2=new TCanvas();
  can2->cd();
  ccd->hLightRot->Draw("colz");  

  double xClustPix=cluster.cmJx*agregRebin; 
  double yClustPix=cluster.cmJy*agregRebin; 
  double theta=cluster.Ialpha; //rad
  double cth=cos(theta),sth=sin(theta);
  double uOffPix=m3Eve->light[quadId].pixOffsetRot, vOffPix=uOffPix;


  // shift & rotate light to center it on the cluster
  int nchX=hPedStat->GetNbinsX();
  int nchY=hPedStat->GetNbinsY(); 
  for(int jchX=1; jchX<=nchX; jchX++)  // range 1,N
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N
      int mBin=hPedStat->GetBin(jchX, jchY);  //   convert 2D to 1D index
      if (  hPedStat->GetBinContent(mBin) ) continue; // drop masked channels
      float rms=ccd->hLight->GetBinError(mBin); 
      float val=ccd->hLight->GetBinContent(mBin); 
      assert(rms>5);// (adu) sth went wrong
      
      // transformx,y coordinates
      double xPix=ccd->hLight->GetXaxis()->GetBinCenter(jchX) -xClustPix;
      double yPix=ccd->hLight->GetYaxis()->GetBinCenter(jchY) -yClustPix;
 
      double uPix=xPix*cth + yPix*sth  +uOffPix;
      double vPix=-xPix*sth + yPix*cth +vOffPix;
 
      // find kBin in target histo
      int jchU=ccd->hLightRot->GetXaxis()->FindBin(uPix);
      int jchV=ccd->hLightRot->GetYaxis()->FindBin(vPix);
      double oldVal=ccd->hLightRot->GetBinContent(jchU,jchV);
      double oldErr=ccd->hLightRot->GetBinError(jchU,jchV);
      double newVal=oldVal+val; // assume errors are very simular
      double newErr=sqrt(oldErr*oldErr + rms*rms);  
      ccd->hLightRot->SetBinContent(jchU,jchV,newVal);
      ccd->hLightRot->SetBinError(jchU,jchV,newErr);
    }
      
  gPad->SetGrid();

}

#endif
  
  
