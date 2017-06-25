#include "HVLog.hh"
#include <cstdlib>
#include <algorithm>
#include "TChain.h"
#include "SkimEvent.hh"

#include "TFile.h" 
#include "TTree.h" 


dmtpc::analysis::HVLog::HVLog( unsigned n_imon, const double * imon_v, const double * imon_t,
               unsigned n_vmon, const double * vmon_v, const double * vmon_t,  
               unsigned n_vset, const double * vset_v, const double * vset_t) 
:  imon(n_imon), vmon(n_vmon), vset(n_vset) , 
   imon_time(n_imon), vmon_time(n_vmon), vset_time(n_vset) 
{

  memcpy(&imon[0], imon_v, sizeof(double) * n_imon); 
  memcpy(&vmon[0], vmon_v, sizeof(double) * n_vmon); 
  memcpy(&vset[0], vset_v, sizeof(double) * n_vset); 
  memcpy(&imon_time[0], imon_t, sizeof(double) * n_imon); 
  memcpy(&vmon_time[0], vmon_t, sizeof(double) * n_vmon); 
  memcpy(&vset_time[0], vset_t, sizeof(double) * n_vset); 

}




int dmtpc::analysis::HVLog::makeSparkFriend(TChain * c, const char *
    outputfile, const char * tree_name, double t_before, double t_after,bool
    overwrite, int max_run_with_biasframe_timestamp_bug, double
    extra_padding_for_bug) { TFile output(outputfile, overwrite ? "RECREATE":
      "CREATE"); 

  if (!output.IsOpen())
  {
    fprintf(stderr,"%s not open, perhaps already created and not overwriting?\n", outputfile); 
    return 1; 
  }

  int run; 
  int event; 
  c->SetEstimate(c->GetEntries()+1); 
  c->Draw("timestamp.fSec + 1.e-9*timestamp.fNanoSec:runNumber:eventNumber","","goff"); 


  TTree * tree = new TTree(tree_name, tree_name); 
  double vset; 
  double vmon_avg; 
  double imon_avg; 
  double imon;
  double vmon;
  double imon_max; 
  double t = 0; 
  double last_t = 0; 
  double avg_delta_t = 0; 
  double dt = 0;

  tree->Branch("t",&t); 
  tree->Branch("dt",&dt); 
  tree->Branch("vset",&vset); 
  tree->Branch("vmon_avg",&vmon_avg); 
  tree->Branch("imon_max",&imon_max); 
  tree->Branch("vmon",&vmon); 
  tree->Branch("imon",&imon); 
  tree->Branch("run",&run); 
  tree->Branch("event",&event); 

  double t900 = 0; 

  for (long int i = 0; i < c->GetEntries(); i++)
  {
//    printf("%d ",i); 
    t  = c->GetV1()[i]; 
    int lastrun = run;
    run = (int) c->GetV2()[i]; 
    event = (int) c->GetV3()[i]; 


    double t_before_adjusted = t_before; 
    double t_after_adjusted = t_after; 

    if (lastrun!=run)
    {
      printf("Processing run %d\n", run); fflush(stdout); 
    }


    if (run <= max_run_with_biasframe_timestamp_bug) 
    {
//        printf("run below biasframe timestamp bug cutoff\n"); fflush(stdout); 

        /*  hardcoded, but true for all runs with this bug! */
        if (event < 900)
        {
//          printf("adjusting t from %f",t); 
          t = c->GetV1()[i+100]; //skip 100 for bias frame
//          printf("to %f\n",t); 
          if (event > 0)
          {
            avg_delta_t += t-last_t; 
          }
          else
          {
            avg_delta_t = 0; 
          }

        }


        else
        {
          if (event == 900)
          {
            avg_delta_t /=900; 
            printf("Average Delta T = %f\n", avg_delta_t); 
            t900 = last_t; 
          }

          t = t900 + avg_delta_t * (1+event - 900.);  //extrapolate, based on averages
          printf("t = %f\n",t); 

          t_before_adjusted += extra_padding_for_bug; 
          t_after_adjusted += extra_padding_for_bug; 
        }
    }
    
    last_t = t; 

    
    vset = getVset(t); 
    vmon_avg = getAvgVmon(t - t_before_adjusted, t+ t_after_adjusted); 
    imon_avg = getAvgImon(t - t_before_adjusted, t+ t_after_adjusted); 
    imon_max = getMaxImon(t - t_before_adjusted, t+ t_after_adjusted); 
    imon = getImon(t,&dt); 
    vmon = getVmon(t); 
    tree->Fill();
  }

  tree->Write(); 
  output.Close(); 
  return 0; 


}

