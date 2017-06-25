int histogram_test_quick(const char * fname, int ev)
{


    return histogram_test(fname,ev,true,false,1,new TCanvas ("c1","readout",800,600)) + histogram_test(fname,ev,false,false,0.01,new TCanvas("c2","avalanche",800,600)); 

}

int histogram_test(const char* fname, int ev, bool readout = false, bool random = false, double acceptance = 1,  TCanvas *c = 0)
{


  TFile f(fname); 
  dmtpc::analysis::MCTruth * truth = 0; 
 
  TTree * sim = (TTree* ) f.Get("Simulation"); 

  sim->SetBranchAddress("truth",&truth); 

  sim->GetEntry(ev); 
  printf("z0: %f\n", truth->z0); 


  TRandom3 rand(0); 
  int cam = dmtpc::analysis::truthalgo::maincam(truth);
  TH2 * h  = readout ? truth->getTruthImage(cam) : truth->avalanche_visible_photon_xy; 

  if (readout) h = dmtpc::image::transform::cropZero(h); 

//  if (readout) 
//  {
//    h->GetXaxis()->SetLimits(-h->GetNbinsX()/2. / truth->xscale[cam] * h->GetXaxis()->GetBinWidth(1) + truth->xoffsets[cam], h->GetNbinsX()/2. / truth->xscale[cam] * h->GetXaxis()->GetBinWidth(1) + truth->xoffsets[cam]); 
//    h->GetYaxis()->SetLimits(-h->GetNbinsY()/2. / truth->yscale[cam] * h->GetYaxis()->GetBinWidth(1) + truth->yoffsets[cam], h->GetNbinsY()/2. / truth->yscale[cam] * h->GetYaxis()->GetBinWidth(1) + truth->yoffsets[cam]); 
//  }
  double xmean,ymean;
//  double x0 = truth->x0;
//  double y0 = truth->y0;
  double x0 = readout ? truth->x0cam(cam) : truth->x0; 
  double y0 = readout ? truth->y0cam(cam) : truth->y0; 
  double phi = dmtpc::analysis::truthalgo::pca_hist(h,&xmean,&ymean); 
  printf("(xmean,ymean) (x0,y0) (xmean - x0, ymean -y0): (%f,%f) (%f,%f) (%f,%f) \n",xmean,ymean,x0,y0, xmean-x0, ymean - y0); 

  double mean_phi = atan2(ymean-y0, xmean- x0); 
  printf("phi,mean_phi: %f,%f\n",phi,mean_phi); 
  bool sign = cos(phi-mean_phi) > 0; 
  printf("same_sign: %s\n", sign ? "true" : "false"); 
  if (!sign) phi = dmtpc::math::angle::normalize(phi + TMath::Pi()); 

  printf("phi after flip: %f\n",phi); 

  if (!c) c = new TCanvas("asymmetry_test","Asymmetry Test", 1000,500); 
  c->Clear(); 

  c->Divide(2,1); 

  c->cd(1); 

  int npercentile = 1; 
  double percentile[] = {0}; 
  int asymmetry_electrons; 

  TF1  debug_f;
  TH1F  debug_h;
  double asymmetry_electrons_line = dmtpc::analysis::truthalgo::hist_asymmetry(h, phi, npercentile, percentile, &asymmetry_electrons, random ? &rand : 0, acceptance, &debug_f, &debug_h); 
  printf("%f\n",asymmetry_electrons_line); 

  debug_h.SetTitle(TString::Format("Projection along 2-D Principal Component (acceptance=%0.03f)",acceptance)); 
  debug_h.Scale(1./debug_h.GetBinWidth(1)); 
  debug_h.DrawCopy(); 
  debug_f.DrawCopy("same"); 

  c->cd(2); 

  if (readout)
  {

  h->GetXaxis()->SetTitle("pixels"); 
  h->GetYaxis()->SetTitle("pixels"); 

  }
  else
  {
  h->GetXaxis()->SetTitle("mm"); 
  h->GetYaxis()->SetTitle("mm"); 
  }
  h->DrawCopy("colz"); 

  if(readout) delete h; 
  double scale = readout ? 10 : 1; 

  TArrow * arrow_truth= new TArrow(x0, y0, x0 + scale*truth->dx,y0 + scale*truth->dy,0.5,">"); 
  TArrow * arrow_pca= new TArrow(xmean, ymean, xmean + scale*cos(phi), ymean + scale*sin(phi),0.5,">"); 
  arrow_truth->SetLineColor(4); 
  arrow_truth->Draw("same"); 
  arrow_pca->SetLineColor(2); 
  arrow_pca->Draw("same"); 

  TMarker * mark = new TMarker(x0,y0,8); 
  mark->SetMarkerColor(4); 
  mark->Draw("same"); 

  TMarker * mark_pca = new TMarker(xmean, ymean, 8); 
  mark_pca->SetMarkerColor(2); 
  mark_pca->Draw("same"); 


}

