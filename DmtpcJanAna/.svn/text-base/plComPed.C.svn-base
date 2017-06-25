TString pdfName="";


plComPed(  int page=2, TString fname="sumOff", TString core="quad3", char *inpPath="out/", char *oPath=""){

  int squareNpix=700;
  
 
  char *nameC[]={"cal_UpedAvr","cal_UpedRms","cal_pedRes1"}; // pg 3
  char *nameD[]={"cal_sumLg"}; // pg 4
  //pg 5  ped residua
  // pg 6 diff ped residua
  // pg 7 diff ped residuafinal com-ped corr using A,B


  TString fullInpName=inpPath;  fullInpName+="/";fullInpName+=fname;
  if(!fullInpName.Contains("root") )fullInpName+=".comPed.root";
  fd=new TFile(fullInpName);
  if(! fd->IsOpen()) {
    printf("EROR: input histo file not found, quit\n",fullInpName.Data());
    return;
  } else {
    printf("Opened: %s  core=%s\n",fullInpName.Data(),core.Data());
  }
  
  // fd->ls();

  gStyle->SetPalette(1,0);
  gStyle->SetOptFit(1);
  gStyle->SetOptStat(1);

  TString  padTit=Form("%s",fname.Data());
  printf("=%s=\n",padTit.Data());
  
  switch (page) {
    
 
 case 3:{
   can=new TCanvas("aa","aa",squareNpix,1.05*squareNpix);
   TPad *c=makeTitle(can,core+" frame info, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(1110);  
    c->Divide(2,2);     
    TH1 *hx=0;
    char **nameX=nameC; 
    for(int i=0;i<3;i++) {
      printf(" draw=%s=\n",nameX[i]);
      hx=(TH1*)fd->Get(core+nameX[i]);   assert(hx);
      c->cd(i+1);
      hx->Draw("colz");
      //  if(i==4) gPad->SetLogx();
      gPad->SetGrid();
    }
    
  } break;//--------------------------------------

  case 4:{
   can=new TCanvas("aa","aa",squareNpix,1.0*squareNpix);
   TPad *c=makeTitle(can,core+" sum of light, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(0);  
    c->Divide(1,1);     
    TH2*hx=0;
    int nReb=8;
    char **nameX=nameD; 
    for(int i=0;i<1;i++) {
      printf(" draw=%s=\n",nameX[i]);
      hx=(TH2D*)fd->Get(core+nameX[i]);   assert(hx);
      c->cd(i+1);
      hx->Draw("colz");
      //hx->Rebin2D(nReb,nReb);
      gPad->SetRightMargin(0.15);
      float yy=20;
      hx->SetMinimum(-yy/4);
      hx->SetMaximum(yy);
    }
    
  } break;//--------------------------------------


 
  case 5:{
   can=new TCanvas("aa","aa",1.7*squareNpix,0.7*squareNpix);
   TPad *c=makeTitle(can,core+" ped residua, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(0);  

    leg = new TLegend(0.85,0.85,0.99,0.99);
    //    leg->SetHeader("ped shape issues");

    enum {NT=4}; // types of status
    int colA[NT]={1,2,4,8};
    int symbA[NT]={26,4,5,3};

    TH1 *hx=0;
    float errA=0;

    for(int i=0;i<2*2;i++) {
      TString hName=core+Form("cal_comPed%c",'A'+i);
      printf(" draw=%s=\n",hName.Data());
      hx=(TH1*)fd->Get(hName);   assert(hx);
      if(i==0) {
	hx->Draw();
	errA=hx->GetBinError(1);
      }
      else
	hx->Draw("same");
      hx->SetMarkerStyle(symbA[i]); 
      hx->SetMarkerColor(colA[i]);
      leg->AddEntry(hx,Form("region %c",'A'+i),"pel");
    }
    gPad->SetGrid();
    leg->Draw();
    TLine *ln=new TLine(0,0,200,0);    ln->Draw();
    float y1=3*errA;
    ln=new TLine(0,y1,200,y1);   ln->Draw();
    ln->SetLineColor(kMagenta);
    ln=new TLine(0,-y1,200,-y1);   ln->Draw();
    ln->SetLineColor(kMagenta);
    
  } break;//--------------------------------------


  case 6:{
    can=new TCanvas("aa","aa",1.7*squareNpix,0.7*squareNpix);
    TPad *c=makeTitle(can,core+" diff of ped residua, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(0);  

    TH1D *hA= extractTH1D( ( TProfile*)fd->Get(core+"cal_comPedA")); assert(hA);
    TH1D *hB=extractTH1D( ( TProfile*)fd->Get(core+"cal_comPedB"));  assert(hB);

    TH1D *hd1=(TH1D*) hA->Clone();
    hd1->SetTitle("difference betwen profiles");
    
    hd1->Add(hB, -1.);
    hd1->Draw();
    int bb=1;
    printf("Err A=%f  B=%f d1=%f \n", hA->GetBinError(bb),hB->GetBinError(bb),hd1->GetBinError(bb));
    float errA=hd1->GetBinError(bb);
    TLine *ln=new TLine(0,0,200,0);    ln->Draw();
    float y1=3*errA;
    ln=new TLine(0,y1,200,y1);   ln->Draw();
    ln->SetLineColor(kMagenta);
    ln=new TLine(0,-y1,200,-y1);   ln->Draw();
    ln->SetLineColor(kMagenta);
    hd1->Fit("pol0");
     
 } break;//--------------------------------------
 

  case 7:{
    can=new TCanvas("aa","aa",1.9*squareNpix,0.5*squareNpix);
    TPad *c=makeTitle(can,core+" A+B com-ped-corr, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(0);  
    
  
     TProfile *prA=  ( TProfile*)fd->Get(core+"cal_comPedA"); assert(prA);
     TProfile *prB=  ( TProfile*)fd->Get(core+"cal_comPedB"); assert(prA);
     TProfile *prS=  ( TProfile*)prA->Clone();
     prS->SetTitle("Comm ped correction");

     prS->SetMinimum(-.3);prS->SetMaximum(.1);
     gPad->SetGrid();
     prS->Add(prB,1.);
     // prS->Rebin(4);

     TH1D *hCR= extractTH1D( prS);
     hCR->SetMarkerStyle(4);
     hCR->SetMarkerColor(kRed);

     prS->SetFillColor(kYellow);
     prS->Draw("h e");

     // apply threshold to compute corrections
     int bb=1; 
     float par_nSig=3;
     float errBand= par_nSig* prS->GetBinError(bb);
     const int nbinsx =  prS->GetNbinsX();
     for(int k=1; k<= nbinsx; k++){
       if(fabs( prS->GetBinContent(k)) > errBand) continue;
       hCR->SetBinContent(k,0);
       hCR->SetBinError(k,0);
     }
     
      hCR->Draw("same e ");
      //prS->Fit("pol6","","R",0,40);
     
     TLine *ln=new TLine(0,0,200,0);    ln->Draw();
     float y1= errBand;
     ln=new TLine(0,y1,200,y1);   ln->Draw();
     ln->SetLineColor(kMagenta);
     ln=new TLine(0,-y1,200,-y1);   ln->Draw();
     ln->SetLineColor(kMagenta);
   
     
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
TH1D* extractTH1D( TProfile * hpr) {
  assert(hpr);
  const int nbinsx =  hpr->GetNbinsX();
  const double xmin = hpr->GetXaxis()->GetXmin();
  const double xmax = hpr->GetXaxis()->GetXmax();
  TString tit1=hpr->GetName();
  TString tit2=hpr->GetTitle();

  TH1D* h=new TH1D("h"+tit1, "h"+tit2, nbinsx, xmin,xmax);
  h->Sumw2();
  for(int k=1; k<= nbinsx; k++){
    h->SetBinContent(k, hpr->GetBinContent(k));
    h->SetBinError(k, hpr->GetBinError(k));
  }
  
  h->Draw();
  return h;
}


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

