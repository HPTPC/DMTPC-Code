#include "SkimDataset.hh"
#include "SkimEvent.hh"
#include "ImageCalculations.hh"
#include "ImageProcessing.hh"
//#include "MaxCamImageTools.hh"


#include <TFile.h>
#include <TH1.h>
#include <TH2F.h>
#include <TString.h>
#include <TF1.h>
#include <TGraph.h>

#include <fstream>
#include <vector>
#include <iostream>

using namespace std;
using namespace dmtpc::core;
using namespace dmtpc::image::calculations;
using namespace dmtpc::image::processing;
using namespace dmtpc::analysis;

TH1D * getPixHist(TH2F * hist){

  
  int xmin = hist->GetXaxis()->GetFirst();
  int ymin = hist->GetYaxis()->GetFirst();
  int xmax = hist->GetXaxis()->GetLast();
  int ymax = hist->GetYaxis()->GetLast();

  vector<double> vals;
  double min = 1e99;
  double max = -1e99;

  //store all bin contents and deduce max and min values for histogram limits
  for (int x = xmin; x <= xmax; x++)
    {
      for (int y = ymin; y <= ymax; y++)
	{
	  double val = hist->GetBinContent(x,y);

	  if (val < min) min = val;
	  if (val > max) max = val;
	  vals.push_back(val);
	}
    }

  int nbins = int(max - min + 0.5);

  TH1D * ph = new TH1D("ph","ph",nbins,min,max);

  for (unsigned i = 0; i < vals.size(); i++)
    {
      ph->Fill(vals[i]);
    }
  return ph;
}

int main(int argc, char** argv){

  //  TFile biasfile("output/darco_bias_10.root");

  //  double biasvalue = MaxCamImageTools::getMean(biasframe);

  bool bias = false;
  

  SkimDataset d;
  d.openRootFile(argv[1]);

  TH1D * pixhist = 0;
  TFile savefile(argv[2],"recreate");
  const int ncam = 1;
  int camid = 3;

  TTree t("pix_t","Pixel Tree");

    //  TString dir = "/home/dmatter/ric_temp/skim/";
  //  TString name = "dmtpc_m3_00";
  double rms = 0;

  double mean = 0;
  double chi = 0;
  int runnum = 0;
  int nevents = 0;
  
  t.Branch("pixhist",&pixhist);
  t.Branch("mean",&mean);
  t.Branch("rms",&rms);
  t.Branch("camid",&camid);
  t.Branch("runnum",&runnum);
  t.Branch("nevents",&nevents);
  t.Branch("chi",&chi);
  

  for(int i = 0; i < d.nevents(); i++){
     //TH2F * biasframe = (TH2F*)d[i].getBiasFrame(c)->Clone("tmpbias");
    d.getEvent(i);
    if(i==0){
      runnum = d.event()->runNumber();
      nevents = d.nevents();
    }
    TH2F * img = (TH2F*)d.event()->image(camid)->Clone("img");
    pixhist = getPixHist(img);
    rms = pixhist->GetRMS();
    mean = pixhist->GetMean();
    TF1 * gFit = new TF1("gFit","gaus");  
    pixhist->Fit(gFit);
    //rms = gFit->GetParameter(2);
    //mean = gFit->GetParameter(1);
    chi = gFit->GetChisquare();
    t.Fill();
    //d.closeRootFile();
    delete img;
  }

  savefile.Write();
  savefile.Close();
  
}
