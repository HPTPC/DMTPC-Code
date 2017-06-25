#include <Dataset.hh>
#include <Image.hh>
#include <Event.hh>
#include <Waveform.hh>

#include <vector>
#include <iostream>
#include <cstdlib>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TTree.h>
#include <TCanvas.h>

int main(int argc, char ** argv)
{   

  dmtpc::core::Dataset * d = new dmtpc::core::Dataset;
  int runno = argc > 1 ? atoi(argv[1]) : 281;
  d->open(TString::Format("/net/zwicky/dmtpc/data/m3/GainMap/raw/dmtpc_m3_%05d.root",runno));
  
  d->getEvent(0);
  //vmin/vmax don't change every event, just every run and channel
  double vmin = d->event()->getScopeConfig(0)->channel_info[0].vmin*1000;
  double vmax = d->event()->getScopeConfig(0)->channel_info[0].vmax*1000;
  std::cout << "vmin: " << vmin << " vmax : " << vmax << std::endl;
  
  /*** get pretrigger window ***/
    
  //pretrigger window doesn't change every event, just every run
  size_t pretrigger = d->event()->getScopeConfig(0)->channel_info[0].nsamples_pretrigger;
  
  double baseline_window_start = 0.1 * pretrigger; 
  double baseline_window_end = 0.11 * pretrigger; 
    
  std::cout << "pretrigger samples total: " << pretrigger << ", bin " << baseline_window_start << 
    " to " << baseline_window_end << std::endl;

  /*** initialize histograms ***/
  
  TH1D * h1 = new TH1D(TString::Format("baseline_average_run%04d",d->event()->run()),
      "Average Baseline (all); mV ; number per voltage bin", 800, -50, 50);
  TH1D * h2= new TH1D(TString::Format("peak_maximum_run%04d",d->event()->run()),
	"Peak Maxima (max value);mV; # per voltage bin", 100, -50, 350);
  TH1D * h3= new TH1D(TString::Format("peak_amplitude_run%04d",d->event()->run()),
	"Peak Amplitudes (baseline subtracted);mV; # per voltage bin", 100,-50,350);

  std::cout << "run: " << d->event()->run() << " # events: " << d->nevents() << std::endl;

  /*** get peak amplitude for each trace in each event ***/

  for(int k=0; k < d->nevents(); k++)//d->nevents(); k++) //k < 10 for short debug runs
  {
    d->getEvent(k);
    int evt = d->event()->ev();
    int ntraces = d->event()->nScopeData();
    if((evt % 25) == 0)
    {
      std::cout << "Event " << evt << " # traces: " << ntraces << std::endl; 
    }
    
    //TODO: for multiple channels
    //unsigned int number = d->event()->nscopes();
    //std::cout << "num scopes: " << number  << std::endl;
   
    //TODO: make for either scope; just for scope 0 now 
    //size_t scope =  d->event()->getScopeConfig(0)->scope_number;
    //std::cout << "scope via event: " << scope  << std::endl;
    //size_t scope2 =  d->event()->getTrace(0)->getInfo()->scope;
    //std::cout << "scope via trace: " << scope2  << std::endl;
    
    double baseline = 0; 
    double peak_max = 0;
    double peak_amplitude = 0;
    double reject_counter = 0;
    for(int j = 0; j < ntraces; j++)
    { 
      /* average the baseline */
      double sum = 0; //sum over specified pre-trigger bins
      for(int i = (int)baseline_window_start; i<(int)baseline_window_end; i++)
      {
        sum+= d->event()->getTrace(j)->GetPhysicalBinContent(i);
        //sum += d->event()->getTrace(j)->GetBinContent(i);
      }
      //std::cout << "sum: " << sum << std::endl;
      baseline = sum/(baseline_window_end - baseline_window_start + 1); //take average
      h1->Fill(baseline); 
      //std::cout << " baseline: " << baseline << std::endl;
     
      unsigned peak_max_bin = d->event()->getTrace(j)->getPhysical()->GetMaximumBin(); 
      //std::cout << "peak max bin: " << peak_max_bin << std::endl;
      if((peak_max_bin > pretrigger+5000) || (peak_max_bin < pretrigger-2000))
	  {
            reject_counter++; //discriminate if max not within certain window
	    //std::cout << "EVENT " << k << " TRACE " << j << " REJECTED" << std::endl;
	    continue;
	  }
	  else
	  {
	    peak_max = d->event()->getTrace(j)->getPhysical()->GetMaximum();//GetPhysicalBinContent(peak_max_bin); should work too...maybe faster?
 	    peak_amplitude = peak_max - baseline; 
	    //std::cout << "max: " << peak_max << std::endl;
	    h2->Fill(peak_max);
	    //std::cout << "amplitude: " << peak_amplitude << std::endl;
	    h3->Fill(peak_amplitude);
	  }

    }
    
    if((evt % 25)==0) 
    {
      std::cout << "Baseline for event " << evt << " = " << baseline << std::endl;
      std::cout << "Peak max for event " << evt << " = " << peak_max << " peak amplitude = " << peak_amplitude << std::endl;
      std::cout << "Traces rejected = " << reject_counter << std::endl;
    }

  }
  
  //int ntraces_total = (d->nevents())*(d->event()->nScopeData());
  double baseline_avg = h1->GetXaxis()->GetBinCenter(h1->GetMaximumBin()); //h1->GetMaximumBin();
  std::cout << "avg baseline value: " << baseline_avg << std::endl;  
 
  /* 
  ***** calculated peak amplitude using avg baseline, not per-trace baseline *****
  for(int e = 0; e < d->nevents(); e++)
  { 
    d->getEvent(e);
    int evt = d->event()->ev();
        for(unsigned t = 0; t < d->event()->nScopeData();t++)
    {
	  unsigned peak_max_bin = d->event()->getTrace(t)->getPhysical()->GetMaximumBin();
	  //std::cout << "peak max bin: " << peak_max_bin << std::endl;
	  if((peak_max_bin > pretrigger+500) || (peak_max_bin < pretrigger-1000))
	  {//std::cout << "TRACE REJECTED" << std::endl;
	   continue;
	  }
	  else
	  {
	    peak_max = d->event()->getTrace(t)->getPhysical()->GetMaximum(vmax);
 	    peak_amplitude = peak_max - baseline_avg;
	    //std::cout << "max: " << peak_max << std::endl;
	    h2->Fill(peak_max);
	    //std::cout << "amplitude: " << peak_amplitude << std::endl;
	    h3->Fill(peak_amplitude);
	  }
    }
    if((evt % 25) == 0)
    {
      std::cout << "Peak max for event " << evt << " = " << peak_max << " peak amplitude = " << peak_amplitude << std::endl;
    }
  }
  */
  
  std::cout << "Peak amplitude hist made" << std::endl;
  unsigned run = d->event()->run();
  TFile save(TString::Format("/net/zwicky/dmtpc/nmg/analysis/make_peak_hist/rootfiles/run_%05i_peak_hist.root",run), "recreate");
    
  save.cd();
  h1->Write();
  h2->Write();
  h3->Write();
  save.Write();
  save.Close();
}
//by Natalia Guerrero (:
