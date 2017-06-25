#include <assert.h>

#include "TH2.h"
#include "TH2I.h"

#include "TF1.h"
#include "TFile.h"
#include "TString.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TEllipse.h"

#include "TText.h"
#include "TStyle.h"

#include "M3CcdClusterMaker.hh" 
#include "M3CcdCalibMaker.hh" 
#include "M3JanEvent.hh" 

//=======================================
//=======================================
void dmtpc::ped::M3CcdClusterMaker::setCSVreport(TString outFname){
  printf("write report to: %s\n",outFname.Data());
  fdCSV=fopen(outFname.Data(),"w");
  assert(fdCSV);
  int runId=jEve->runId;
  fprintf(fdCSV,"# seed cluster finder report, runId: %d, cam: %d\n",runId,camId);
  fprintf(fdCSV,"# frame, camera, type, light(adu), diag (mm), CM-X(pix), CM-Y(pix), I_min, I_max, alpha(rad)\n");

  listPiSpark+=Form(" cam%d:",camId);
  listStarSpark+=Form(" cam%d:",camId);

}

//=======================================
//=======================================
void dmtpc::ped::M3CcdClusterMaker::saveHisto(TFile *fd) {
  int runId=jEve->runId;

  printf(" %s histos  are written  to %s \n" ,coreName.Data(),fd->GetName());
  for(int i=0;i<mxH;i++) {
    if(hA[i]) hA[i]->Write();
  }
  printf("%s has %d promissing frames",coreName.Data(),nPromise);

  if(fdCSV) fclose(fdCSV);

  h2DlightRot->Write(); //tmp
  if(can) {
    can=new TCanvas(); // this is memory leak - who cares
    can->cd(0);
    TText mesg(0.3,0.8,Form("cam%d run%d cluster finder summary ",camId, runId));
    mesg.Draw();  // mesg.Print();
    TText mesg1(0.1,0.7,Form("%d promissing frames, below rejected frames: ",nPromise));
    mesg1.Draw();  // mesg.Print();

    TText mesg2(0.1,0.6,listPiSpark);
    mesg2.SetTextSize(0.025);  mesg2.Draw();  //mesg2.Print();
  
    TText mesg3(0.1,0.5,listStarSpark);
    mesg3.SetTextSize(0.025);  mesg3.Draw();  //mesg3.Print();
  

    can->Print( pdfName+")","pdf");
  }
  delete can;
  
  printf(" , save Ok\n");
}

//=======================================
//=======================================
void dmtpc::ped::M3CcdClusterMaker::setDrawings(TCanvas *canX, TString outPath){
  assert(canX);

  can=canX;
  can->cd(0);

  int runId=jEve->runId;
  //  int uReb=calibData->par_userRebin;
  double pedRms=jEve->light[camId].avrPedRms;
  int agregRebin=jEve->light[camId].agregRebin;
 
  TText mesg(0.3,0.8,Form("cam%d run%d  cover page",camId, runId));
  mesg.Draw();  // mesg.Print();

  TText mesg2(0.1,0.6,Form("CUTS1: cellCntL=%.0f  clustDiagL(mm)=%.1f  pcaRatioH=%.2f  starSparkLght=%.2e", cut_cellCountL, cut_clustDiagL, cut_pcaRatioH,par_starSparkLight));
  mesg2.SetTextSize(0.025);  mesg2.Draw();  // mesg2.Print();

  TText mesg21(0.1,0.5,Form("CUTS2:   nSigThr1=%.1f  nSigThr2=%.1f  clustLightL=%.0f ADU", par_nSigThr1, par_nSigThr2,cut_clustLightL));
  mesg21.SetTextSize(0.025);  mesg21.Draw(); //  mesg21.Print();

  
  TText mesg3(0.1,0.4,Form("CONDITIONS:  avrPedRms=%.1f ADU  agregRebin=%d ", pedRms, agregRebin));
  mesg3.SetTextSize(0.025);  mesg3.Draw(); //  mesg3.Print();
  
  pdfName=outPath+"/"+Form("anaClust_R%d_cam%d_reb%d.pdf",runId,camId, agregRebin);
  can->Print( pdfName+"(","pdf");

}


