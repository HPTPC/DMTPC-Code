{

  //TF1* f1 = new TF1("f1", "poln");                                                                                                       


  TCanvas * c1 = new TCanvas("c1","Residual Histogram",0,0,700,600);
  //c1->Divide(2,2,0,0);


  TFile m3_gain_R1034039("m3_gain_R1034039.root");
  // TFile light_ratio_bias("light_ratio_bias.root");
  TFile m3_gain_R1029052("m3_gain_R1029052.root");

  TH2D * ped_old = (TH2D*)m3_gain_R1034039->Get("cam3_pedRms");
  // TH2D * ped_old = (TH2D*)light_ratio_bias->Get("biasosavg1");
  TH2D * ped_new = (TH2D*)m3_gain_R1029052->Get("cam3_pedRms");

  TH2D* residual = (TH2D*)ped_new->Clone();
  residual->Add(ped_old,-1);
 
  c1->cd();
  residual->SetXTitle("X Pixels");
  residual->SetYTitle("Y Pixels");
  //residual->SetMaximum(2300);
  //residual->SetMinimum(-100);
  residual->Draw("colz");
  c1->Update();
 
  TCanvas *c = new TCanvas("c","",0,0,900,300);
  c->Divide(2,1);
  c->cd(1);
  residual->ProjectionX("ProjX", 0, -1)->Draw();
  c->cd(2);
  residual->ProjectionY("ProjY", 0, -1)->Draw();
  c->Update();

}
