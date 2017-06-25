{

  TCanvas* c = new TCanvas("c","",0,0, 700, 600);
 
  TCutG *alpharegion = new TCutG("alpharegion",5);
 
  alpharegion->SetVarX("");
  alpharegion->SetVarY("");
  //  alpharegion->SetTitle("Graph");

  alpharegion->SetPoint(0, 350, 450);
  alpharegion->SetPoint(1, 350, 430);
  alpharegion->SetPoint(2, 1800, 900);
  alpharegion->SetPoint(3, 1900, 1400);
  alpharegion->SetPoint(4, 350, 450);

  alpharegion->SetLineColor(kRed);
  alpharegion->SetLineWidth(2);
  //  alpharegion->Draw("alp");

  TH2F *hpxpy = new TH2F("hpxpy","py vs px",100,0,3084,100,0,3084);

  gRandom->SetSeed();
  Float_t px, py, pz;
  const Int_t kUPDATE = 1000;
  for (Int_t i = 0; i < 25000; i++) {
    gRandom->Rannor(px,py);
    pz = px*px + py*py;
    Float_t random = gRandom->Rndm(1);
    hpxpy->Fill(px,py);
  }

  Int_t i,j;
  Int_t nx = hpxpy->GetNbinsX();
  Int_t ny = hpxpy->GetNbinsY();
  Double_t area = 0;
  Double_t xc,yc;

  for (i=1; i<=nx; i++) {
    for (j=1; j<=nx; j++) {
      xc = hpxpy->GetXaxis()->GetBinCenter(i);
      yc = hpxpy->GetYaxis()->GetBinCenter(j);
      if (alpharegion->IsInside(xc, yc)) {
	area = area + hpxpy->GetXaxis()->GetBinWidth(i)*
	  hpxpy->GetYaxis()->GetBinWidth(j);
      }
    }
  }
  printf("Area defined by gcut = %d\n",area);

  c.cd(1);
  hpxpy->Draw("colz");
  alpharegion->Draw("alp, same"); 
  c.Update();
  
}
