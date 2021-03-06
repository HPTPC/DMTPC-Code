{

   TCanvas * c1 = new TCanvas("c1","Mean and RMS",0,0,900,600);
   c1->Divide(2,1);


  TFile von("von.root");
  TFile voff("voff.root");

  TH1 * mean_von = (TH1*)von->Get("mhist");
  TH1 * mean_voff = (TH1*)voff->Get("mhist");
  TH1 * rms_von = (TH1*)von->Get("rhist");
  TH1 * rms_voff = (TH1*)voff->Get("rhist");

  Double_t scale = 1/mean_von->Integral();
  mean_von->Scale(scale);

  Double_t scale1 = 1/mean_voff->Integral();
  mean_voff->Scale(scale1);

  Double_t scale2 = 1/rms_von->Integral();
  rms_von->Scale(scale2);

  Double_t scale3 = 1/rms_voff->Integral();
  rms_voff->Scale(scale3);

  c1->cd(1);
  mean_von->Draw();
  mean_von->SetLineColor(kBlue);
  mean_voff->Draw("same");
  mean_voff->SetLineColor(kRed);

  TLegend *legend = new TLegend(0.4,0.8,0.9,0.7);
  legend->AddEntry(mean_von,"Voltage-on data","l");
  legend->AddEntry(mean_voff,"Voltage-off data","l");
  legend->Draw();
  
  c1->cd(2);
  rms_von->Draw();
  rms_von->SetLineColor(kBlue);
  rms_voff->Draw("same");
  rms_voff->SetLineColor(kRed);

  TLegend *legend1 = new TLegend(0.4,0.8,0.9,0.7);
  legend1->AddEntry(rms_von,"MaxCam method","l");
  legend1->AddEntry(rms_voff,"Manual method","l");
  legend1->Draw();

  c1->Update();

}
