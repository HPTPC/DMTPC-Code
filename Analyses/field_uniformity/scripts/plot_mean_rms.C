{

  gStyle->SetOptStat(0);

  bool cut = false;
  
  //  TFile f("output/plot_pix_intensity/nomeansub/all.root");
  //    TFile f("output/plot_pix_intensity/nomeansub/darkdata/all.root");
  //cam 2
  TFile f("/scratch2/gmuraru/dmtpc_software/Analyses/field_uniformity/output/testband/0flush4x4.root");
  TTree * t = (TTree*)f.Get("pix_t");
  
  double rms = 0;
  double mean = 0;
  double chi = 0;
  TH1D * pixhist = 0;
  int camid = 0;
  int runnum = 0;
  int nevents = 0;


  t->SetBranchAddress("pixhist",&pixhist);
  t->SetBranchAddress("mean",&mean);
  t->SetBranchAddress("rms",&rms);
  t->SetBranchAddress("camid",&camid);
  t->SetBranchAddress("runnum",&runnum);
  t->SetBranchAddress("nevents",&nevents);
  //  t->SetBranchAddress("chi",&chi);
  t->SetBranchAddress("chi",&chi);

  TFile *m0flush4x4 = new TFile("m0flush4x4.root","RECREATE");
  /*TH1D *mean_von = new TH1D("mean_von","",100, -5, 5);
  TH1D *rms_von = new TH1D("rms_von","",100, 10, 14);

  mean_von->Fill(mean,"mean > -500");
  mean_von->Write();
  rms_von->Fill(rms);
  rms_von->Write();*/

  TCanvas c2;
  c2->Divide(2);
  c2->cd(1);
  t->Draw("mean>>mhist","","goff");
  mhist->SetTitle("Image Mean");
  mhist->SetXTitle("Image Mean");
  mhist->Draw();
  mhist->Write();
  c2->cd(2);
  t->Draw("rms>>rhist","","goff");
  rhist->SetTitle("Image RMS");
  rhist->SetXTitle("Image RMS");
  rhist->Draw();
  rhist->Write();
  c2->Update();

}
  
