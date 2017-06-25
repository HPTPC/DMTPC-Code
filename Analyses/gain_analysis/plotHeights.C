{
  //gSystem->Load("$MAXCAM/libMaxCam.so"); 

  int n= 6; 
  double voltages[] = {650,660,670,680,690,700}; 
  const char * files[] = {
    "trees/650top.root", 
    "trees/660top.root", 
    "trees/670top.root", 
    "trees/680top.root", 
    "trees/690top.root", 
    "trees/700top_new.root", 
  }; 


  TCut rcut(" rc > 0.005 && rc < 0.006"); 
  TCut railcut(" !rails && n_since_last_rail > 5"); 
  TCut basecut(" abs(baseline) < 50 "); 
  TCut chisqcut(" chisq < 5000 && chisq > 0 "); 
  TCut startcut ("start > 0.001 && start < 0.002"); 

  TCanvas * heights = new TCanvas("heights","heights",n*200,200); 
  heights->Divide(n,1); 

  double peaks[n]; 
  double widths[n]; 

  for (int i = 0; i < n; i++)
  {

    double v = voltages[i]; 
    TFile f(files[i]); 
    TTree * tree = (TTree*) f.Get("analysis"); 
    heights->cd(i+1); 
    TH1I * hist = new TH1I("hist","heights", 512,0,1.5); 
    tree->Draw("height >> hist", rcut && basecut && railcut && chisqcut); 
    peaks[i] = hist->GetBinCenter(hist->GetMaximumBin()); 
    hist->DrawCopy(); 
    f.Close(); 
  }


  TCanvas hv("hv","heights vs voltage", 800,800); 

  TGraph* g = new TGraph(n, voltages, peaks); 
  g->Draw("alp"); 


}
