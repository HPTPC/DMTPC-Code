

plPixQA(  TString hname="pixR1019021Cam0x1639y1542"){

  TString fname="m3_Michael_R1019021";
  char *inpPath="./";
  char *oPath="";

  int squareNpix=800;
  int inSel=-2015;
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
 
 
  c=new TCanvas("aa","aa",squareNpix, squareNpix*.7);
 
  gStyle->SetOptStat(0);  
  hx=(TH1*)fd->Get(hname);   assert(hx);
  hy=hx->DrawClone();
  //
  c->Divide(1,2);
  c->cd(1);  
  hy->Draw();
  c->cd(2);
  hx->SetAxisRange(0,100);
  hx->SetMaximum(120);
  hx->SetMinimum(-120);
  hx->Draw();
  
  TBox *bx=new TBox(15,-200,16,200);
  bx->Draw();
  bx->SetFillColor(kYellow);

  hx->Fit("pol1","","R",16.5,25);

  gStyle->SetOptFit(1);
}

