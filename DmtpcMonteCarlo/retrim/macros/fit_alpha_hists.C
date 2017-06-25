void fithists(const char * collfile, const char * out,int pressure = 60, double nbins=100, double binsize=2.5e-2)
{
  gSystem->Load("lib/libretrim.so"); 
  dmtpc::mc::retrim::CollisionReader c(collfile); 
  dmtpc::mc::retrim::TableReader * tables[3]; 
  tables[0] = new dmtpc::mc::retrim::TableReader(TString::Format("data/srim/f_in_cf4_%dtorr.txt",pressure)); 
  tables[1] = new dmtpc::mc::retrim::TableReader(TString::Format("data/srim/c_in_cf4_%dtorr.txt",pressure)); 
  tables[2] = new dmtpc::mc::retrim::TableReader(TString::Format("data/srim/he_in_cf4_%dtorr.txt",pressure)); 

  dmtpc::mc::retrim::TrackMaker *t = new dmtpc::mc::retrim::TrackMaker(&c,3,tables,0.77); 

  TFile of(out,"RECREATE"); 

  double skewness, slope, diff,diff1,integral, slope_error, chisq,s0,s1, phi, diff_phi, diff_phi0, phi_err; 
  double max_x, min_x, min_xrot, max_xrot, xmiddle,middle; 
  int nelec;
  TTree * tree = new TTree("fit","Fits"); 
  tree->Branch("skewness",&skewness); 
  tree->Branch("slope",&slope); 
  tree->Branch("slope_error",&slope_error); 
  tree->Branch("nelec",&nelec); 
  tree->Branch("diff",&diff); 
  tree->Branch("diff1",&diff1); 
  tree->Branch("integral",&integral); 
  tree->Branch("chisq",&chisq); 
  tree->Branch("s0",&s0); 
  tree->Branch("s1",&s1); 
  tree->Branch("phi",&phi); 
  tree->Branch("diff_phi",&diff_phi); 
  tree->Branch("diff_phi0",&diff_phi0); 
  tree->Branch("xmax",&max_x); 
  tree->Branch("xmiddle",&xmiddle); 
  tree->Branch("middle",&middle); 
  tree->Branch("xmin",&min_x); 
  tree->Branch("xmax_rot",&max_xrot); 
  tree->Branch("xmin_rot",&min_xrot); 

  TH1 * total = new TH1F("total","Total Ionization",nbins,0,binsize*nbins); 
  t->enableHistograms(binsize,nbins); 
  t->enableLeftoverCut(0.5); 
  tree->Branch("hist",&total); 

  TGraph * elec_xy = 0; 
  tree->Branch("elec_xy",&elec_xy);  

  TF1 line("line","pol1"); 
  TF1 elecline("elecline","pol1"); 
  TRandom3 rand; 
  dmtpc::mc::retrim::SimpleIonizationModel ion; 

  for (int i = 0; i < c.nTracks(); i++)
  {
    t->makeTrack(i); 
    total->Reset(); 
    total->Add(t->getIonizationIon()); 
    total->Add(t->getIonizationRecoils()); 

    nelec = t->makeElectrons(&ion); 
    
    elec_xy = new TGraph(nelec); 
    for (int e = 0; e < nelec; e++)
    {
      elec_xy->SetPoint(e, t->getElectronX(e)->X(), t->getElectronX(e)->Y()); 
    }

    elec_xy->Fit(&elecline,"NRQ"); 

    phi = atan(elecline.GetParameter(1)); 

    double elec_line_b = elecline.GetParameter(0); 

    double *xrot = new double[nelec]; 


    min_xrot = 1e100;
    max_xrot = -1e00; 
    max_x = -1e00; 
    min_x = 1e100; 
    for (int e = 0; e < nelec; e++)
    {
      double x = elec_xy->GetX()[e]; 
      double y = elec_xy->GetY()[e]; 
      xrot[e] = x* cos(phi) + y * sin(phi);  
      if (xrot[e] > max_xrot) max_xrot = xrot[e]; 
      if (xrot[e] < min_xrot) min_xrot = xrot[e]; 
      if (x > max_x) max_x = x; 
      if (x < min_x) min_x = x; 
    }
    
    middle = (max_xrot + min_xrot)/2.; 
    xmiddle = (max_x + min_x)/ 2; 
//    printf("min,max,middle: %f %f %f\n",min_xrot, max_xrot, middle); 

    diff_phi = 0; 
    diff_phi0 = 0; 

    for (int e = 0; e < nelec; e++)
    {
      double x = elec_xy->GetX()[e]; 
      double y = elec_xy->GetY()[e]; 

      if (x < xmiddle) 
      {
        diff_phi0++; 
      }

      else if (x > xmiddle) 
      {
        diff_phi0--;  
      }

      if (xrot[e] < middle) 
      {
        diff_phi++; 
      }
      else if (xrot[e] > middle)
      {
        diff_phi--; 
      }
    }

    delete xrot; 
    
    int lastbin = total->GetNbinsX(); 

    for (int b = lastbin; b > 0; b--)
    {
      if (total->GetBinContent(b) > 0)
      {
        lastbin = b; 
        break; 
      }
    }

    integral = total->Integral(); 
    skewness = total->GetSkewness(); 
    diff = total->Integral(1,lastbin/2) - total->Integral((lastbin+1)/2+1,lastbin); 
    diff1 = total->Integral(1,lastbin/2) - total->Integral(lastbin/2+1,lastbin); 
    line.SetRange(total->GetBinLowEdge(1),total->GetBinLowEdge(lastbin+1)); 
    line.SetParameter(0,integral/lastbin); 
    line.SetParameter(1,0); 
    total->Fit(&line,"NRQ"); 
    slope = line.GetParameter(1); 
    slope_error = line.GetParError(1);
    chisq = line.GetChisquare(); 
    s0 = line.Eval(total->GetBinCenter(1)); 
    s1 = line.Eval(total->GetBinCenter(lastbin)); 
    
    tree->Fill(); 
    t->clearHistograms(); 
  }

  tree->Write(); 
  of.Close(); 

}

void fit_alpha_hists()
{
  fithists("data/he/60torr/coll/COLLISON_50keV.txt", "50kevalpha.root", 60, 2000, 5e-3); 
}


