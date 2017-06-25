{

 
  TCanvas * c1 = new TCanvas("c1","Residual Histogram",0,0,700,600);
  TCanvas * c2 = new TCanvas("c2","Averaged voltage-on images",0,0,700,600);
  TCanvas * c3 = new TCanvas("c3","Averaged voltage-off images",0,0,700,600);
 
  //TFile sumsCs("sums.root");
  TFile alpha_adu2("alpha_adu2.root");
  TFile alpha_bias_adu("alpha_bias_adu.root");

  TH2D * summed_light = (TH2D*)alpha_adu2->Get("sumImage");
  // TH2D * ped_old = (TH2D*)light_ratio_bias->Get("biasosavg1");
  TH2D * summed_bias = (TH2D*)alpha_bias_adu->Get("biasImage");

  TH2D * residual = (TH2D*)summed_light->Clone();
  residual->Add(summed_bias,-1.);

  TCutG *alpharegion = new TCutG("alpharegion",5);
  alpharegion->SetVarX("");
  alpharegion->SetVarY("");
  alpharegion->SetPoint(0, 350, 450);
  alpharegion->SetPoint(1, 350, 430);
  alpharegion->SetPoint(2, 1800, 900);
  alpharegion->SetPoint(3, 1900, 1400);
  alpharegion->SetPoint(4, 350, 450);

  alpharegion->SetLineColor(kRed);
  alpharegion->SetLineWidth(2);

  Int_t i,j;
  Int_t nx = residual->GetNbinsX();
  Int_t ny = residual->GetNbinsY();
  Double_t area = 0;
  Double_t xc,yc;

  for (i=0; i<=nx; i++) {
    for (j=0; j<=ny; j++) {
      xc = residual->GetXaxis()->GetBinCenter(i);
      yc = residual->GetYaxis()->GetBinCenter(j);
      if (alpharegion->IsInside(xc, yc)) {
	area = area + residual->GetXaxis()->GetBinWidth(i)*
	  residual->GetYaxis()->GetBinWidth(j);
      }
    }
  }

  Double_t cut_ADU = alpharegion->IntegralHist(residual);
  cout << "ADU count = " << cut_ADU << endl;
  printf("Area defined by alpharegion = %d\n",area);
  //biasImage->Draw(Form("[%s]", alpharegion->GetName()));                                                   \
                                                                                                                  
  // alpharegion->Draw("alp, same");
  //  residual->Draw("colz [alpharegion]");

  int bin = residual->FindBin(1500,1500);
  cout << "residual bin = " << residual->GetBinContent(bin) << endl;
  cout << "ADU = " << residual->Integral() << endl;
 
  c1->cd();
  residual->SetXTitle("X Pixels");
  residual->SetYTitle("Y Pixels");
  residual->Rebin2D(2);
  residual->SetMaximum(150);
  residual->SetMinimum(-100);
  residual->Draw("colz");
  c1->Update();

  c2->cd();
  summed_light->SetXTitle("X Pixels");
  summed_light->SetYTitle("Y Pixels");
  //  summed_light->Rebin2D(2);
  //  summed_light->SetMaximum(7350);
  //summed_light->SetMinimum(-50);
  summed_light->Draw("colz");
  c2->Update();

  c3->cd();
  summed_bias->SetXTitle("X Pixels");
  summed_bias->SetYTitle("Y Pixels");
  // summed_bias->SetMaximum(2300);
  //summed_bias->SetMinimum(-50);
  summed_bias->Draw("colz");
  c3->Update();
 
  TCanvas *c = new TCanvas("c","",0,0,900,300);
  c->Divide(2,1);
  c->cd(1);
  residual->ProjectionX("ProjX", 0, -1)->Draw();
  c->cd(2);
  residual->ProjectionY("ProjY", 0, -1)->Draw();
  c->Update();

}