dmtpc::analysis::HVLog::HVLog(const char * file_name, const char * imon_tree_name, const char * vmon_tree_name, const char * vset_tree_name, const char * valname, const char * timename) 
{

  TFile f(file_name);



  TTree * imon_tree = (TTree*) f.Get(imon_tree_name); 
  TTree * vmon_tree = (TTree*) f.Get(vmon_tree_name); 
  TTree * vset_tree = (TTree*) f.Get(vset_tree_name); 

  imon_tree->SetEstimate(imon_tree->GetEntries()+1); 
  vmon_tree->SetEstimate(vmon_tree->GetEntries()+1); 
  vset_tree->SetEstimate(vset_tree->GetEntries()+1); 

  long int n_imon = imon_tree->Draw(TString::Format("%s:%s", valname, timename),"","goff");
  long int n_vmon = vmon_tree->Draw(TString::Format("%s:%s", valname, timename),"","goff");
  long int n_vset =  vset_tree->Draw(TString::Format("%s:%s", valname, timename),"","goff");


  imon.insert(imon.end(), n_imon,0); 
  imon_time.insert(imon_time.end(), n_imon,0); 
  vmon.insert(vmon.end(), n_vmon,0); 
  vmon_time.insert(vmon_time.end(), n_vmon,0); 
  vset.insert(vset.end(), n_vset,0); 
  vset_time.insert(vset_time.end(), n_vset,0); 


  memcpy(&imon[0], imon_tree->GetV1(), sizeof(double) * n_imon); 
  memcpy(&vmon[0], vmon_tree->GetV1(), sizeof(double) * n_vmon); 
  memcpy(&vset[0], vset_tree->GetV1(), sizeof(double) * n_vset); 
  memcpy(&imon_time[0], imon_tree->GetV2(), sizeof(double) * n_imon); 
  memcpy(&vmon_time[0], vmon_tree->GetV2(), sizeof(double) * n_vmon); 
  memcpy(&vset_time[0], vset_tree->GetV2(), sizeof(double) * n_vset); 


}


static double getVal(const std::vector<double> * vals, const std::vector<double> *ts, double t, double *dt = 0)
{
  size_t low = std::lower_bound(ts->begin(), ts->end(),t) - ts->begin() - 1; 
//  printf("%zu\n",low); 
  if (dt) *dt = t - ts->at(low) ; 
  return vals->at(low); 
//  unsigned up = std::upper_bound(ts->begin(), ts->end(),*t,timestamp_cmp) - ts->begin(); 
//
//
//  if (low == up) 
//  {
//    return vals->at(low); 
//  }


//  double T = t->AsDouble(); 
//  double t0 = ts->at(low).AsDouble(); 
//  double t1 = ts->at(up).AsDouble(); 
//  double diff = t1 - t0; 
//  double frac = (T - t0)/diff; 
//  return vals->at(low) * (1-frac) + vals->at(up) * frac; 

}

static double getMaxVal(const std::vector<double> * vals, const std::vector<double> *ts, double t0, double t1)
{
  unsigned long low = std::lower_bound(ts->begin(), ts->end(),t0) - ts->begin(); 
  unsigned long up = std::upper_bound(ts->begin(), ts->end(),t1) - ts->begin(); 

  double v= getVal(vals,ts,t0); 

  double vm = getVal(vals,ts,t1); 
  if (vm > v) v = vm; 

  for(unsigned long i = low +1;  i < up; i++)
  {
    if (vals->at(i) > v) v = vals->at(i); 
  }

  return v; 
}

static double getAvgVal(const std::vector<double> * vals, const std::vector<double>* ts, double t0, double t1)
{
  unsigned long low = std::lower_bound(ts->begin(), ts->end(),t0) - ts->begin(); 
  unsigned long up = std::upper_bound(ts->begin(), ts->end(),t1) - ts->begin(); 

  double sum = 0; 

  sum += getVal(vals,ts,t0); 

  if(low!=up)
    sum+=  getVal(vals,ts,t1); 
  int n = up - low+1; 

  for(unsigned long i = low +1;  i < up; i++)
  {
    sum+= vals->at(i); 
  }

  return sum /n; 
} 
    

double dmtpc::analysis::HVLog::getImon(double t, double *dt) const 
{
  return getVal(&imon, &imon_time, t, dt); 
}


double dmtpc::analysis::HVLog::getVmon(double t, double *dt) const 
{
  return getVal(&vmon, &vmon_time, t,dt); 
}

double dmtpc::analysis::HVLog::getVset(double t, double *dt) const 
{
  return getVal(&vset, &vset_time, t,dt); 
}


double dmtpc::analysis::HVLog::getMaxImon( double t0, double t1) const 
{
  return getMaxVal(&imon, &imon_time, t0,t1); 
}


double dmtpc::analysis::HVLog::getAvgImon( double t0, double t1) const 
{
  return getAvgVal(&imon, &imon_time, t0,t1); 
}

double dmtpc::analysis::HVLog::getAvgVmon( double t0, double t1) const 
{
  return getAvgVal(&vmon, &vmon_time, t0,t1); 
}
