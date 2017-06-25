void bias_spark(int runs){

  gSystem->Load("/scratch2/gmuraru/dmtpc_software/DmtpcCore/lib/libDmtpcCore.so");
  gSystem->Load("/scratch2/gmuraru/dmtpc_software/DmtpcAnalysis/lib/libDmtpcAnalysis.so");

  const char * dir ="/scratch3/darkmatter/dmtpc/data/m3/2016/04/raw/";
  //int cam = 3;
  //int ncam = 1;

  int camDAQ = 3;
  int ncam = 1;

  int nbins = 1000; //int(max - min + 0.5);                                                                                         
  int runnum = 0;
  int nevents = 0;
  double rms = 0;
  double mean = 0;
  double chi = 0;
  double ccdTemp = 0;
  TH1F* intensity;
  
  TFile * bias_spark3 = new TFile("bias_spark3.root","RECREATE"); 
  TTree *t = new TTree("pix_t","Pixel Tree");
  t.Branch("intensity",&intensity);
  t.Branch("mean",&mean);
  t.Branch("rms",&rms);
  t.Branch("camDAQ",&camDAQ);
  t.Branch("runnum",&runnum);
  t.Branch("nevents",&nevents);
  t.Branch("chi",&chi);
  t.Branch("ccdTemp",&ccdTemp);
  

  for (int fi=0;fi<runs;fi++){
    d=new dmtpc::core::Dataset;
    int run=1095001;
    d->open(TString::Format("%s/m3_gain_R%06d.raw.root",dir,run+fi));

 
    //dmtpc::core::Dataset *d = new dmtpc::core::Dataset;
    //d->open("/scratch3/darkmatter/dmtpc/data/m3/2016/04/raw/m3_gain_R1095001.raw.root");
  
 
    cout << d->nbias() << endl;
    for(int i = 0; i < d->nbias(); i++){
    cout << "i=" << i <<  endl;
    d->getBiasRaw(i);
    // dmtpc::core::Image* myimage = d->biasRaw(camDAQ);                                                                                                                                               
    TH2I* myimage = d->biasRaw(camDAQ)->getHist();
    myimage->Print("b");

    ccdTemp = d->event()->ccdConfig(camDAQ)->ccdTemp;
    
    TH1F* intensity=new TH1F("intensity","intensity",100,0,5000);
    for (int m=0; m<myimage->GetNbinsX(); m++){
      for (int n=0; n<myimage->GetNbinsY(); n++) {
	float pixval=myimage->GetBinContent(m,n);
	
	intensity->Fill(pixval);
      }
    }
    
    //intensity->SetStats(kTRUE);
    //gStyle->SetOptStat(1111);
    //intensity->SetXTitle("ADU"); 
    //intensity->SetYTitle("Number of Entries");
    
    TF1 * gFit = new TF1("gFit","gaus");
    intensity->Fit(gFit);
    rms = gFit->GetParameter(2);
    mean = gFit->GetParameter(1);
    chi = gFit->GetChisquare();
    t.Fill();
    }
  }
  bias_spark3->cd();
  t->Write();
  bias_spark3->Write();
  bias_spark3->Close();
}
