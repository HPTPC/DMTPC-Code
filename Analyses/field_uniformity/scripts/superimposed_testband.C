{

   TCanvas * c1 = new TCanvas("c1","Mean and RMS",0,0,900,600);
   c1->Divide(2,1);


  TFile m0flush4x4("m0flush4x4.root");
  TFile m1flush4x4("m1flush4x4.root");
  TFile m2flush4x4("m2flush4x4.root");
 
  TH1 * mean_0flush4x4 = (TH1*)m0flush4x4->Get("mhist");
  TH1 * mean_1flush4x4 = (TH1*)m1flush4x4->Get("mhist");
  TH1 * mean_2flush4x4 = (TH1*)m2flush4x4->Get("mhist");
 
  TH1 * rms_0flush4x4 = (TH1*)m0flush4x4->Get("rhist");
  TH1 * rms_1flush4x4 = (TH1*)m1flush4x4->Get("rhist");
  TH1 * rms_2flush4x4 = (TH1*)m2flush4x4->Get("rhist");
 

  Double_t scale = 1/mean_0flush4x4->Integral();
  mean_0flush4x4->Scale(scale);

  Double_t scale1 = 1/mean_1flush4x4->Integral();
  mean_1flush4x4->Scale(scale1);

  Double_t scale2 = 1/mean_2flush4x4->Integral();
  mean_1flush4x4->Scale(scale2);

  Double_t scale4 = 1/rms_0flush4x4->Integral();
  rms_0flush4x4->Scale(scale4);

  Double_t scale5 = 1/rms_1flush4x4->Integral();
  rms_1flush4x4->Scale(scale5);

  Double_t scale6 = 1/rms_2flush4x4->Integral();
  rms_2flush4x4->Scale(scale6);

  c1->cd(1);
  mean_0flush4x4->Draw();
  mean_0flush4x4->SetLineColor(kBlue);
  mean_1flush4x4->Draw("same");
  mean_1flush4x4->SetLineColor(kRed);
  mean_2flush4x4->Draw("same");
  mean_2flush4x4->SetLineColor(kBlack);
 
  TLegend *legend = new TLegend(0.4,0.8,0.9,0.7);
  legend->AddEntry(mean_0flush4x4,"0 flush","l");
  legend->AddEntry(mean_1flush4x4,"1 flush","l");
  legend->AddEntry(mean_2flush4x4,"2 flushes","l");
  //legend->AddEntry(mean_BL,"BL camera","l");
  legend->Draw();
  
  c1->cd(2);
  rms_0flush4x4->Draw();
  rms_0flush4x4->SetLineColor(kBlue);
  
  rms_1flush4x4->Draw("same");
  rms_1flush4x4->SetLineColor(kRed);
 
  rms_2flush4x4->Draw("same");
  rms_2flush4x4->SetLineColor(kBlack);
 
 
  TLegend *legend1 = new TLegend(0.4,0.8,0.9,0.7);
  legend1->AddEntry(rms_0flush4x4,"0 flush","l");
  legend1->AddEntry(rms_1flush4x4,"1 flush","l");
  legend1->AddEntry(rms_1flush4x4,"2 flushes","l");
  // legend1->AddEntry(rms_TL,"TL camera","l");
  legend1->Draw();

  c1->Update();

}
