{

  gStyle->SetOptStat(0);

  bool cut = false;
  
  //  TFile f("output/plot_pix_intensity/nomeansub/all.root");
  //    TFile f("output/plot_pix_intensity/nomeansub/darkdata/all.root");
  //cam 2
  TFile f("/scratch2/gmuraru/dmtpc_software/Analyses/field_uniformity/output/test_gainmap_fe/all_von.root");
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

  //  for(int i = 0; i < t->GetEntries(); i++){
  //    t->Ge
  
  //-----draw mean and rms vs. entry number
  TCanvas c1;
  c1.Divide(2);
  c1.cd(1);
  if(cut){
    
    t->Draw("mean:Entry$>>meanhist(10100,0,10100,100,-5,5)","mean<0 && mean>-5.0","colz");
  }
  else{
    t->Draw("mean:Entry$>>meanhist(10100,0,10100,100,-5,5)","","colz");
  }
  c1.cd(2);
  if(cut){
    int nbins = 3463;//t->GetEntries("mean < 100 && mean > -100");
    t->Draw("rms:Entry$>>rmshist(10100,0,10100,100,16,22)","mean <100 && mean > -100","colz");
  }
  else{
    t->Draw("rms:Entry$>>rmshist(10100,0,10100,100,16,22)","","colz");
  }

  //--draw mean and rms vs. run number
  TCanvas c2;
  c2.Divide(2);
  c2.cd(1);
  t->Draw("mean:runnum>>mhist","mean > -500","goff");
  mhist->SetTitle("Image Mean vs. Run Number");
  mhist->SetXTitle("Run Number");
  mhist->SetYTitle("Image Mean (ADU)");
  mhist->Draw("colz");
  c2.cd(2);
  t->Draw("rms:runnum>>rhist","mean > -500","goff");
  rhist->SetTitle("Image RMS vs. Run Number");
  rhist->SetXTitle("Run Number");
  rhist->SetYTitle("Image RMS (ADU)");
  rhist->Draw("colz");

  //----plot entries 5 sigma above mean 
  TH1D * sat_thresh = new TH1D("sat_thresh","Pixel Values 3 \sigma Above Mean",200,-10,200);
  for(int i = 0; i < t->GetEntries(); i++){
    //    for(int bin = pixhist->FindBin(mean + 3*rms); bin < pixhist->GetNbinsX(); bin++){
      //      if(pixhist->GetBinLowEdge(bin) > mean + 3*rms){
    t->GetEntry(i);
    std::cout<<"mean: "<<mean<<"rms: "<<rms<<"chi: "<<chi<<std::endl;
    for(int bin = pixhist->FindBin(mean+3*rms); bin < pixhist->GetNbinsX(); bin++){
      double binlow = pixhist->GetBinLowEdge(bin);
      
      
      if(i == 0){
	std::cout<<"pixhist->GetBinLowEdge(bin): "<<binlow<<std::endl;
	std::cout<<"sat_thresh->FindBin(binlow): "<<sat_thresh->FindBin(binlow)<<std::endl;
	std::cout<<"pixhist->GetBinContent(bin): "<<pixhist->GetBinContent(bin)<<std::endl;
      }
      sat_thresh->Fill(sat_thresh->FindBin(binlow),pixhist->GetBinContent(bin));
    }
  }
  
  

  TCanvas c3;
  c3.cd();
  sat_thresh->Draw();
  
}
