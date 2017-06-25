// plot pedestal of selected pixel for multiple runs.

enum {mxF=16};
int runA[mxF]={1029034,1029052, 1030004,1030012,1030020,
	       1031003,1031006,1031014, 1032003,1032036,1032043,
	       1033003, 1033011,1033019, 1034003,1034011 };

TFile *fdA[mxF];


void plPixPedRun(  int xPix=200, int yPix=200, int nReb=1){
  TString inpPath="./doneCalibDay29_34/";
  //TString inpPath="./";

  // open files 
  for(int j=0; j<mxF;j++) {
    TString fname=inpPath+Form("m3_gain_R%d.m3ped.root",runA[j]);
    TFile *fd=new TFile(fname); 
    assert(fd->IsOpen());
    fdA[j]=fd;
    //printf("%d opened %s\n",j,fd->GetName());
    //break;
  }

  TString coreName=Form("pix_x%d_y%d",xPix,yPix);
  TH1F *h1=new TH1F(coreName, coreName+Form(" avr pedestal vs. run, nReb=%d; run index (day29-34); (ADU)",nReb), mxF, 0.5, mxF+0.5);
  h1->SetMarkerStyle(4);
  
  c=new TCanvas(coreName,coreName);
  populatePixel(xPix, yPix, h1,nReb);
  gStyle->SetOptFit(1);
  gStyle->SetOptStat(0);
  h1->Draw();
  h1->Fit("expo","","R",3,22.4);
  gPad->SetGrid();
  gPad->SetLeftMargin(0.15);
}

//-----------------------------------
void populatePixel( int xPix, int yPix, TH1F *h1, int nReb){  
  TString avrName="cam3_pedAvr";
  float par_nFrame=200;
  float fact=nReb*nReb;

  for(int j=0; j<mxF;j++) {
    TH2I *h2avr=fdA[j]->Get(avrName); assert(h2avr);
    h2avr->Rebin2D(nReb,nReb);
 
    int xBin=h2avr->GetXaxis()->FindBin(xPix);
    int yBin=h2avr->GetYaxis()->FindBin(yPix);

    double val=h2avr->GetBinContent(xBin, yBin)/fact;
    double err=h2avr->GetBinError(xBin, yBin)/fact/sqrt(par_nFrame);

    h1->SetBinContent(j+1,val);
    h1->SetBinError(j+1,err);

    printf("j=%d  bin:%d/%d, val=%.1f sig=%.3f R%d\n",j+1,xBin, yBin,val,err,runA[j]);
    //
     
  } 

}
