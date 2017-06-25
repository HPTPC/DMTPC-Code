

plTrace( float yWinHalf=30 ){

  TString hname="cam3cal_lightRot";
  TString fname="m3_Michael_R1019021";
  char *inpPath="./";
  char *oPath="";

  int squareNpix=800;
  int inSel=2015;
  if(inSel==2015){  inpPath="./"; fname="dmtpc_m3_alpha_00198"; }
  
 
  TString fullInpName=inpPath;  fullInpName+="/";fullInpName+=fname;
  if(!fullInpName.Contains("root") )fullInpName+=".m3Ana.root";
  fd=new TFile(fullInpName);
  if(! fd->IsOpen()) {
    printf("EROR: input histo file not found, quit\n",fullInpName.Data());
    return;
  } else {
    printf("Opened: %s  \n",fullInpName.Data());
  }
 
  TString  padTit=Form("%s",fname.Data());
  printf("=%s=\n",padTit.Data());
 
 
  c=new TCanvas("aa","aa",squareNpix, squareNpix);

  float pixOffsetRot=4356;
  float yPixMax=300, xPixMax=1500;
  int nReb=3;

  float yEnd=9000;

  gStyle->SetOptStat(0);  
  h2=(TH2I*)fd->Get(hname);   assert(h2);
  h2->SetAxisRange(pixOffsetRot-xPixMax, pixOffsetRot+xPixMax, "x");
  h2->SetAxisRange(pixOffsetRot-yPixMax, pixOffsetRot+yPixMax, "y");
  c->Divide(1,3);
  c->cd(1);  
  h2->Draw("colz");
  gPad->SetGrid();

  // mark center of best cluster
  float Rell=40;
  TEllipse *el=new TEllipse(pixOffsetRot,pixOffsetRot,Rell*3,Rell);
  el->SetFillStyle(0); el->SetLineColor(kMagenta);
  el->Draw();

  enum {mx1d=1};
  TH1D *h1d[mx1d];
  // integrate bands and make projections
  for(int ib=0; ib<mx1d; ib++) { 
    TString bName=Form("%c_band",'A'+ib);
    float yOff=100*ib;
    float yMin=pixOffsetRot-yWinHalf +yOff;
    float yMax=pixOffsetRot+yWinHalf + yOff;
    int bin1=h2->GetXaxis()->FindBin(yMin);
    int bin2=h2->GetXaxis()->FindBin(yMax);
    printf("ib=%d  yMin=%.0f bin1=%d\n", ib,yMin,bin1);  
    TH1D*  h1=h2->ProjectionX(bName,bin1,bin2);

    h1d[ib]=h1;
    h1->Rebin(nReb);
    float sum=h1->Integral();
    TString txSum=Form("%c_band sum=%.1f kAdu",'A'+ib,sum);
    //txSum.Print();
    
    // beautification
    c->cd(1);
    TLine *ln=new TLine(0, yMin,yEnd,yMin);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2); ln->Draw();
    ln=new TLine(0, yMax,yEnd,yMax);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2); ln->Draw();
    c->cd(2);
    float xPixWin=150;
    if (ib==0) {
      h1->Draw("h e");
      h1->SetFillColor(kGreen);
      // h1->Fit("pol1","","R",pixOffsetRot-xPixWin, pixOffsetRot+xPixWin);
      TText *tx=new TText(pixOffsetRot-yWinHalf, 300,txSum); tx->Draw();
    } else {
      h1->Draw("same h e");
      h1->SetLineColor(kRed);
      TText *tx=new TText(pixOffsetRot-yWinHalf, -300,txSum); tx->Draw();
    }
    
    // compute running sume
    hi=(TH1D*) h1->Clone();
    TString bName2=Form("%c_sum",'A'+ib);
    hi->SetName(bName2);
    hi->Reset();

    c->cd(3); 
    hi->Draw();
    double sumL=0, sumL2=0;
    for(int k=1;k<h1->GetNbins(); k++) {
      printf("k=%d\n",k);
    }

  }




  return;
  gStyle->SetOptFit(1);
}

