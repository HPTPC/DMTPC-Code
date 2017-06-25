#include "SkimEvent.hh" 
#include "TChain.h" 
#include "ClusterAlgo.hh"
#include "TMath.h"
#include <cmath>
#include <cstdio>
#include "TH2.h" 
#include "TFile.h"
#include <vector>


int main (int nargs, char ** args) 
{

  dmtpc::analysis::SkimEvent * ev = new dmtpc::analysis::SkimEvent; 

  double minE = getenv("SIMPLE_ALPHA_MIN_E") ? atof(getenv("SIMPLE_ALPHA_MIN_E")) : 1e5; 
  double maxE = getenv("SIMPLE_ALPHA_MAX_E") ? atof(getenv("SIMPLE_ALPHA_MAX_E")) : 1.2e6; 
  double minRange = getenv("SIMPLE_ALPHA_MIN_RANGE") ? atof(getenv("SIMPLE_ALPHA_MIN_RANGEE")) : 2000; 

  int pointing_left = getenv("SIMPLE_ALPHA_POINTING_LEFT") ? atoi(getenv("SIMPLE_ALPHA_POINTING_LEFT")) : 1; 


  TChain ch("skim"); 

  if (nargs < 3) 
  {
    printf ("SimpleAlphaGain gain.root skim0.root skim1.root ...\n"); 
    return 1; 
  }



  for (int i = 2; i < nargs; i++)
  {
    ch.Add(args[i]); 
  }



  printf("number of entries: %d \n", (int) ch.GetEntries()); 


  ch.SetBranchAddress("event",&ev); 

  ch.GetEntry(0); 
  int ncam = ev->ncamera(); 


  TFile out(args[1],"CREATE"); 
  std::vector<TH1*> sums(ncam); 
  std::vector<TH1*> norm(ncam); 

  TTree * gain = new TTree("gain","gain"); 
  int cam; 
  double max; 
  double max_location;
  double range; 
  double E; 
  double maxpixel; 
  double phi; 
  double width; 
  int run; 
  int event; 

  gain->Branch("max",&max); 
  gain->Branch("max_location",&max_location); 
  gain->Branch("range",&range); 
  gain->Branch("E",&E); 
  gain->Branch("maxpixel",&maxpixel); 
  gain->Branch("phi",&phi); 
  gain->Branch("width",&width); 
  gain->Branch("run",&run); 
  gain->Branch("cam",&cam); 
  gain->Branch("event",&event); 



  for (int i = 0; i < ncam; i++)
  {

    double binning = ev->image(i)->GetXaxis()->GetBinWidth(1); 
    double width = sqrt(2) * ev->image(i)->GetXaxis()->GetXmax() - ev->image(i)->GetXaxis()->GetXmin(); 
    sums[i] = new TH1D (TString::Format("avg%d",i),TString::Format("average for %s", ev->cameraSerialNumber(i).Data()), 
                        int(width/binning),0,width); 
    norm[i] = new TH1D (TString::Format("norm%d",i),TString::Format("norms for %s", ev->cameraSerialNumber(i).Data()), 
                        int(width/binning),0,width); 
                      
  }



  TString cutstr = TString::Format("_E< %f && _E> %f && _range> %f  &&  _lastspark > 10", maxE, minE, minRange); 
  printf("%s\n",cutstr.Data()); 
  int passcut = ch.Draw("Entry$", cutstr,"goff"); 


  printf("%d pass cut\n", passcut) ; 


  for (int i = 0; i < passcut; i++)
  {
    int entry = (int( ch.GetV1()[i])); 
    printf("%d\n",entry); 

    ch.GetEntry(entry); 
    if (i % 100 == 0) printf("processing entry %d\n",i); 

    for (int c = 0; c < ncam; c++)
    {
      if (ev->spark(c)) continue; 
    
      for (int t = 0; t < ev->ntracks(c); t++)
      {
        if (ev->E(c,t) > maxE) continue; 
        if (ev->E(c,t) < minE) continue; 
        if (ev->range(c,t) < minRange) continue; 
        range = ev->range(c,t); 
        cam = c; 
        run = ev->runNumber(); 
        event = ev->eventNumber(); 
        maxpixel = ev->maxpixel(c,t); 
        E = ev->E(c,t); 
        phi = ev->phi(c,t); 
        width = sqrt(ev->transverse_moment(c,2,t)); 

        if (cos(phi) < 0 != pointing_left) phi += TMath::Pi(); 

        TH1 * projection = dmtpc::analysis::clusteralgo::project(ev->cluster(c)->getCluster(t), phi); 
        max = projection->GetMaximum(); 
        max_location = projection->GetBinCenter(projection->GetMaximumBin()); 

        for (int x = 1; x <= projection->GetNbinsX(); x++)
        {
          norm[c]->AddBinContent(x); 
          sums[c]->AddBinContent(x,projection->GetBinContent(x)); 
        }

        delete projection;
        gain->Fill(); 
      }
    }
  }


  out.cd();

  for (int i = 0; i < ncam; i++)
  {
    norm[i]->Write(); 
    sums[i]->Divide(norm[i]); 
    sums[i]->Write(); 
  }

  gain->Write(); 

  out.Close(); 
  return 0; 







}


