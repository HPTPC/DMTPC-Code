{

  gStyle->SetOptStat(0);

  //subtract the voltage off data from the source data

  TFile von("/scratch2/gmuraru/dmtpc_software/Analyses/field_uniformity/output/test_gainmap_fe/test.root");

  TFile voff("/scratch2/gmuraru/dmtpc_software/Analyses/field_uniformity/output/gainmap_voff_fe/voff.root");


  // - multiple cameras
  
  /* int ncam = 4;
  int cmin = 3;
  int cmax = 4;

  TH2F * imon[ncam];
  TH2F * imoff[ncam];
  TH2F * root_result[ncam];
  TH2F * maxcam_result[ncam];
  TH2F * nosatresult[ncam];

  TCanvas * c1 = new TCanvas("c1","Root Subtraction",1024,768);
  TCanvas * c2 = new TCanvas("c2","MaxCam Subtraction",1024,768);
  if(ncam > 1){
      c1->Divide(2,2);
      c2->Divide(2,2);
  }


  for(int i = cmin; i < cmax; i++){
    imon[i]= (TH2F*)von.Get(Form("summed_image_%d",i));
    imoff[i] = (TH2F*)voff.Get(Form("summed_image_%d",i));

    //using root subtraction
    root_result[i] = (TH2F*)imon[i]->Clone(Form("root_result_%d",i));
    root_result[i]->Add(imoff[i],-1);

    //MaxCam Subtract Pedestal
    double pedestal = MaxCamImageTools::getMean(imoff[i]);
    maxcam_result[i] = (TH2F*)imon[i]->Clone(Form("maxcam_result_%d",i));
    MaxCamImageTools::subtractPedestal(maxcam_result[i],pedestal);
    maxcam_result[i]->SetTitle("Pedestal-Subtracted Summed Data");
    maxcam_result[i]->SetXTitle("Pixels");
    maxcam_result[i]->SetYTitle("Pixels");
    //
    
    //draw
    c1->cd(i+1);
    root_result[i]->Draw("colz");
    c2->cd(i+1);
    maxcam_result[i]->Draw("colz");
    
    }*/

  

  // - single camera
  
  TH2F * imon = (TH2F*)von.Get("summed_image_1");
  TH2F * imon_avg = (TH2F*)von.Get("manual_summed_avgd_1");
  TH2F * imoff = (TH2F*)voff.Get("summed_image_1");
  TH2F * imoff_avg = (TH2F*)voff.Get("manual_summed_avgd_1");
  TH2F * root_result = imon->Clone("root_result_1");
  TH2F * root_result_avg = imon_avg->Clone("gain_variations_1");

  TH1F* intensity=new TH1F("intensity","intensity",1000,-500,500);
  for (int m=0; m<imon_avg->GetNbinsX(); m++){
    for (int n=0; n<imon_avg->GetNbinsY(); n++) {
      float pixval=imon_avg->GetBinContent(m,n);

      intensity->Fill(pixval);
    }
  }

  // intensity->SetStats(kTRUE);
  // gStyle->SetOptStat(1111);
  intensity->SetXTitle("ADU");
  intensity->SetYTitle("Number of Entries");

  TH1F* intensity_voff=new TH1F("intensity voff","intensity voff",1000,-500,500);
  for (int m=0; m<imoff_avg->GetNbinsX(); m++){
    for (int n=0; n<imoff_avg->GetNbinsY(); n++) {
      float pixval=imoff_avg->GetBinContent(m,n);

      intensity_voff->Fill(pixval);
    }
  }

  //  intensity_voff->SetStats(kTRUE);
  //gStyle->SetOptStat(1111);
  intensity_voff->SetXTitle("ADU");
  intensity_voff->SetYTitle("Number of Entries");

  TCanvas *c = new TCanvas("c","",0,0,900,300);
  c->Divide(2,1);
  c->cd(1);
  intensity->Draw();
  c->cd(2);
  intensity_voff->Draw();
  c->Update();
  

  TH2F * noclust_on = (TH2F*)von.Get("noclust_add_counter_1");
  noclust_on->SetTitle("Count of Bins Added to Sum");
  noclust_on->SetXTitle("Pixels");
  noclust_on->SetYTitle("Pixels");
  //noclust_on->SetMinimum(10570);
  
  //  TH2F * maxcam_result;
  //  TH2F * nosatresult;
  root_result->Add(imoff,-1);
  root_result->SetTitle("Pedestal-Subtracted Summed Data");
  root_result->SetXTitle("Pixels");
  root_result->SetYTitle("Pixels");

  TCanvas *c5 = new TCanvas("c5","",0,0,900,300);
  c5->Divide(2,1);
  c5->cd(1);
  root_result->ProjectionX("ProjX", 0, -1)->Draw();
  c5->cd(2);
  root_result->ProjectionY("ProjY", 0, -1)->Draw();
  c5->Update();

  root_result_avg->Add(imoff_avg,-1);
  // root_result_avg->Rebin2D(4);
  //root_result_avg->Rebin2D();
  root_result_avg->SetTitle("Pedestal-Subtracted Summed Data");
  root_result_avg->SetXTitle("Pixels");
  root_result_avg->SetYTitle("Pixels");

  root_result_avg->SetMaximum(300);
  root_result_avg->SetMinimum(-100);

  TH2F * inverse_map = (TH2F*)root_result_avg->Clone("invers_map_1");
  //inverse_map->Rebin2D(2);
  inverse_map->SetTitle("Gain Map BL Camera");
  inverse_map->SetXTitle("Pixels");
  inverse_map->SetYTitle("Pixels");
  inverse_map->SetMaximum(5);
  inverse_map->SetMinimum(-5);

  for(int xbin = 1; xbin <= inverse_map->GetNbinsX(); xbin++){
    for(int ybin = 1; ybin <= inverse_map->GetNbinsY(); ybin++){
      inverse_map->SetBinContent(xbin,ybin,1/inverse_map->GetBinContent(xbin,ybin));
    }
  }

  
  TCanvas c1("c1","Root Subtraction",600,400);
  c1.cd();
  root_result->Draw("colz");
  
  TCanvas c2("c2","Root Average Subtraction",600,400);
  c2.cd();
  root_result_avg->Draw("colz");

  TCanvas c3("c3","Gain Map (Inverse)",600,400);
  c3.cd();
  inverse_map->Draw("colz");

  TCanvas c4("c4","Bin Counter On",600,400);
  c4.cd();
  noclust_on->Draw("colz");
}
  

