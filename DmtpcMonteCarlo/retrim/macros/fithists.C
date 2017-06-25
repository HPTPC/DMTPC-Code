void fithists(const char * collfile, const char * out,int pressure = 30, double nbins=100, double binsize=2.5e-2)
{
  gSystem->Load("lib/libretrim.so"); 
  gSystem->Load("../../DmtpcMath/lib/libDmtpcMath.so"); 

  dmtpc::math::PrincipalComponentFinder pca(3); 

  dmtpc::mc::retrim::CollisionReader c(collfile); 
  dmtpc::mc::retrim::TableReader * tables[2]; 
  tables[0] = new dmtpc::mc::retrim::TableReader(TString::Format("data/srim/f_in_cf4_%dtorr.txt",pressure)); 
  tables[1] = new dmtpc::mc::retrim::TableReader(TString::Format("data/srim/c_in_cf4_%dtorr.txt",pressure)); 

  dmtpc::mc::retrim::TrackMaker *t = new dmtpc::mc::retrim::TrackMaker(&c,2,tables,0.98); 

  TH1D var_frac_mean("fracmean","fracmean2",20,0,1); 
  TH1D var_frac_mean2("fracmean2","fracmean2",20,0,1); 
  TH1D var_length_mean("lengthmean","lengthmean",20,0,binsize*nbins); 
  TH1D var_length_mean2("lengthmean2","lengthmean2",20,0,binsize*nbins); 
  TH1I var_frac_n("fracn","fracn",20,0,1); 
  TH1I var_length_n("lengthn","lengthn",20,0,binsize*nbins); 
  

  TFile of(out,"RECREATE"); 

  double skewness, slope, diff,diff1,integral, slope_error, chisq,s0,s1, phi, diff_phi, diff_phi0, phi_err; 
  double max_x, min_x, min_xrot, max_xrot, xmiddle,middle,width,width_first, width_second, width_first_phi0, width_second_phi0; 
  double opening; 
  int nelec;
  TH2D * var_frac = new TH2D("var_frac","Transverse RMS vs. track fraction",50,0,1,20,0,.5); 
  TH2D * var_length = new TH2D("var_length","Transverse RMS vs. track length",nbins,0,binsize*nbins,20,0,.5); 

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
  tree->Branch("width",&width); 
  tree->Branch("width_first",&width_first); 
  tree->Branch("width_second",&width_second); 
  tree->Branch("width_first_phi0",&width_first_phi0); 
  tree->Branch("width_second_phi0",&width_second_phi0); 
  tree->Branch("xmax_rot",&max_xrot); 
  tree->Branch("xmin_rot",&min_xrot); 
  tree->Branch("opening",&opening); 

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
    
    pca.clearPoints(); 
    elec_xy = new TGraph(nelec); 
    for (int e = 0; e < nelec; e++)
    {
      elec_xy->SetPoint(e, t->getElectronX(e)->X(), t->getElectronX(e)->Y()); 
      double pca_pt[3]; 
      pca_pt[0] = t->getElectronX(e)->X(); 
      pca_pt[1] = t->getElectronX(e)->Y(); 
      pca_pt[2] = t->getElectronX(e)->Z(); 
      pca.addPoint(pca_pt); 
    }

    pca.solve(); 
    double pca_solution[3]; 
    pca.getComponent(0,pca_solution); 

    //dot product of (1,0,0) with principal component, take sign from mean 
    int sign = pca.getMean(0) > 0 ? 1 : -1;
    opening = acos(sign * fabs(pca_solution[0]));  



    elec_xy->Fit(&elecline,"NRQ"); 

    phi = atan(elecline.GetParameter(1)); 

    double elec_line_b = elecline.GetParameter(0); 

    double *xrot = new double[nelec]; 


    min_xrot = 1e100;
    max_xrot = -1e00; 
    max_x = -1e00; 
    min_x = 1e100; 
    double width_mean = 0; 
    double width_mean_sq = 0; 
    double width_mean_1 = 0; 
    double width_mean_2 = 0; 
    double width_mean_sq_1 = 0; 
    double width_mean_sq_2 = 0; 
    int n1=0; 
    int n2=0; 
    int n1_phi0=0; 
    int n2_phi0=0; 

    double width_mean_1_phi0 = 0; 
    double width_mean_2_phi0 = 0; 
    double width_mean_sq_1_phi0 = 0; 
    double width_mean_sq_2_phi0 = 0; 


    for (int e = 0; e < nelec; e++)
    {
      double x = elec_xy->GetX()[e]; 
      double y = elec_xy->GetY()[e]; 
      xrot[e] = x* cos(phi) + y * sin(phi);  
      double w = -x * sin(phi) + y * cos(phi); 

      width_mean += w/nelec; 
      width_mean_sq += w*w/nelec; 

      if (xrot[e] > max_xrot) max_xrot = xrot[e]; 
      if (xrot[e] < min_xrot) min_xrot = xrot[e]; 
      if (x > max_x) max_x = x; 
      if (x < min_x) min_x = x; 
    }
    width = sqrt(width_mean_sq-width_mean*width_mean); 
    
    middle = (max_xrot + min_xrot)/2.; 
    double length = max_xrot - min_xrot; 
    xmiddle = (max_x + min_x)/ 2; 
//    printf("min,max,middle: %f %f %f\n",min_xrot, max_xrot, middle); 

    diff_phi = 0; 
    diff_phi0 = 0; 

    var_frac_mean.Reset(); 
    var_frac_mean2.Reset(); 
    var_frac_n.Reset(); 
    var_length_mean.Reset(); 
    var_length_mean2.Reset(); 
    var_length_n.Reset(); 

    for (int e = 0; e < nelec; e++)
    {
      double x = elec_xy->GetX()[e]; 
      double y = elec_xy->GetY()[e]; 
      double w = -x * sin(phi) + y * cos(phi); 

      double wm = w - width_mean; 

      var_frac_n.Fill((xrot[e] - min_xrot)/length);  
      var_frac_mean.Fill((xrot[e] - min_xrot)/length, wm);  
      var_frac_mean2.Fill((xrot[e] - min_xrot)/length, wm*wm);  
      var_length_n.Fill((xrot[e] - min_xrot));  
      var_length_mean2.Fill((xrot[e] - min_xrot), wm*wm);  
      var_length_mean.Fill((xrot[e] - min_xrot), wm);  

      if (x < xmiddle) 
      {
        diff_phi0++; 
        n1_phi0++;
        width_mean_1_phi0 += y; 
        width_mean_sq_1_phi0 += y*y; 

      }

      else if (x > xmiddle) 
      {
        diff_phi0--;  
        n2_phi0++; 
        width_mean_2_phi0 += y; 
        width_mean_sq_2_phi0 += y*y; 
      }

      if (xrot[e] < middle) 
      {
        diff_phi++; 
        n1++;
        width_mean_1 += w; 
        width_mean_sq_1 += w*w; 
      }
      else if (xrot[e] > middle)
      {
        diff_phi--; 
        n2++;
        width_mean_2 += w; 
        width_mean_sq_2 += w*w; 
      }
    }

    for (int b = 1; b <= var_frac_mean2.GetNbinsX(); b++)
    {

      if (var_frac_n.GetBinContent(b) > 0)
      {
        double ww = var_frac_mean2.GetBinContent(b)/var_frac_n.GetBinContent(b); 
        var_frac->Fill(var_frac_mean2.GetXaxis()->GetBinCenter(b), sqrt(ww)); 
      }
    }

    for (int b = 1; b <= var_length_mean2.GetNbinsX(); b++)
    {

      if (var_length_n.GetBinContent(b) > 0)
      {
        double ww = var_length_mean2.GetBinContent(b)/ var_length_n.GetBinContent(b); 
        var_length->Fill(var_length_mean2.GetXaxis()->GetBinCenter(b), sqrt(ww)); 
      }
    }

    width_mean_1_phi0/= n1_phi0;
    width_mean_2_phi0/= n2_phi0;
    width_mean_1/= n1;
    width_mean_2/= n2;
    width_mean_sq_1_phi0/= n1_phi0;
    width_mean_sq_2_phi0/= n2_phi0;
    width_mean_sq_1/= n1;
    width_mean_sq_2/= n2;

    width_first = sqrt(width_mean_sq_1 - width_mean_1 * width_mean_1); 
    width_second = sqrt(width_mean_sq_2 - width_mean_2 * width_mean_2); 
    width_first_phi0 = sqrt(width_mean_sq_1_phi0 - width_mean_1_phi0 * width_mean_1_phi0); 
    width_second_phi0 = sqrt(width_mean_sq_2_phi0 - width_mean_2_phi0 * width_mean_2_phi0); 


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
  var_frac->Write(); 
  var_length->Write(); 
  tree->Write(); 
  of.Close(); 

}



