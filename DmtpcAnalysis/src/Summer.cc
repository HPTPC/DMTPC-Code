#include "SkimEvent.hh" 
#include <cmath>
#include "TChain.h" 
#include <cstdio>
#include "TH2.h" 
#include "TFile.h"
#include <vector>


int main (int nargs, char ** args) 
{

  dmtpc::analysis::SkimEvent * ev = new dmtpc::analysis::SkimEvent; 

  TChain ch("skim"); 

  double maxE = getenv("SUMMER_MAX_E") ? atof(getenv("SUMMER_MAX_E")) : 5e4; 
  int maxburnin = getenv("SUMMER_MAX_BURNIN") ? atoi(getenv("SUMMER_MAX_BURNIN")) : 4; 
  double max_maxpixel = getenv("SUMMER_MAX_MAXPIXEL") ? atof(getenv("SUMMER_MAX_MAXPIXEL")) : 80; 
  double max_npixel = getenv("SUMMER_MAX_NPIXEL") ? atoi(getenv("SUMMER_MAX_NPIXEL")) : 500; 
  char * sparktree = getenv("SUMMER_SPARKTREE") ? getenv("SUMMER_SPARKTREE") : 0; 
  double  v = getenv("SUMMER_V") ? atof(getenv("SUMMER_V")) : 800; 
  double  imax = getenv("SUMMER_I") ? atof(getenv("SUMMER_I")) : 0.05; 

  if (nargs < 3) 
  {
    printf ("Summer out.root skim1.root .."); 
    return 1; 
  }



  for (int i = 2; i < nargs; i++)
  {
    ch.Add(args[i]); 
  }


  TChain spark("spark"); 

  if (sparktree) spark.Add(sparktree); 



  printf("number of entries: %d \n", (int) ch.GetEntries()); 


  ch.SetBranchAddress("event",&ev); 

  ch.GetEntry(0); 
  int ncam = ev->ncamera(); 

  TFile out(args[1],"CREATE"); 
  std::vector<TH2 *> avg (ncam) ; 
  std::vector<TH2 *> norm (ncam) ; 


  for (int i = 0; i < ncam; i++)
  {

    avg[i] = new TH2D(TString::Format("avg%d",i), TString::Format("average for %s", ev->cameraSerialNumber(i).Data()), 
                      ev->image(i)->GetNbinsX(), ev->image(i)->GetXaxis()->GetXmin(),  ev->image(i)->GetXaxis()->GetXmax(), 
                      ev->image(i)->GetNbinsY(), ev->image(i)->GetYaxis()->GetXmin(),  ev->image(i)->GetYaxis()->GetXmax());  

    norm[i] = new TH2D(TString::Format("norm%d",i), TString::Format("counts for %s", ev->cameraSerialNumber(i).Data()), 
                      ev->image(i)->GetNbinsX(), ev->image(i)->GetXaxis()->GetXmin(),  ev->image(i)->GetXaxis()->GetXmax(), 
                      ev->image(i)->GetNbinsY(), ev->image(i)->GetYaxis()->GetXmin(),  ev->image(i)->GetYaxis()->GetXmax());  
  }


  int spark_run; 
  int spark_event; 
  double spark_vmon;
  double spark_imon;

  if (sparktree)
  {
    spark.SetBranchAddress("run",&spark_run); 
    spark.SetBranchAddress("event",&spark_event); 
    spark.SetBranchAddress("vmon",&spark_vmon); 
    spark.SetBranchAddress("imon",&spark_imon); 

    spark.BuildIndex("run","event"); 
    spark.GetEntry(0);
  }


  for (int i = 0; i < ch.GetEntries(); i++)
  {
    ch.GetEntry(i); 

    if (i % 100 == 0) printf("processing entry %d\n",i); 

    if (sparktree)
    {
      if (spark.GetEntryWithIndex(ev->runNumber(), ev->eventNumber())) 
      {
        if (fabs(spark_vmon-v) > 1 || spark_imon > imax) continue; 
      }

    }

    for (int c = 0; c < ncam; c++)
    {
      if (ev->spark(c)) continue; 
    
      for (int t = 0; t < ev->ntracks(c); t++)
      {
        if (ev->E(c,t) > maxE) continue; 
        if (ev->nburnin(c,t) > maxburnin) continue; 
        if (ev->maxpixel(c,t) > max_maxpixel) continue; 
        if (ev->npixel(c,t) > max_npixel) continue; 

        const std::vector<int> *pix = & (ev->cluster(c)->getCluster(t)->pixels); 

        for (unsigned p = 0; p < pix->size(); p++)
        {
          norm[c]->AddBinContent(pix->at(p)); 
          avg[c]->AddBinContent(pix->at(p), ev->image(c)->GetBinContent(pix->at(p))); 
        }
      }
    }
  }


  out.cd();
  for (int i = 0; i < ncam; i++)
  {
    norm[i]->Write(); 
    avg[i]->Divide(norm[i]); 
    avg[i]->Write(); 
  }


  out.Close(); 
  return 0; 



}
