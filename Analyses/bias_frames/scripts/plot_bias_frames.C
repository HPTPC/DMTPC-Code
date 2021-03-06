{

  TString file = "/scratch2/gmuraru/dmtpc_software/Analyses/bias_frames/output/selfsub_bframes_310.root";
  TFile f(file);
  gStyle->SetOptStat(0);

  bool selfsub = true;

  const int ncam = 4;
  const int nbias = 5;
  TCanvas * c[nbias];
  TH2I * bframe[nbias][ncam];

  int biasit = 1;
  if(selfsub)
    biasit = 2;

  for(int i = 0; i < nbias; i += biasit){
    c[i] = new TCanvas(Form("c%d",i),Form("Bias Frame %d",i),1024,768);
    c[i]->Divide(2,2);
    for(int cam = 0; cam < ncam; cam++){
      bframe[i][cam] = (TH2I*)f.Get(Form("bframe_%d_%d",cam,i));
      if(cam < 3)
	bframe[i][cam]->SetMaximum(1500);
      else
	bframe[i][cam]->SetMaximum(2500);

      c[i]->cd(cam+1);
      bframe[i][cam]->Draw("colz");
    }
  }


}

    
  
