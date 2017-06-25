int cam=2; // BL quadrant                                                                                                               
unsigned int ev_counter;
unsigned int bias_counter;

void sum(TH2D* sum, int run=5002, unsigned int& counter)
{
  dmtpc::core::Dataset *d = new dmtpc::core::Dataset;
  TString filename = TString::Format("/scratch3/darkmatter/dmtpc/data/m3/2016/03/raw/m3_alpha_R%d.raw.root",run);
  //if (run>=1032000) file="../../02/raw/"+filename;
  d->open(filename);

  for (int i=0; i<d->nevents();i++){
    d->getEvent(i);
    TH2I* myImage = d->event()->getImage(cam)->getVisible();
    sum->Add(myImage);
    counter++;
  }
}


void alpha_adu_BR()
{
  gSystem->Load("$DMTPC_HOME/DmtpcCore/lib/libDmtpcCore.so");
  gSystem->Load("$DMTPC_HOME/DmtpcWaveform/lib/libDmtpcWaveform.so");

  ev_counter=0;
  bias_counter=0;

  // open a data file just to get histo clones                                                                                          
  dmtpc::core::Dataset *d = new dmtpc::core::Dataset;
  d->open("/scratch3/darkmatter/dmtpc/data/m3/2016/03/raw/m3_alpha_R1088046.raw.root");
  d->getEvent(0);
  TH2I* myImage = d->event()->getImage(cam)->getVisible();
  TH2D* sumImage = (TH2D*)myImage->Clone("sumImage");
  sumImage->Reset();
  TH2D* biasImage = (TH2D*)myImage->Clone("biasImage");
  biasImage->Reset();

  //for (int j=0; j<d->event()->nScopeData(); j++) {
  
    // exposure                                                                                              
    //TH1D* myTrace = (TH1D*)d->event()->getTrace(j)->getPhysical();
    //if (j<48 || j>50) continue;		\
    
    //define cut region                                                                                             
    TCutG *alpharegion = new TCutG("alpharegion",5);
    alpharegion->SetVarX("");
    alpharegion->SetVarY("");
    alpharegion->SetPoint(0, 350, 450);
    alpharegion->SetPoint(1, 350, 4300);
    alpharegion->SetPoint(2, 1800, 900);
    alpharegion->SetPoint(3, 1900, 1400);
    alpharegion->SetPoint(4, 350, 450);

    alpharegion->SetLineColor(kRed);
    alpharegion->SetLineWidth(2);

    /*dmtpc::waveform::CspWaveform cspwf;
    dmtpc::waveform::analysis::analyzeCSP( myTrace , cspwf , 100);
    dmtpc::waveform::CspPulse pulse = cspwf.at(0);
    
    dmtpc::waveform::FastWaveform fastwf;
    dmtpc::waveform::analysis::analyzeFast( myTrace ,fastwf);
    dmtpc::waveform::analysis::isTrough( myTrace ,0, 100);
    dmtpc::waveform::FastPulse fastpulse = fastwf.at(0);
    
    if (fastpulse.getFastPeak() > 400. && fastpulse.getFastPeak() < 800.) {*/
      for (int r=1088046; r<=1088047; r++) sum(sumImage,r,ev_counter);
      for (int r=1089001; r<=1089006; r++) sum(sumImage,r,ev_counter);
      //for (int r=1089028; r<=1089030; r++) sum(sumImage,r,ev_counter);
      // for (int r=1089031; r<=1089069; r++) sum(sumImage,r,ev_counter);
      //for (int r=1090001; r<=1090003; r++) sum(sumImage,r,ev_counter);
      //for (int r=1090008; r<=1090011; r++) sum(sumImage,r,ev_counter);
      
      // voltage-off, shutter closed                                                                                                        
      //  for (int b=1065001; b<=1065002; b++) sum(biasImage,b,bias_counter);
      // average                                                                                                                            
      sumImage->Scale(1./(double)ev_counter);
            
      Double_t cut_ADU = alpharegion->IntegralHist(sumImage);
      cout << "ADU count = " << cut_ADU << endl;
      // printf("Area defined by alpharegion = %d\n",area);
      //biasImage->Draw(Form("[%s]", alpharegion->GetName()));                                                        
      //alpharegion->Draw("alp, same");
      //sumImage->Draw("colz [alpharegion]");

      // int bin = sumImage->FindBin(1500,1500);
      //cout << "sumImage bin = " << sumImage->GetBinContent(bin) << endl;
      //cout << "total summed events = " << ev_counter << endl;
      //cout << "ADU = " << sumImage->Integral()/91840 << endl;
      // cout << "biasImage bin = " << biasImage->GetBinContent(bin) << endl;
      //cout << "total summed bias = " << bias_counter << endl;
      //}



      TFile *file=new TFile("alpha_adu2.root","recreate");
      sumImage->Write();
      //biasImage->Write();
      file->Close();
      
}
