void calc_rms_runs(int run1 = 1167004, int runs = 1, Bool_t pixflag=0, const char * dir = "/scratch3/wparker2/dmtpc2/data/2017/06/raw")
{// runs = number of runs you want to consider 
//run1 is the runID of the first one 
//pixflag turns branch pixflag on and off. If it's set to 1, all intensity values of all pixels are saved
  //in the branch pixval. The size of output file is inflated by more than 1000 times
//dir is the path to the directory with the raw data files in it

  gSystem->Load("$DMTPC_HOME/DmtpcCore/lib/libDmtpcCore.so");
  gSystem->Load("$DMTPC_HOME/DmtpcAnalysis/lib/libDmtpcAnalysis.so");

  double stops[5] = {0,0.34,0.61,0.84,1.0};
  gStyle->SetOptStat(0);
  double red[5] = {0.0,0.0,0.87,1.0,0.51};
  double green[5] = {0.0,0.81,1.00,0.2,0.0};
  double blue[5] = {0.51,1.0,0.12,0.0,0.0};
  TColor::CreateGradientColorTable(5,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

  //From here starts the analysis
  int cam = 0;
  int ncam = 1;

  for (int fi=0;fi<runs;fi++){
    //loop over the number of runs you want to consider

    TFile f(TString::Format("%s/hptpc_test_R%05d.raw.root",dir,run1+fi));
    if (f.IsZombie()) { //check that the file opens
    cout<<"file with run number "<< run1+fi<<" is a zombie"<<endl;
    continue; //go onto next run if it doesn't
    }
    else {

     //open the data from the file into a dataset
     d = new dmtpc::core::Dataset;
     d->open(TString::Format("%s/hptpc_test_R%05d.raw.root",dir,run1+fi));

     //output file
     TString output_name = Form("calc_rms_R%i.root",run1+fi);
     TFile * calc_rms1 = new TFile(output_name,"RECREATE");
     TTree t("pix_t","Pixel Tree");
     int nbins = 200;

     if (!d) {//check the dataset exists
       std::cout<<"no d"<<std::endl;
       continue;
     }
 
     TH2 * bias = d->biasAvg(cam);
 
     int n = d->nevents(); //get the number of events in the run to loop over 
     TH2 * image = new TH2D("image","image", d->event()->ccdData(cam)->GetNbinsX(),
       d->event()->ccdData(cam)->GetXaxis()->GetXmin(),
       d->event()->ccdData(cam)->GetXaxis()->GetXmax(),
       d->event()->ccdData(cam)->GetNbinsY(),
       d->event()->ccdData(cam)->GetYaxis()->GetXmin(),
       d->event()->ccdData(cam)->GetYaxis()->GetXmax());

     TH1D* intensity=new TH1D("intensity","intensity",nbins,-100,100);
     //intensity histogram to hold all the data from every pixel of every event
     double rms = 0;
     double mean = 0;
     double chi = 0;
     int runnum = 0;
     double ccdTemp;
     int nevents = 0;
 
    //This block creates a variable to store pixval data.
    const int stupidcppx = (int)(d->event()->ccdData(cam)->GetNbinsX());
    const int stupidcppy = (int)(d->event()->ccdData(cam)->GetNbinsY());
    const int x = stupidcppx;
    const int y = stupidcppy;
    Double_t pixval[x][y];

    //Store rms values of all pixels
    TH2F *pix =new TH2F("pix","rms of individual pixels",x,0,x,y,0,y);
    pix->SetStats(0);

    //This block creates 3 arrays for computing rms of each individual pixels
    Double_t sum[x][y]
            ,sumsquare[x][y]
            ,rms[x][y];

    //create branches for the variables you want to save to the tree
    t.Branch("mean",&mean);
    t.Branch("rms",&rms);
    t.Branch("nevents",&nevents);
    t.Branch("chi",&chi);
    t.Branch("runnum",&runnum);
    t.Branch("ccdTemp",&ccdTemp);
  //create branch pixval only if pixflag is set to 1
    if (pixflag==1)
    {
      const char *leaflist = (char*)TString::Format("pixval[%i][%i]/D",x,y);
      t.Branch("pixval",&pixval[0][0],leaflist);
    }

    std::cout<< n <<" events."<<std::endl;

    for (int i = 0; i < n; i++) { //loop over each event
      std::cout<<endl;
      std::cout<<"Event "<<i+1<<std::endl;
      TH1D *eventintensity = new TH1D("eventintensity","eventintensity",nbins,-100,100);
      //this histogram only holds the pixel values from each event and is made new each iteration
      eventintensity->Sumw2();
      d->getEvent(i);
      nevents = d->nevents();
      runnum = run1+fi;
      image = d->event()->getImage(cam)->getVisible();
      image->Add(d->biasAvg(cam),-1);

      ccdTemp = d->event()->ccdConfig(cam)->ccdTemp;

      for (int m=0; m<x; m++){
        for (int b=0; b<y; b++) {
          Double_t temppixval=image->GetBinContent(m,b);

          //fill temppixval to pixval[][] if pixflag is true(1)
          if (pixflag==1)
          {
            pixval[m][b]=temppixval;
          }

          //fill sum[][] and sumsquare[][]
          sum[m][b]=sum[m][b]+temppixval;
          sumsquare[m][b]=sumsquare[m][b]+temppixval*temppixval;

         //fill both intenstiy hisotgrams with the pixel data
         intensity->Fill(temppixval);
         eventintensity->Fill(temppixval);
       }
     }

     //gaussian is fitted to the data from event i 
     TF1 * gFit = new TF1("gFit","gaus");
     eventintensity->Fit(gFit,"WL");
     rms = eventintensity->GetRMS();
     mean = gFit->GetParameter(1);
     chi = gFit->GetChisquare();

      t.Fill(); //data added to the tree
      delete eventintensity;
      delete gFit;
    }

    //get rms of each pixel and store in (TH2F)pix
     for(int j=0;j<x;j++)
     {
       for(int k=0;k<y;k++)
       {
         rms[j][k]=(Double_t)TMath::Sqrt(sumsquare[j][k]/n-(sum[j][k]/n)*(sum[j][k]/n));
         pix->Fill(j,k,rms[j][k]);
       }
     }

    //fit gaussian to all data
    TF1 * gFit2 = new TF1("gFit2","gaus");
    intensity->Fit(gFit2);

    double rmsfinal = intensity->GetRMS();
    std::cout<<"The rms of the intensity is "<<rmsfinal<<"."<<std::endl;

    intensity->SetStats(kTRUE);
    gStyle->SetOptStat(1111);
    intensity->SetXTitle("ADU");
    intensity->SetYTitle("Number of Entries");

    //write tree to output file wherever you run the code from
    t.Write();
    image->Write();
    intensity->Write();
    bias->Write();
    pix->Write();

    }

  }
}
