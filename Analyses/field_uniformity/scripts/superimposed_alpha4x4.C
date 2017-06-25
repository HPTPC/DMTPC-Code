{

   TCanvas * c1 = new TCanvas("c1","Mean and RMS",0,0,900,600);
   c1->Divide(2,1);


  TFile alphaTL("alphaTL.root");
  TFile alphaTR("alphaTR.root");
  TFile alphaBR("alphaBR.root");
  TFile alphaBL("alphaBL.root");

  TH1 * mean_TL = (TH1*)alphaTL->Get("mhist");
  TH1 * mean_TR = (TH1*)alphaTR->Get("mhist");
  TH1 * mean_BR = (TH1*)alphaBR->Get("mhist");
  TH1 * mean_BL = (TH1*)alphaBL->Get("mhist");
  TH1 * rms_TL = (TH1*)alphaTL->Get("rhist");
  TH1 * rms_TR = (TH1*)alphaTR->Get("rhist");
  TH1 * rms_BR = (TH1*)alphaBR->Get("rhist");
  TH1 * rms_BL = (TH1*)alphaBL->Get("rhist");

  Double_t scale = 1/mean_TL->Integral();
  mean_TL->Scale(scale);

  Double_t scale1 = 1/mean_TR->Integral();
  mean_TR->Scale(scale1);

  Double_t scale2 = 1/mean_BR->Integral();
  mean_BR->Scale(scale2);

  Double_t scale3 = 1/mean_BL->Integral();
  mean_BL->Scale(scale3);

  Double_t scale4 = 1/rms_TL->Integral();
  rms_TL->Scale(scale4);

  Double_t scale5 = 1/rms_TR->Integral();
  rms_TR->Scale(scale5);

  Double_t scale6 = 1/rms_BR->Integral();
  rms_BR->Scale(scale6);

  Double_t scale7 = 1/rms_BL->Integral();
  rms_BL->Scale(scale7);

  c1->cd(1);
  mean_TL->Draw();
  mean_TL->SetLineColor(kGreen);
  mean_TR->Draw("same");
  mean_TR->SetLineColor(kBlack);
  mean_BR->Draw("same");
  mean_BR->SetLineColor(kBlue);
  mean_BL->Draw("same");
  mean_BL->SetLineColor(kRed);

  TLegend *legend = new TLegend(0.4,0.8,0.9,0.7);
  legend->AddEntry(mean_TL,"TL camera","l");
  legend->AddEntry(mean_TR,"TR camera","l");
  legend->AddEntry(mean_BR,"BR camera","l");
  legend->AddEntry(mean_BL,"BL camera","l");
  legend->Draw();
  
  c1->cd(2);
  rms_BR->Draw();
  rms_BR->SetLineColor(kBlue);
  
  rms_BL->Draw("same");
  rms_BL->SetLineColor(kRed);
 
  rms_TR->Draw("same");
  rms_TR->SetLineColor(kBlack);
 
  rms_TL->Draw("same");
  rms_TL->SetLineColor(kGreen);
 

  TLegend *legend1 = new TLegend(0.4,0.8,0.9,0.7);
  legend1->AddEntry(rms_BR,"BR camera","l");
  legend1->AddEntry(rms_BL,"BL camera","l");
  legend1->AddEntry(rms_TR,"TR camera","l");
  legend1->AddEntry(rms_TL,"TL camera","l");
  legend1->Draw();

  c1->Update();

}
