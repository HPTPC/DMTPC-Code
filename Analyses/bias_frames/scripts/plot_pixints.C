{

  TString file = "output/bframe_pixint_310.root";
  TFile f(file);
  gStyle->SetOptStat(0);

  bool selfsub = false;

  const int ncam = 4;
  const int nbias = 10;
  TCanvas * c[nbias];
  TH1D * bframe[nbias][ncam];

  int biasit = 1;
  if(selfsub)
    biasit = 2;

  for(int i = 0; i < nbias; i += biasit){
    c[i] = new TCanvas(Form("c%d",i),Form("Pixel Intensities %d",i),1024,768);
    c[i]->Divide(2,2);
    for(int cam = 0; cam < ncam; cam++){
      bframe[i][cam] = (TH1D*)f.Get(Form("ph_%d_%d",cam,i));

      c[i]->cd(cam+1);
      bframe[i][cam]->Draw("colz");
    }
  }


}

    
  
