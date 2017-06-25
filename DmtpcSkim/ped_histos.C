{

  //TF1* f1 = new TF1("f1", "poln");                                                                                                       


  TCanvas * c1 = new TCanvas("c1","Superimposed",0,0,600,500);
  //c1->Divide(2,2,0,0);


  TFile m3_gain_R1093001("m3_gain_R1093001.m3ped.root");


  TH2 * H1 = m3_gain_R1093001->Get("cam3_pedStat");
  //  TH2 * H2 = m3_gain_R1034039->Get("cam3_pedAvr");
  //TH2 * H3 = m3_gain_R1034039->Get("cam3_pedRms");
  //TH2 * H4 = m3_gain_R1034039->Get("cam3_pedPeakSum");
  
  c1->cd();
  // H1->Draw("colz");
  H1->GetXaxis()->SetRange(0,2048);
  H1->GetYaxis()->SetRange(0,2048);
  //c1->cd(2);
  H1->Draw("colz");
  //c1->cd(3);
  //H4->Draw();
  //c1->cd(4);
  // H4->Draw();
  c1->Update();

}