//=======================================
//=======================================
void dmtpc::ped::M3CcdClusterMaker::initHisto() {

  assert(jEve);
  assert(jEve->light[camId].hPedStat);
  coreName=Form("cam%dclu",camId);
  h2Dlight= jEve->light[camId].h2Dlight;
  h2DlightRot= jEve->light[camId].h2DlightRot;
  hPedStat=  jEve->light[camId].hPedStat;
 
 
  // create local 2D plane storing some cluster info
  hUtil=(TH2I*) jEve->light[camId].hPedStat->Clone(); // helping build clusters

  hUtil->SetNameTitle(Form("cam%d_clUt_fixMe88",camId), Form("util clustFind info cam%d  fixMe78 ",camId));
  hUtil->Reset();
  hUtil->Sumw2();  // allow independent errors from values
  hUtil->SetMarkerSize(0.7);//<-- large text 
  //hUtil->SetMarkerColor(9); // & color


  memset(hA,0,sizeof(hA)); // input spectra, unweighted
  TH1* h;
 
  enum {mxCaseC=11,mxCase=10};
  int nCase=mxCaseC;
  hA[0]=h=new TH1D(coreName+"_cluCase","QA of clusters; type; CLUSTERS",nCase,0,nCase);  h->GetXaxis()->SetTitleOffset(0.4);  h->GetXaxis()->SetLabelSize(0.06);  h->GetXaxis()->SetTitleSize(0.05); h->SetMinimum(0.8);  h->SetFillColor(29);h->SetLineWidth(2);  h->SetMarkerSize(2);//<-- large text 
    

  TString nameA[mxCaseC]={"any","nCell","diag","fiduIn","fiduOut","light","no3Edge","pcaR","isHR","isGood","-"};
  for(int i=0;i<mxCaseC;i++) {
    hA[0]->Fill(nameA[i],0.);
  }

  hA[1]=h=new TH1D(coreName+"_frmCase","QA of frames; type; FRAMES",nCase,0,nCase);  h->GetXaxis()->SetTitleOffset(0.4);  h->GetXaxis()->SetLabelSize(0.06);  h->GetXaxis()->SetTitleSize(0.05); h->SetMinimum(0.8); h->SetFillColor(42);h->SetLineWidth(2);  h->SetMarkerSize(2);//<-- large text 
    
  TString nameB[mxCase]={"inp","starSpark","hasClust","piSpark","goodClust","manyGood","g","k","m","n"};
  for(int i=0;i<mxCase;i++) {
    hA[1]->Fill(nameB[i],0.);
  }
  
  //1..3 - free

  hA[4]=h=new TH1D(coreName+"_frmLt","sum light/frame (zoom-in), all input; sum frame light=raw-ped (kADU)",100, -10,50);
  
  hA[5]=h=new TH1D(coreName+"_celCnt","cluster cell-count, any; Log10(cell count); clust count", 100,-0.1,3); h->SetFillColor(kGreen);
  { TList *tl=h->GetListOfFunctions(); assert(tl);
    double cutVal=TMath::Log10(cut_cellCountL);
    TLine *ln=new TLine(cutVal, 0, cutVal, 1e6);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
  }

  //float diag2=300.;// alpha
  float diag2=80.;// alpha

  hA[6]=h=new TH2D(coreName+"_Ldens","cluster LtDens vs. diag (pi-spark det), so far; diagonal (mm) ; light/diagonal (ADU/mm)  ",  80,0.,diag2, 50,0.,600); 
  { TList *tl=h->GetListOfFunctions(); assert(tl);    
    TLine *ln=new TLine(  par_piSparkDiag,  par_piSparkLtDens, par_piSparkDiag,800);   
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);  tl->Add(ln);
    ln=new TLine(  par_piSparkDiag,  par_piSparkLtDens,400,par_piSparkLtDens);   
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);  tl->Add(ln);
  }


  hA[7]=h=new TH2D(coreName+"_S_L","clust size-vs-light (pi-spark det), any;  Log10(cell count); Log10(light/ADU)", 50,-0.1,3, 50,1.5,5.5);
  { TList *tl=h->GetListOfFunctions(); assert(tl);
    double cutVal=TMath::Log10( par_piSparkNcell);
    TLine *ln=new TLine(cutVal, 0, cutVal, 1e6);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
  }

  hA[8]=h=new TH2D(coreName+"_frmS_L","Frame sum light, any;cluster sum Log10(light/ADU) ; full frame Log10(light/ADU)",  50,1,7, 50,1,9);
  { TList *tl=h->GetListOfFunctions(); assert(tl);
    TLine *ln=new TLine(1, 1, 8,8);    tl->Add(ln); // diagonal
    ln->SetLineColor(kGreen); ln->SetLineStyle(2);
  }
  
  hA[9]=h=new TH1D(coreName+"_diag","cluster diameter , so far; diagonal (mm); clust count", 100,1,diag2/5.);  h->SetFillColor(kGreen);
  { TList *tl=h->GetListOfFunctions(); assert(tl);
    double cutVal=cut_clustDiagL;
    TLine *ln=new TLine(cutVal, 0, cutVal, 1e6);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
  }
  

  hA[10]=h=new TH2D(coreName+"_cmXY","Cluster CM  XY on mesh, Z=Light sum, so far; X pos (pix); Y pos (pix)",93,0,3100,93,0,3100);
  h->Sumw2();


  hA[11]=h=new TH1D(coreName+"_Light","cluster light, so far ; Log10(light/ADU); clust count", 100,1,7);  h->SetFillColor(kGreen);
  { TList *tl=h->GetListOfFunctions(); assert(tl);
    double cutVal=TMath::Log10(cut_clustLightL);
    TLine *ln=new TLine(cutVal, 0, cutVal, 1e6);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
  }

  hA[12]=h=new TH1D(coreName+"_pcaR","cluster PCA ratio; I_small/I_large ",100,0,1.); h->SetFillColor(kGreen);
  { TList *tl=h->GetListOfFunctions(); assert(tl);
    double cutVal=cut_pcaRatioH;
    TLine *ln=new TLine(cutVal, 0, cutVal, 1e6);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
  }

  hA[13]=h=new TH2D(coreName+"_pca12","cluster PCA magnitude, so far; I_large  Log10(ch^2*ADU) ;  I_small  Log10(ch^2*ADU) ",  50,1,8, 50,1,8);
   { TList *tl=h->GetListOfFunctions(); assert(tl);
    TLine *ln=new TLine(2, 1, 8,7);    tl->Add(ln); // diagonal on 2D plot
    ln->SetLineColor(9); ln->SetLineStyle(2);
    TText *tx=new TText(3,3,"I_large=10 * I_small");  tl->Add(tx);
    tx->SetTextSize(0.04); tx->SetTextColor(9); tx->SetTextAngle(45);
  }

   hA[14]=h=new TH2D(coreName+"_Imi_L","cluster Imin vs. Light, so far;  Log10(light/ADU) ;  I_small  Log10(ch^2*ADU) ",  50,1.5,5.5, 50,1,8);


   hA[15]=h=new TH2D(coreName+"_nCD","cluster shape , so far; diagonal (mm) ; sqrt(nCell)  ",  50,0.,diag2, 30,0.,30); 

  hA[16]=h=new TH2D(coreName+"_celXY","Cluster cells XY, agregated, good; X pos (pix); Y pos (pix)",93,0,3100,93,0,3100);


  hA[17]=h=new TH1D(coreName+"_okLtSm","cluster light,  final ; light (ADU); clust count", 300,0,3000);  h->SetFillColor(kGreen);   h->SetFillColor(kGreen);

  hA[18]=h=new TH2D(coreName+"_LtDg","cluster light vs. diagonal, so far ; light (kADU); diag (mm)", 100,0,8,30,0,8); //100,0,40,30,0,30);
  { TList *tl=h->GetListOfFunctions(); assert(tl);
    double cutVal=cut_clustLightL/1000.;
    TLine *ln=new TLine(cutVal, 0, cutVal, 50);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
    
    cutVal=cut_clustDiagL;
    ln=new TLine(0,cutVal, 100, cutVal);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
    
    if(cut_factHadrRec>0) {     
      double x1=8000, y1=1000.*x1/cut_factHadrRec;
      ln=new TLine(0, 0, x1,y1);    tl->Add(ln);
      ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
    }
  }

  int max_cm=200;
  hA[19]=h=new TH2D(coreName+"_hallZX","Cluster to source lines ; floor X (cm); hight Z (cm)", max_cm,-max_cm,max_cm, max_cm,-max_cm,max_cm);
   { TList *tl=h->GetListOfFunctions(); assert(tl);
    float Rcam=65;//cm
    float th1=0,th2=90;  //deg, for cam1
    float xTx=2*Rcam, yTx=0.5*Rcam;

    if(camId==3) { th1=180; th2=270; xTx=-2.3*Rcam;   yTx*=-1;}
    if(camId==0) { th1=90; th2=180;  xTx=-2.3*Rcam;}
    if(camId==2) { th1=-90; th2=0; yTx*=-1;}
    
    TEllipse *el=new TEllipse(0,0,Rcam,Rcam,th1,th2);    tl->Add(el); 
    el->SetFillStyle(0); el->SetLineColor(kMagenta);
   
    TText *camTx=new TText( xTx,yTx,Form("cam-%d",camId));
    tl->Add(camTx);  camTx->SetTextColor(kMagenta);
   }


   hA[20]=h=new TH1D(coreName+"_okAl","cluster PCA alpha W=1, final ; PCA angle (rad); cluster count",50,-0.5,3.5);  h->SetFillColor(kGreen);

   hA[21]=h=new TH2D(coreName+"_okAlLt","cluster PCA alpha vs. light W=1, final ;light (kADU);PCA  angle (rad)", 100,0,40,50,-0.5,3.5);

   hA[22]=h=new TH2D(coreName+"_okAldi","cluster PCA alpha vs. diagonal W=1, final ;diagonal(mm);PCA  angle (rad)", 60,0,30,50,-0.5,3.5);

 
   printf(" M3CcdClusterMaker::initHisto()  dbg=%d cuts:  cellCountL=%.0f clustLightL=%.0f ADU clustDiagL(mm)=%.1f pcaRatioH=%.2f  par_starSparkLight=%.2e   nSigThr1=%.f  nSigThr2=%.1f  fiduBand=%.0f  hadrRec=%.1f\n", dbg, cut_cellCountL,cut_clustLightL, cut_clustDiagL, cut_pcaRatioH,par_starSparkLight, par_nSigThr1, par_nSigThr2,par_fiduBandPix,cut_factHadrRec);

  
}

