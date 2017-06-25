TString pdfName="";


plClustQA(  int page=1, TString core="cam3", TString fname="m3_Michael_R1028100", char *inpPath="./", char *oPath=""){

  //fname="dmtpc_m3_alpha_00198";

  //fname="sum22";
  int squareNpix=700;
  
  char *nameA[]={"clu_frmCase","clu_cluCase"}; //pg 1
  char *nameB0[]={"clu_frmLt","clu_frmS_L"}; // pg 2
  char *nameB[]={"clu_S_L","clu_celCnt", "clu_Ldens", "clu_diag", "clu_nCD","clu_Light"}; // pg 3
  char *nameD[]={"clu_cmXY","clu_pcaR","clu_Imi_L","clu_pca12","clu_celXY"}; // pg 4

  char *nameE[]={"clu_LtDg","clu_okAlLt"}; // pg 5
  char *nameF[]={"clu_okAl","clu_okLtSm","clu_okAldi"}; // pg 6


  // pg 11 - x-y CM of clusters , big
  // pg 12 - x-y all cells in clusters , big
  // pg 13 - xy hall , extrapolated tracks

  char *nameDD[]={"cal_UpedAvr","cal_UpedRms","cal_UpedThr1"}; //pg 20
 

  TString fullInpName=inpPath;  fullInpName+="/";fullInpName+=fname;
  if(!fullInpName.Contains("root") )fullInpName+=".m3ana.root";
  fd=new TFile(fullInpName);
  if(! fd->IsOpen()) {
    printf("EROR: input histo file not found, quit\n",fullInpName.Data());
    return;
  } else {
    printf("Opened: %s  core=%s\n",fullInpName.Data(),core.Data());
  }
  
  // fd->ls();

  h0=(TH1D*)fd->Get(core+"clu_cluCase"); assert(h0);

  if( page==1) {
    fd->ls(); 
    printf("%s bin:count  ",h0->GetName());
    for(int k=1;k<=7;k++) printf("%d=>%.2g,  ",k,h0->GetBinContent(k));
    printf("\n");
  }

  gStyle->SetPalette(1,0);
  gStyle->SetOptFit(1);
  gStyle->SetOptStat(1);

  TString  padTit=Form("%s",fname.Data());
  printf("=%s=\n",padTit.Data());
  
  switch (page) {
    
  case 1:{
    //can=new TCanvas("aa","aa",700,700);    
    can=new TCanvas("aa","aa",squareNpix,1.05*squareNpix);
    TPad *c=makeTitle(can,core+"  summary, "+padTit,page); c->cd();   
    c->Divide(1,2); 
    gStyle->SetOptStat(0);
    char **nameX=nameA;   
    for(int i=0;i<2;i++) {
      printf("->%s%s<\n",core.Data(),nameX[i]);
      h=(TH1*)fd->Get(core+nameX[i]);         assert(h);
      c->cd(i+1);      
      h->Draw("bar2 text");      
      gPad->SetTopMargin(0.20); 
      gPad->SetLogy();      
    }    
  }  break;//--------------------------------------

 case 2:{
   //can=new TCanvas("aa","aa",700,700);    
   can=new TCanvas("aa","aa",squareNpix,1.05*squareNpix);
   TPad *c=makeTitle(can,core+" frame info, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(10);  
    c->Divide(2,2);     
    TH1 *hx=0;
    char **nameX=nameB0; 
    for(int i=0;i<2;i++) {
      printf(" draw=%s=\n",nameX[i]);
      hx=(TH1*)fd->Get(core+nameX[i]);   assert(hx);
      c->cd(i+1);
      hx->Draw("colz");
      //  if(i==4) gPad->SetLogx();
    }
  } break;//--------------------------------------



  case 3:{
    can=new TCanvas("aa","aa",1.5*squareNpix,squareNpix);
    TPad *c=makeTitle(can,core+" cluster w/o skin, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(10);  
    c->Divide(3,2);     
    TH1 *hx=0;
    char **nameX=nameB; 
    for(int i=0;i<6;i++) {
      printf(" draw=%s=\n",nameX[i]);
      hx=(TH1*)fd->Get(core+nameX[i]);   assert(hx);
      c->cd(i+1);
      hx->Draw("colz");
      if(i==0 || i==5) { gPad->SetRightMargin(0.15); gPad->SetTopMargin(0.15);}
      if(i==1) gPad->SetLogy();
      if(i==5 || i==3) gPad->SetLogy();
      gPad->SetGrid();
    }
  } break;//--------------------------------------



  case 4:{
    can=new TCanvas("aa","aa",1.5*squareNpix,squareNpix);
    TPad *c=makeTitle(can,core+" calibration QA, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(1110);  
    c->Divide(3,2);     
    TH1 *hx=0;
    char **nameX=nameD; 
    for(int i=0;i<5;i++) {
      printf(" draw=%s=\n",nameX[i]);
      hx=(TH1*)fd->Get(core+nameX[i]);   assert(hx);
      c->cd(i+1);
      hx->Draw("colz");
      gPad->SetGrid();
    }
  } break;//--------------------------------------



  case 5:{
    can=new TCanvas("aa","aa",squareNpix,1.05*squareNpix);  
    TPad *c=makeTitle(can,core+" final selection of clusters, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(10);  
    c->Divide(1,2);     
    TH1 *hx=0;
    char **nameX=nameE; 
    for(int i=0;i<2;i++) {
      printf(" draw=%s=\n",nameX[i]);
      hx=(TH1*)fd->Get(core+nameX[i]);   assert(hx);
      c->cd(i+1);
      hx->Draw("colz");
      gPad->SetGrid();

      if(i==1) { 
	//	hx->SetAxisRange(0,20,"x");
      }
    }


  } break;//--------------------------------------


  case 6:{
    can=new TCanvas("aa","aa",squareNpix,1.05*squareNpix);  
    TPad *c=makeTitle(can,core+" final selection of clusters, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(1110);  
    c->Divide(2,2);     
    TH1 *hx=0;
    char **nameX=nameF; 
    for(int i=0;i<3;i++) {
      printf(" draw=%s=\n",nameX[i]);
      hx=(TH1*)fd->Get(core+nameX[i]);   assert(hx);
      c->cd(i+1);
      hx->Draw("colz");
      gPad->SetGrid();
      if(i==0) hx->Rebin(2);
     }
    hy=((TH2D*)hx)->ProjectionX();
    c->cd(4);
    //    hy->Draw();
    hy->Fit("expo");
    gPad->SetLogy();
 gPad->SetGrid();
  } break;//--------------------------------------


   case 11:{
    can=new TCanvas("aa","aa",squareNpix,1.05*squareNpix);
    TPad *c=makeTitle(can,core+" cluster XY CM location, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(10);  

    char *nameX="clu_cmXY";
    printf(" draw=%s=\n",nameX);
    hx=(TH1*)fd->Get(core+nameX);   assert(hx);
    hx->Draw("colz");
   }   break;//-------------------------------------- 



   case 12:{
    can=new TCanvas("aa","aa",squareNpix,1.05*squareNpix);
    TPad *c=makeTitle(can,core+" good cluster cells XY  location, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(10);  

    char *nameX="clu_celXY";
    printf(" draw=%s=\n",nameX);
    hx=(TH1*)fd->Get(core+nameX);   assert(hx);
    hx->Draw("colz");
    drawFiducialLimit(core);
   }   break;//-------------------------------------- 


   case 13:{
    can=new TCanvas("aa","aa",squareNpix,1.05*squareNpix);
    TPad *c=makeTitle(can,core+" extrapolated good tracks, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(10);  
    char *nameX="clu_hallZX";
    printf(" draw=%s=\n",nameX);
    hx=(TH1*)fd->Get(core+nameX);   assert(hx);
    hx->Draw("colz");
   }   break;//-------------------------------------- 



  case 20:{
    can=new TCanvas("aa","aa",1000,700);    
    TPad *c=makeTitle(can,core+" pedestals calibration QA, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(1111);  
    c->Divide(2,2);     
    TH1 *hx=0;
    char **nameX=nameDD; 
    for(int i=0;i<3;i++) {
      printf(" draw=%s=\n",nameX[i]);
      hx=(TH1*)fd->Get(core+nameX[i]);   assert(hx);
      c->cd(i+1);
      hx->Draw("colz");

    }
  } break;//--------------------------------------



   case 21:{
    can=new TCanvas("aa","aa",700,730);    
    TPad *c=makeTitle(can,core+" masked pixels, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(0);  

    char *nameX="_pedStat";
    printf(" draw=%s=\n",nameX);
    hx=(TH1*)fd->Get(core+nameX);   assert(hx);
    hx->Draw("colz");
    // hx->Draw("box");
   }    break;//--------------------------------------




 

  default:
    printf("page=%d NOT defined\n",page);
    return;
  }

  char text[100];
  sprintf(text,"%s%s_page%03d",oPath,fname.Data(),page);
  TString tit=text;
  can->SetTitle(tit);
  can->SetName(tit);

  if(pdfName.Sizeof() >5 ) can->Print(pdfName,"pdf");
}


//------------------------

//------------------------
void splitPadX(float x, TPad **cL, TPad **cR) {
  (*cL) = new TPad("padL", "apdL",0.0,0.,x,0.95);
  (*cL)->Draw();
  (*cR) = new TPad("padL", "apdL",x+0.005,0.,1.0,0.95);
  (*cR)->Draw();
}

//------------------------
void splitPadY(float y, TPad **cU, TPad **cD) {
  (*cU) = new TPad("padD", "apdD",0,y+0.005,1.0,1.);
  (*cU)->Draw();
  (*cD) = new TPad("padU", "apdU",0.0,0.,1.,y);
  (*cD)->Draw();

  /* use case:
     TPad *cU,*cD;   splitPadY(0.4,&cU,&cD);    cU->cd(); h->Draw()
  */
}

//------------------------
TPad *makeTitle(TCanvas *c,TString core, int page) {
  c->Range(0,0,1,1);
  TPad *pad0 = new TPad("pad0", "apd0",0.0,0.95,1.,1.);
  pad0->Draw();
  pad0->cd();

  TPaveText *pt = new TPaveText(0,0.,1,1,"br");
  pt->Draw();
  TDatime dt;
  TString txt2=core;
  txt2+=", page=";
  txt2+=page;
  txt2+=",  ";
  txt2+=dt.AsString();
  pt->AddText(txt2);
  txt2="--";
  pt->AddText(txt2);

  c->cd();
  pad = new TPad("pad1", "apd1",0.0,0.0,1,.95);
  pad->Draw();
  return pad;
}


//------------------------
void doAll(TString core="cam3", TString fname="m3_Michael_R1022003") {
  TString outPdf=fname+".clust_ana.pdf";
  pdfName=outPdf+"(";    plClustQA(1,core,fname);  // 1st page

  pdfName=outPdf; //for pages: 2.... N-1 
  plClustQA(2,core,fname);  
  plClustQA(3,core,fname);  
  plClustQA(4,core,fname); 
  plClustQA(5,core,fname); 
  //  plClustQA(12); 
  // plClustQA(20);  

  pdfName=outPdf+")";    plClustQA(12,core,fname);   // last page

  printf("\nto display PDF execute:\n   evince %s  \n",outPdf.Data());
 
}
 

 
//==============================
void drawFiducialLimit( TString camName) {
  if(camName=="cam2") {
    C=TVector2(320,2790);
    A1=TVector2(380,140);
    A2=TVector2(3000,2795);
  }
  if(camName=="cam3") {
    C=TVector2(2800,2840);
    A1=TVector2(100,2820);
    A2=TVector2(2830,160); // (pix), cam3
  }
  
  TVector2 R1=A1  - C;
  TVector2 R2=A2  - C;
  double phi1=R1.Phi()/3.1416*180; //Returns  interval [0,2*PI)
  double phi2=R2.Phi()/3.1416*180; //Returns  interval [0,2*PI)
  //if( cluster.Ialpha<0) thetaDeg=90+thetaDeg;
  printf("phi1,2= %.1f %.1f\n", phi1,phi2);
  if (phi2<phi1) phi2+=360;
  double r=0.5*(R1.Mod() + R2.Mod());
  TEllipse *el=new TEllipse(C.X(),C.Y(),r,r,phi1,phi2);
  el->Draw();
  el->SetFillStyle(0); el->SetLineColor(kMagenta);
}
