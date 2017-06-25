TString pdfName="";


plBlue(  int page=1, TString core="cam3", TString fname="m3_Michael_R1022003", char *inpPath="out3/", char *oPath=""){ 

  //fname="dmtpc_m3_alpha_00198";

  fname="sum22bf";
  int squareNpix=700;
  
  char *nameA[]={"bl_eveCase","bl_camFrCase","bl_scoFrCase"}; //pg 1

  char *nameB[]={"bl_scoT","bl_scoA","bl_scoTR50","bl_cluDiag" ,"bl_cluLight","bl_cluPcaR"}; // pg 2

  char *nameC[]={"bl_eveL_P","bl_eveCMxy","bl_MuOk"}; // pg 3

  char *nameD[]={"bl_scoTok","bl_scoAok","bl_cluDiagOk" ,"bl_cluLgOk","bl_cluPcaROk"}; // pg 4

  TString fullInpName=inpPath;  fullInpName+="/";fullInpName+=fname;
  if(!fullInpName.Contains("root") )fullInpName+=".m3blue.root";
  fd=new TFile(fullInpName);
  if(! fd->IsOpen()) {
    printf("EROR: input histo file not found, quit\n",fullInpName.Data());
    return;
  } else {
    printf("Opened: %s  core=%s\n",fullInpName.Data(),core.Data());
  }
  
  // fd->ls();

  h0=(TH1D*)fd->Get(core+"bl_eveCase"); assert(h0);

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
    can=new TCanvas("aa","aa",squareNpix,1.2*squareNpix);
    TPad *c=makeTitle(can,core+"  summary, "+padTit,page); c->cd();   
    c->Divide(1,3); 
    gStyle->SetOptStat(0);
    char **nameX=nameA;   
    for(int i=0;i<3;i++) {
      printf("->%s%s<\n",core.Data(),nameX[i]);
      h=(TH1*)fd->Get(core+nameX[i]);         assert(h);
      c->cd(i+1);      
      h->Draw("bar2 text");      
      gPad->SetTopMargin(0.20); 
      gPad->SetLogy();      
    }    
  }  break;//--------------------------------------

 case 2:{
   can=new TCanvas("aa","aa",1.5*squareNpix,squareNpix);
   TPad *c=makeTitle(can,core+" frame info, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(1110);  
    c->Divide(3,2);     
    TH1 *hx=0;
    char **nameX=nameB; 
    for(int i=0;i<6;i++) {
      printf(" draw=%s=\n",nameX[i]);
      hx=(TH1*)fd->Get(core+nameX[i]);   assert(hx);
      c->cd(i+1);
      hx->Draw("colz");
      if(i==3) gPad->SetLogy();
      gPad->SetGrid();
    }
    
  } break;//--------------------------------------



  case 3:{
    can=new TCanvas("aa","aa",squareNpix,1.05*squareNpix);
    TPad *c=makeTitle(can,core+" accapted, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(10);  
    c->Divide(2,2);     
    TH1 *hx=0;
    char **nameX=nameC; 
    for(int i=0;i<3;i++) {
      printf(" draw=%s=\n",nameX[i]);
      hx=(TH1*)fd->Get(core+nameX[i]);   assert(hx);
      c->cd(i+1);
      hx->Draw("colz");
      gPad->SetGrid();
    }
  } break;//--------------------------------------

 
 case 4:{
   can=new TCanvas("aa","aa",squareNpix,1.05*squareNpix);
   TPad *c=makeTitle(can,core+" final accepted, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(1110);  
    c->Divide(2,2);     
    TH1 *hx=0;
    char **nameX=nameD; 
    for(int i=0;i<4;i++) {
      printf(" draw=%s=\n",nameX[i]);
      hx=(TH1*)fd->Get(core+nameX[i]);   assert(hx);
      c->cd(i+1);
      hx->Draw("colz");
      if(i==2) gPad->SetLogy();
      if(i==0) hx->Rebin(3);
      gPad->SetGrid();
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
