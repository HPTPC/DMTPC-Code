TString pdfName="";

plPedQA(  int page=1, TString core="cam3", TString fname="m3_Michael_R1028100", char *inpPath="doneCalib/", char *oPath=""){
  core+="_";
  int squareNpix=600;
  int inSel=-2015;
  if(inSel==2015){  inpPath="./"; fname="dmtpc_m3_alpha_00198"; }
  
  char *nameA[]={"case"}; //pg 1
  // pg2 - pedStat in colors

  char *nameC[]={"peakSum_cut","peakAmpl","pedRms_cut","pedVal_cut"};// pg3

  // pg4 - for big-ped histo

  //

  char *nameD[]={"pedStat","pedPeakSum","pedPeakAmpl","pedRms","pedAvr","pedThr"};// pg20-24

  TString fullInpName=inpPath;  fullInpName+="/";fullInpName+=fname;
  if(!fullInpName.Contains("root") )fullInpName+=".m3ped.root";
  fd=new TFile(fullInpName);
  if(! fd->IsOpen()) {
    printf("EROR: input histo file not found, quit\n",fullInpName.Data());
    return;
  } else {
    printf("Opened: %s  core=%s\n",fullInpName.Data(),core.Data());
  }
  
 
  h0=(TH1D*)fd->Get(core+"case"); assert(h0);
  int inpBin=h0->GetXaxis()->FindBin("active");
  int goodBin=h0->GetXaxis()->FindBin("good");
  double totInpPix=h0->GetBinContent(inpBin);
  double totGoodPix=h0->GetBinContent(goodBin);
  printf("tot inp pix=%.2e , goode=%.2e \n",totInpPix,totGoodPix);
  if(totInpPix<=1000){
    printf("It is wrong , aborting 1\n");
    return;
  }


  if( page==1) {
    fd->ls(); 
    printf("%s bin:count   )",h0->GetName());
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
    can=new TCanvas("aa","aa",700,700);    
    TPad *c=makeTitle(can,core+"  summary, "+padTit,page); c->cd();  
    gStyle->SetOptStat(0); 
    c->Divide(1,2); 
    char **nameX=nameA;   
    for(int i=0;i<1;i++) {
      printf("->%s%s<\n",core.Data(),nameX[i]);
      h=(TH1*)fd->Get(core+nameX[i]);         assert(h);
      c->cd(i+1);      
      h->Draw(); 
      if(i==0)  { h->Draw("bar2 text");      gPad->SetTopMargin(0.20); }
     
      if(i<2)  gPad->SetLogy();
      
    }    
  }  break;//--------------------------------------


  case 2:{
    can=new TCanvas("aa","aa",squareNpix,squareNpix);
    TPad *c=makeTitle(can,core+" rejected pixels , ped QA , "+padTit,page); c->cd();
    gStyle->SetOptStat(0);
    char *nameX="pedStat";
    TH2I *h2=(TH2I*)fd->Get(core+nameX);   assert(h2);
    printf("got %s  \n",h2->GetName());

    int nbX =h2->GetNbinsX();
    int nbY =h2->GetNbinsY();
    leg = new TLegend(0.8,0.85,0.99,0.99);
    leg->SetHeader("ped shape issues");

    enum {NT=5}; // types of status
    int colA[NT]={1,2,4,8,9};
    int symbA[NT]={7,4,5,26,32};
    int statA[NT]={1,2,3,4,5};
    TString titA[NT]={"overscan","low sum","rms lo","rms hi","avr hi"};
    
    int nBad=0;
    for(int it=0;it<NT;it++){
      printf(" status type=%s\n",titA[it].Data());
      h2t=(TH2I*) h2->Clone();
      h2t->Reset();
      for(int jx=1; jx<=nbX; jx++)      
	for(int jy=1; jy<=nbY; jy++) {
	  if(statA[it]!=h2->GetBinContent(jx,jy)) continue;
	  h2t->SetBinContent(jx,jy,1); // display it	
	  if(statA[it]!=1) nBad++;	  
	}    
      if(it==0)
	h2t->Draw();
      else
	h2t->Draw("same");
      h2t->SetMarkerStyle(symbA[it]); 
      h2t->SetMarkerColor(colA[it]);
      leg->AddEntry(h2t,titA[it],"p");
    }// loop over types
    leg->Draw();
    printf(" nBad=%d (except over-scan)\n",nBad);
  } break;//--------------------------------------


  case 3:{
    can=new TCanvas("aa","aa",squareNpix, squareNpix*.7);
    TPad *c=makeTitle(can,core+" 1D spectra of cut vaues, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(1000110);  
    c->Divide(2,2);     
    TH1 *hx=0;
    char **nameX=nameC; 
    for(int i=0;i<4;i++) {
      hx=(TH1*)fd->Get(core+nameX[i]);   assert(hx);
      c->cd(i+1);
      hx->Draw();
      hx->SetFillColor(kYellow);
      gPad->SetLogy();
    }
  } break;//--------------------------------------

  case 4:{
    can=new TCanvas("aa","aa",1000,600);    
    TPad *c=makeTitle(can,core+" big ped histo, "+padTit,page); c->cd(); 
    gStyle->SetOptStat(1000110);  
    char *nameX="bigPed";
    hx=(TH1*)fd->Get(core+nameX);  
    if(hx==0) {
      fd->ls();
      printf("IGNORE: bigPed histo not fund in %s , ignore\n",fullInpName.Data());
      continue;
    }

    hx->Draw("colz");
    
    printf(" to zoom:   %s->SetAxisRange(5e5, 5e5+4000)\n",hx->GetName());
  } break;//--------------------------------------


  case 20:
  case 21:
  case 22:
  case 23:
  case 24:

    hAvr=(TH1*)fd->Get(core+"pedVal_cut"); assert(hAvr);
    hRms=(TH1*)fd->Get(core+"pedRms_cut"); assert(hRms);
    float avrPed=hAvr->GetMean();
    float rmsPed=hRms->GetMean();
    float rmsRms=hRms->GetRMS();
    printf("AAA avrPed=%.1f rme=%.1f  rmsrms=%.1f\n",avrPed, rmsPed,rmsRms);
    
    {   can=new TCanvas("aa","aa",squareNpix,squareNpix);
    TPad *c=makeTitle(can,core+" ped analysis , "+padTit,page); c->cd();
    gStyle->SetOptStat(0);
    TH1 *hx=0;
    char **nameX=nameD;
    int i=page-20;
    printf("->%s< displayed\n",nameX[i]);
    h2=(TH1*)fd->Get(core+nameX[i]);   assert(h2);
    h2->Draw("colz");
    gPad->SetRightMargin(0.15);

    if(page==23) { // rms
      int off=4*rmsRms;//adu
      h2->SetMinimum(rmsPed-off);
      h2->SetMaximum(rmsPed+off);
    }
    if(page==24) { // ped
      int fac=1;
      h2->SetMinimum(avrPed-fac*rmsPed);
      h2->SetMaximum(avrPed+fac*rmsPed);
    }
    drawFiducialLimit( core);
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
void doAll( TString core="cam3_", TString fname="m3_Michael_R1022003") {
  TString outPdf=fname+".pedQA.pdf";
  pdfName=outPdf+"(";    plPedQA(1,core,fname);  // 1st page
  pdfName=outPdf; //2.... N-1 pages
  plPedQA(2,core,fname);
  plPedQA(3,core,fname);  
  plPedQA(20,core,fname);  
  plPedQA(23,core,fname);  

  pdfName=outPdf+")";  plPedQA(24,core,fname); // last page

  
  printf("\nto display PDF execute:\n   evince %s  \n",outPdf.Data());
 
}
 
//==============================
void drawFiducialLimit( TString camName) {
  if(camName=="cam2_") {
    C=TVector2(320,2790);
    A1=TVector2(380,140);
    A2=TVector2(3000,2795);
  }
  if(camName=="cam3_") {
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