//=======================================
//=======================================
void dmtpc::ped::M3CcdClusterMaker::setFiducial(float bandPix){
  assert(camId==3 || camId==2); // tmp, other were not tested

  // hardcoded paramatrized quadrant
  TVector2 C,R1,R2;

  if(camId==2) {    
    TVector2 C0(320,2790), A1(380,140), A2(3000,2795);// (pix), cam2
    C=C0+TVector2(bandPix,-bandPix);
    R1=A1 + TVector2(bandPix,bandPix) - C;
    R2=A2 - TVector2(bandPix,bandPix) - C;
  } 


  if(camId==3) {
    TVector2 C0(2800,2840), A1(100,2820), A2(2830,160); // (pix), cam3
    C=C0-TVector2(bandPix,bandPix);
    R1=A1 + TVector2(bandPix,-bandPix) - C;
    R2=A2 + TVector2(-bandPix,bandPix) - C;
  } 

  
  cut_fiduC=C;
  cut_fiduPhi1=R1.Phi(); //Returns  interval [0,2*PI)
  cut_fiduPhi2=R2.Phi();
  if(cut_fiduPhi2 < cut_fiduPhi1) cut_fiduPhi2+=6.2832;
  double xx=0.5*(R1.Mod()+ R2.Mod());
  cut_fiduR2max=xx*xx;

  // >0 in, <0 out, =0 off
  par_fiduBandPix=bandPix; // acts like a switch
  
  printf(" fiduCut(camId=%d) set, band=%.0f (pix)  phiRange=[%.2f,%.2f] Rmax=%.1f pix\n",camId,par_fiduBandPix,cut_fiduPhi1,cut_fiduPhi2,sqrt(cut_fiduR2max));
}


//=======================================
//=======================================
int dmtpc::ped::M3CcdClusterMaker::isFiducial(float xPix, float yPix){
  int isIn=1;  
  

  TVector2 P=TVector2(xPix,yPix) - cut_fiduC;

  if(par_fiduBandPix>0 ) { // accept inside
    if(P.Mod2() >cut_fiduR2max) isIn=0;  
    double phi=P.Phi();
    if(phi<cut_fiduPhi1)  isIn=0;
    if(phi>cut_fiduPhi2)  isIn=0;
  }
  
   if(par_fiduBandPix<0 ) { // accept outside
     // if( fabs(xPix-800) <100 &&  fabs(yPix-1000) <100)
     // printf("zzz %.1f %.1f  %d \n",P.Mod2() ,cut_fiduR2max,P.Mod2() <cut_fiduR2max);
      if(P.Mod2() <cut_fiduR2max) isIn=0;  
  }

  return isIn;
}

