TString pdfName="";


plClustQA(  int page=1,TString fname="sum", char *inpPath="outS/", TString core="quad3", char *oPath=""){


  //fname="sumAmBe";
  //fname="m3_Michael_R1035021";
  int squareNpix=550;
  
  char *nameA[]={"clu_frmCase","clu_cluCase","clu_expT0"}; //pg 1
  char *nameB[]={"cal_frmLtRes","cal_frmLtSum","cal_binPedRes"}; // pg 2
  char *nameC[]={"clu_celCnt","clu_S_L", "clu_Ldens", "clu_diag","clu_cmXY", "clu_nCD"}; // pg 3
  char *nameD[]={"clu_Light","clu_pcaR","clu_Imi_L","clu_pca12","clu_celXY"}; // pg 4

  char *nameE[]={"clu_LtDg","clu_okAlLt"}; // pg 5
  char *nameF[]={"clu_okAl","clu_okLtSm","clu_okS_L","clu_okAldi","clu_okLtDens"}; // pg 6

  char *nameG[]={"clu_okwidi","clu_okspdi","clu_pklrm","clu_okdi","clu_okLt"}; // pg 7

  // pg 11 - x-y CM of clusters , big
  // pg 12 - x-y all cells in clusters , big
  // pg 13 - xy hall , extrapolated tracks
  // pga 14 - rate of ok-clust vs capmaign time

  //pg 20 - summary of accepted

  char *nameDD[]={"cal_UpedAvr","cal_UpedRms"}; //pg 20
 

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
  //  gStyle->SetOptStat(1);

  TString  padTit=Form("%s",fname.Data());
  printf("=%s=\n",padTit.Data());
  
  switch (page) {
    
  case 1:{
    can=new TCanvas("aa","aa",squareNpix,1.05*squareNpix);
    TPad *c=makeTitle(can,core+"  summary, "+padTit,page); c->cd();   
    c->Divide(1,3); 
    gStyle->SetOptStat(0);
    char **nameX=nameA;   
    for(int i=0;i<3;i++) {
      printf("->%s%s<\n",core.Data(),nameX[i]);
      h=(TH1*)fd->Get(core+nameX[i]);         assert(h);
      c->cd(i+1);      
      if(i<2)  { h->Draw("bar2 text");       gPad->SetLogy();  }
      else h->Draw();
      gPad->SetTopMargin(0.20); 
         
    }    
  }  break;//--------------------------------------

 case 2:{
   //can=new TCanvas("aa","aa",700,700);    
   can=new TCanvas("aa","aa",squareNpix,1.05*squareNpix);
   TPad *c=makeTitle(can,core+" frame info, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(1110);  
    c->Divide(2,2);     
    TH1 *hx=0;
    char **nameX=nameB; 
    for(int i=0;i<3;i++) {
      printf(" draw=%s=\n",nameX[i]);
      hx=(TH1*)fd->Get(core+nameX[i]);   assert(hx);
      c->cd(i+1);
      hx->Draw("colz");
      //  if(i==4) gPad->SetLogx();
      gPad->SetGrid();
    }
    
  } break;//--------------------------------------



  case 3:{
    can=new TCanvas("aa","aa",1.5*squareNpix,squareNpix);
    TPad *c=makeTitle(can,core+" cluster w/o skin, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(1110);  
    c->Divide(3,2);     
    TH1 *hx=0;
    char **nameX=nameC; 
    for(int i=0;i<6;i++) {
      printf(" draw=%s=\n",nameX[i]);
      hx=(TH1*)fd->Get(core+nameX[i]);   assert(hx);
      c->cd(i+1);
      hx->Draw("colz");
      //     if(i==0 || i==5) { gPad->SetRightMargin(0.15); gPad->SetTopMargin(0.15);}
      if(i==0 || i==3) gPad->SetLogy();
      if( i==1) gPad->SetLogz();
      if(i==4) drawFiducialLimit(core);
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
      if(i==4) drawFiducialLimit(core);
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
    can=new TCanvas("aa","aa",1.5*squareNpix,squareNpix);  
    TPad *c=makeTitle(can,core+" final selection of clusters, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(1110);  
    c->Divide(3,2);     
    TH1 *hx=0;
    char **nameX=nameF; 
    for(int i=0;i<5;i++) {
      printf(" draw=%s=\n",nameX[i]);
      hx=(TH1*)fd->Get(core+nameX[i]);   assert(hx);
      c->cd(i+1);
      hx->Draw("colz");
      gPad->SetGrid();
      if(i==0 ) { hx->Rebin(8);hx->Draw("h e");  }
      if(i==1 ) hx->Rebin(8);
      
     }
 
  } break;//--------------------------------------





  case 7:{
    can=new TCanvas("aa","aa",1.5*squareNpix,squareNpix);  
    TPad *c=makeTitle(can,core+" final selection of clusters, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(1110);  
    c->Divide(3,2);     
    TH1 *hx=0;
    char **nameX=nameG; 
    for(int i=0;i<5;i++) {
      printf(" draw=%s=\n",nameX[i]);
      hx=(TH1*)fd->Get(core+nameX[i]);   assert(hx);
      c->cd(i+1);
      hx->Draw("colz");
      gPad->SetGrid();

      if(i==3) { 
	hx->Rebin(2);
	hx->SetMinimum(0.7);
	hx->Fit("expo");
	gPad->SetLogy();
      }
      if(i==4) gPad->SetLogy();
    }

    c->cd(6);
    hy=(TH2F*)fd->Get(core+"clu_okwidi");   assert(hy);
    //   hy->Draw("colz");
    hyp=hy->ProjectionY();
    hyp->Draw();
    gPad->SetGrid();
    //    hyp->
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


   case 14:{
    can=new TCanvas("aa","aa",squareNpix,1.4*squareNpix);
    TPad *c=makeTitle(can,core+" good clust vs. time, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(10);  
    c->Divide(1,3);
    h1=(TH1*)fd->Get(core+"clu_expT0");   assert(h1);
    c->cd(1);    h1->Draw();  
    
    h2=(TH1*)fd->Get(core+"clu_okT0");   assert(h2);
    c->cd(2);    h2->Draw();

    h3=(TH1*) h2->Clone(); h3->Sumw2();
    h3->Divide(h1);
    c->cd(3); h3->Draw();
    int nReb=8;
    h3->Rebin(nReb); h3->Scale(1./nReb); h3->GetYaxis()->SetTitle(" ");
    h3->SetTitle("good clust rate per 1 expo-hour");
    TLine *ln=new TLine(0,0,350,0); ln->Draw();
    gPad->SetGrid();
    
   
    
   }   break;//-------------------------------------- 



  case 20:{
    can=new TCanvas("aa","aa",1.8*squareNpix,squareNpix);
    TPad *c=makeTitle(can,core+" summary of accepted, "+padTit,page); c->cd();
    gStyle->SetOptStat(1110);  

    TPad *cL,*cR;   splitPadX(0.54,&cL,&cR);    
    cL->cd(); gPad->SetRightMargin(0.15);
    hx=(TH1*)fd->Get(core+"clu_celXY");   assert(hx);
    hx->Draw("colz");
    drawFiducialLimit(core);
    gPad->SetGrid();

    cR->cd(); cR->Divide(2,2);
    char *nameXX[]={"clu_okLtSm","clu_pcaR","clu_okdi","clu_frmCase"}; 
    
   for(int i=0;i<4;i++) {
      printf(" draw=%s=\n",nameXX[i]);
      hx=(TH1*)fd->Get(core+nameXX[i]);   assert(hx);
      cR->cd(i+1);
      hx->Draw("colz");
      gPad->SetGrid();
      if(i==0) hx->Rebin(4);
      if(i==1){ hx->SetAxisRange(0.,0.4);}
      if(i==2) { gPad->SetLogy(); hx->Rebin(2);}
      hx->GetXaxis()->SetLabelSize(0.05);
      hx->GetYaxis()->SetLabelSize(0.05);
      //hx->SetTitleSize(0.1);
      if(i==3) { gPad->SetLogy(); hx->Draw("bar2 text");}
      gPad->SetLeftMargin(0.15);
    }

 
 } break;//--------------------------------------


 
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
 /* use case:
     TPad *cL,*cR;   splitPadX(0.4,&cL,&cR);    cL->cd(); h->Draw()
  */
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
void doAll(TString core="quad3", TString fname="m3_Michael_R1022003") {
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
void drawFiducialLimit( TString quadName) {
  if(quadName=="quad2") {
    C=TVector2(320,2790);
    A1=TVector2(380,140);
    A2=TVector2(3000,2795);
  }
  if(quadName=="quad3") {
    C=TVector2(2800,2840);
    A1=TVector2(100,2820);
    A2=TVector2(2830,160); // (pix), quad3
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
