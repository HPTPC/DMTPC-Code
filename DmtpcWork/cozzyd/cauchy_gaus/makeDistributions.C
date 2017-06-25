#include "TRandom3.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"

void fillDistribution(TH1 * h, double noise, double gaus_sigma, double cauchy_width, int signal)
{

  static TRandom3 rand(0);

  for (int i = 0; i < signal; i++)
  {

    double x = rand.Gaus(0,gaus_sigma); 
    double a,b; 
    rand.Rannor(a,b); 
    x +=  a/b * cauchy_width; 
    h->Fill(x); 
  }

  for (int i = 1; i <=h->GetNbinsX(); i++)
  {
    h->SetBinContent(h->GetBinContent(i),noise); 
  }
}


void makeDistributions(const char * outfile = "distributions.root", double binning = 0.64, int nbins = 20)
{

  TFile out(outfile,"RECREATE"); 
  double noise; 
  int signal; 
  double sigma; 
  double cauchy;

  TH1* hist = new TH1D("dist","Distribution", nbins, -nbins/2 * binning, nbins/2 * binning); 

  TTree * dists=  new TTree("dists","tree"); 
  dists->Branch("noise",&noise);
  dists->Branch("signal",&signal);
  dists->Branch("sigma",&sigma);
  dists->Branch("cauchy",&cauchy);
  dists->Branch("hist",&hist);


  for (noise = 0; noise < 20; noise++)
  {

    for (signal= 10; signal < 1000; signal+=10)
    {
      for (sigma=0; sigma < 2; sigma+=0.05)
      {
        for (cauchy=0; cauchy < 2; cauchy +=0.05)
        {

          for (int i = 0; i < 20; i++)
          {
            hist->Reset(); 
            fillDistribution(hist,noise,sigma,cauchy,signal); 
            dists->Fill();
          }
        }
      }
    }

  }


}
