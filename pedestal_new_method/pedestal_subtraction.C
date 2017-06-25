TH2F *hPedAvr;
TH2S *hPedStat;

TH2D* calibrate_image(const dmtpc::core::Event * eve){
  
  assert(eve);
  TH2D* h2Dlight = eve->getImage(0)->makeEmptyTH2D();
  
  //TH2D *h2Dlight = sample->makeEmptyTH2D();

  int frameId=eve->ev();
  int runId=eve->run();
  
  h2Dlight->SetNameTitle(coreName+Form("_fr%d_light",frameId),coreName+Form(" fr%d R%d, Light (ADU), nBinX=%d",frameId,runId,h2Dlight->GetNbinsX()));
  h2Dlight->Reset("M");
  
  int ncam_in =eve->nccd();
  assert(camDaq<ncam_in);
  TH2I * h2Raw=(TH2I*)eve->ccdData(camDaq);
  assert(h2Raw);
  
  // basic sanity check                                                                                                                                                                            
  assert(h2Raw->GetNbinsX() == hPedStat->GetNbinsX());
  assert(h2Raw->GetNbinsY() == hPedStat->GetNbinsY());
  assert(h2Raw->GetXaxis()->GetXmax()==hPedStat->GetXaxis()->GetXmax());
  
  //.... subtract pedestals  & propagate error of pedestals                                                                                                                                        
  // ...  and clear masked pixels                                                                                                                                                                  
  int nchX=hPedStat->GetNbinsX();
  int nchY=hPedStat->GetNbinsY();
  for(int jchX=1; jchX<=nchX; jchX++) {  // range 1,N                                                                                                                                              
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N                                                                                                                                             
      if (  hPedStat->GetBinContent(jchX,jchY) ) continue;
      float val=h2Raw->GetBinContent(jchX,jchY);
      float ped=hPedAvr->GetBinContent(jchX,jchY);
      float ltErr=hPedAvr->GetBinError(jchX,jchY);
      h2Dlight->SetBinContent(jchX,jchY,val-ped);
      h2Dlight->SetBinError(jchX,jchY,ltErr); // set err to pedRMS                                                                                                                                 
      
    }
  }

  return h2Dlight;
  
  }


void pedestal_subtraction(){

  //open pedestal file  
  //retrieve histos hpedstat and hpedavr
  //open data file
  //loop over events
  //inside the loop call the method that Michael described in the mail, calibrate image method
  gSystem->Load("/scratch2/gmuraru/dmtpc_software/DmtpcCore/lib/libDmtpcCore.so");
    
  TFile * ped_file = TFile::Open("/scratch2/gmuraru/dmtpc_software/pedestal_new_method/m3_gain_R1029034.m3ped.root");
  hPedAvr = (TH2F*)ped_file->Get("hPedAvr");
  hPedStat = (TH2S*)ped_file->Get("hPedStat");

  //TFile * data_file = TFile::Open("/scratch1/darkmatter/dmtpc/data/m3/2016/m3_gain_R1029034.raw.root");
  int camDAQ = 1;

  dmtpc::core::Dataset *d = new dmtpc::core::Dataset;
  d->open("/scratch1/darkmatter/dmtpc/data/m3/2016/m3_gain_R1029034.raw.root");
 

  int n = d->nevents();
  n = 5;
  cout << "n=" << n << endl;
  for (int i = 0; i < n; i++)
    {
      d->getEvent(i);
      TH2D * subtracted_img = calibrate_image(d->event());
      subtracted_img->Draw("colz");
      getchar():
    }
  
}

/*TH2D* calibrate_image(const dmtpc::core::Event * eve){

  assert(eve);
  TH2D* h2Dlight = eve->getImage(0)->makeEmptyTH2D();

  //TH2D *h2Dlight = sample->makeEmptyTH2D();                                                                                                                                                          

  int frameId=eve->ev();
  int runId=eve->run();

  h2Dlight->SetNameTitle(coreName+Form("_fr%d_light",frameId),coreName+Form(" fr%d R%d, Light (ADU), nBinX=%d",frameId,runId,h2Dlight->GetNbinsX()));
  h2Dlight->Reset("M");

  int ncam_in =eve->nccd();
  assert(camDaq<ncam_in);
  TH2I * h2Raw=(TH2I*)eve->ccdData(camDaq);
  assert(h2Raw);

  // basic sanity check                                                                                                                                                                                
  assert(h2Raw->GetNbinsX() == hPedStat->GetNbinsX());
  assert(h2Raw->GetNbinsY() == hPedStat->GetNbinsY());
  assert(h2Raw->GetXaxis()->GetXmax()==hPedStat->GetXaxis()->GetXmax());

  //.... subtract pedestals  & propagate error of pedestals                                                                                                                                            
  // ...  and clear masked pixels                                                                                                                                                                      
  int nchX=hPedStat->GetNbinsX();
  int nchY=hPedStat->GetNbinsY();
  for(int jchX=1; jchX<=nchX; jchX++) {  // range 1,N                                                                                                                                                  
    for(int jchY=1; jchY<=nchY; jchY++) { // range 1,N                                                                                                                                                 
      if (  hPedStat->GetBinContent(jchX,jchY) ) continue;
      float val=h2Raw->GetBinContent(jchX,jchY);
      float ped=hPedAvr->GetBinContent(jchX,jchY);
      float ltErr=hPedAvr->GetBinError(jchX,jchY);
      h2Dlight->SetBinContent(jchX,jchY,val-ped);
      h2Dlight->SetBinError(jchX,jchY,ltErr); // set err to pedRMS                                                                                                                                     

    }
  }

  return h2Dlight;

  }*/