int asymmetry_test(const char * fname, int ev, bool diffused = false, bool flat = true, bool fast_pca = false, TCanvas * c = 0)
{
  TFile f(fname); 

  dmtpc::analysis::MCTruth * truth = 0; 
 
  TTree * sim = (TTree* ) f.Get("Simulation"); 

  sim->SetBranchAddress("truth",&truth); 

  sim->GetEntry(ev); 
  printf("z0: %f\n", truth->z0); 

  dmtpc::math::PrincipalComponentFinder pca(flat ? 2 : 3, fast_pca); 

  TVector3 electrons_dir;  
  if (flat)
    electrons_dir = dmtpc::analysis::truthalgo::pca_electrons_flat(truth, &pca, diffused)->asVec() ;
  else
    electrons_dir = dmtpc::analysis::truthalgo::pca_electrons(truth, &pca, diffused)->asVec() ; 

  electrons_dir.Print(); 
  TVector3 electrons_mean(pca.getMean(0), pca.getMean(1),flat ? 0 :  pca.getMean(2)); 
  TVector3 startpos(truth->x0, truth->y0, flat ? 0 : truth->z0); 
  TVector3 electrons_mean_transformed  =  electrons_mean - startpos; 
  bool electrons_sign = cos(electrons_mean_transformed.Angle(electrons_dir)) > 0; 
  if (!electrons_sign) electrons_dir *= -1; 


  TF1  debug_f;
  TH1F  debug_h;

  int npercentile = 1; 
  double percentile[] = {0}; 
  int asymmetry_electrons; 

  double asymmetry_electrons_line = dmtpc::analysis::truthalgo::electron_asymmetry(truth, &electrons_dir, diffused,flat, npercentile, percentile, &asymmetry_electrons, &debug_f, &debug_h);
  cout << asymmetry_electrons_line << endl; 

  
  if (!c) c = new TCanvas("asymmetry_test","Asymmetry Test", 1000,500); 

  c->Clear(); 
  c->Divide(2,1); 
  c->cd(1); 
  debug_h.SetTitle(TString::Format("Projection along %s Principal Component",flat ? "2-D" : "3-D"));
  debug_h.Scale(1./debug_h.GetBinWidth(1)); 
  debug_h.DrawCopy(); 
  debug_f.DrawCopy("same"); 
  c->cd(2); 

  sim->Draw(diffused? "diffused_electron_y:diffused_electron_x" : "primary_electron_y:primary_electron_x",TString::Format("Entry$==%d",ev),"colz"); 

  TH2 *htemp = (TH2*)gPad->GetPrimitive("htemp");
  htemp->SetTitle(TString::Format("%s Electrons (x-y projection)", diffused ? "Diffused " : "Primary"));  
  htemp->GetXaxis()->SetTitle("mm"); 
  htemp->GetYaxis()->SetTitle("mm"); 
  TMarker * mark = new TMarker(truth->x0,truth->y0,8); 
  mark->SetMarkerColor(4); 
  mark->Draw("same"); 
  TArrow * arrow_truth= new TArrow(truth->x0, truth->y0, truth->x0 + truth->dx, truth->y0 + truth->dy,0.5,">"); 
  arrow_truth->SetLineColor(4); 
  arrow_truth->Draw("same"); 
  TArrow * arrow_pca= new TArrow(electrons_mean.x(), electrons_mean.y(), electrons_mean.x() + electrons_dir.x(), electrons_mean.y() + electrons_dir.y(),0.5,">"); 
  arrow_pca->SetLineColor(2); 
  arrow_pca->Draw("same"); 

  TMarker * mark_pca = new TMarker(electrons_mean.x(), electrons_mean.y(), 8); 
  mark_pca->SetMarkerColor(2); 
  mark_pca->Draw("same"); 

  return 0; 

}
