{

  //  noise_in_ADU = sqrt[ (read_noise_in_e*ADU_per_e_for_this_camera)**2 + (dark_rate_in_e_per_pixel_per_s * exposure_length_in_s * pixels * ADU_per_e_for_this_camera)**2 ] 
  
  const int ncam = 3;
  /*
  std::map<TString, int> cameras;
  cameras["PL0141514"] = 0;
  cameras["PL0251514"] = 1;
  cameras["PL0261514"] = 2;
  */
  double pixels = 3056*3056;
     
  double read_noise[ncam];
  double adu_pe[ncam];
  double dark[ncam];
  
  read_noise[0] = 11.2;
  read_noise[1] = 10.2;
  read_noise[2] = 11.3; //9.6;
  
  adu_pe[0] = 1.55;
  adu_pe[1] = 1.52;
  adu_pe[2] = 1.53; //1.52;


  //dark should be:  dark[0] = 0.6/(100*adu_pe[1]*pixels); but going to multiply by pixels and gain in calculation, so just omitting here
  dark[0] = 2.5/(100);
  dark[1] = 0.6/(100);
  dark[2] = 40.8/(100); //0.7/(100);

  double exp[5] = {1.0,2.0,5.0,10.0,30.0};

  double noise[5];
  TGraph * graphs[ncam];
  TCanvas * c1[ncam];



  for(int c = 0; c < ncam; c++){
    for(int i = 0; i < 5; i++){
      noise[i] = TMath::Sqrt(read_noise[c]*adu_pe[c]*read_noise[c]*adu_pe[c] + dark[c]*exp[i]*dark[c]*exp[i]);
    }

    graphs[c] = new TGraph(5,exp,noise);
    c1[c] = new TCanvas();
    c1[c]->cd();
    graphs[c]->SetTitle(Form("Predicted Noise vs. Exposure Cam %d",c));
    graphs[c]->GetXaxis()->SetTitle("Exposure Time (s)");
    graphs[c]->GetYaxis()->SetTitle("Predicted Noise (ADU)");
    graphs[c]->Draw("alp");
  }

}
  




  
