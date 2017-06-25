#include <cstdio>

#include "TrackFitter.hh"
#include <iostream>
#include "TGraphAsymmErrors.h"
#include "ParametricVonMisesHTTest.hh"
#include "Angular.hh"
#include "TCut.h" 
#include "TChain.h" 
#include "TMath.h"
#include "TFile.h" 
#include <fstream>
#include <istream>
#include "EnergyTest.hh" 
#include <set>
#include "TRandom3.h"
#include <fenv.h>


const double mean = -M_PI/2. ; 

int hist_nEbins = 25; 
double  hist_Emin = 0; 
double  hist_Emax = 150; 

int hist_nphibins = 30; 
double  hist_phimin = -TMath::Pi(); 
double  hist_phimax = TMath::Pi(); 

const char * red_text = "\e[31m"; 
const char * blue_text = "\e[34m"; 
const char * normal_text = "\e[0m"; 


static double Eerr_fn(double Eerr)
{
  if (Eerr < 2) return 3; 
  if (Eerr > 7) return 5.5; 

  return 3 + (Eerr - 2) * 2.5 / 5;  

}


static double phierr_fn(double E, double phierr)
{

  double Eprediction = 21.88 / E; 

  if (phierr > 0.55) phierr = 0.55; 

  double phiPrediction = 2.36 * pow(phierr+ 0.0072,0.896); 

  return Eprediction/2 + phiPrediction/2; 
}



double error_fn_expo(double *x, double * par)
{
  return  (0.5 + 0.5*TMath::Erf((*x-par[1])/(sqrt(2)*par[2])) ) * exp(-(*x)/par[0]); 
}

static int last_norm_i = 0; 
static double last_norms[3] = {0,0,0}; 
static double last_norm_par[3][3] = {{0,0,0}, {0,0,0}, {0,0,0}}; 


double norml_fn_impl(double *x, double *par)
{

  int norm_i = -1; 
  for (int i = 0; i < 3; i++)
  {
    int cached_i = (last_norm_i + i) % 3 ;
    if (!memcmp(par,last_norm_par[cached_i], sizeof(last_norm_par[cached_i])))
    {
       norm_i = cached_i; 
    }

  }

  if (norm_i < 0)
  {
    norm_i = (last_norm_i + 1) %3 ; 
    TF1 cn ("calculate_norm",error_fn_expo, 0, 250,3); 
    cn.SetParameters(par); 
    last_norms[norm_i] = cn.Integral(0,250); 
    printf("%f\n",last_norms[norm_i]); 
    memcpy(last_norm_par[norm_i], par, sizeof(last_norm_par[norm_i])); 
  }
  double norm = last_norms[norm_i]; 

  last_norm_i = norm_i; 
  if (!norm) return 0; 

  return par[3]/norm * error_fn_expo(x,par); 

}

double width_scale = 200; 

double width_fn_impl(double *x, double * par)
{
  double X = *x/width_scale; 
  if (X > 1) X = 1; 
  if (X < 0) X = 0; 

  double min_K = par[0]; 
  if (min_K < 0) min_K = 0; 
  double max_K = par[1]; 
  if (max_K < min_K) max_K = min_K; 
  double alpha = fabs(par[2]); 
  double beta = fabs(par[3]); 
  

  return min_K + (max_K - min_K)  * TMath::BetaIncomplete(X, alpha, beta);  

}

double ht_fn_impl(double *x, double *par)
{
  double x0 = par[0]; 
  double width = par[1]; 
  double max = par[2]; 

  if (width == 0) return  max * (*x > x0);   
  
  double val =  (max + 0.5)/2.  + (max - 0.5)/2. *  TMath::Erf((*x-x0)/(sqrt(2)*width)); 

  if (val < 0.5) val = 0.5; 
  if (val > 1) val = 1;
  return val; 
}


double mean_fn_impl(double *x, double * par) 
{

  (void) x;
  return *par; 

}

int main(int nargs, char **args)
{

  const unsigned phipar = dmtpc::analysis::TrackFitter::PAR_PHI; 
  const unsigned Epar = dmtpc::analysis::TrackFitter::PAR_E; 


  dmtpc::analysis::TrackFitter::Result *r = new dmtpc::analysis::TrackFitter::Result;

//  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);

  if (nargs < 4)
  {
    fprintf(stderr,"Usage: FitVonMisesHTTest out.root filesSignal.txt filesHypothesis1.txt filesHypothesis2.txt... \n" ); 
    return 1; 
  }
  
  char * env_ntrials = getenv("VON_MISES_NTRIALS"); 
  int ntrials = env_ntrials ? atoi(env_ntrials) : 1000; 

  char * env_min_n = getenv("VON_MISES_MIN_N"); 
  int min_n = env_min_n ? atoi(env_min_n) : 10; 
  char * env_max_n = getenv("VON_MISES_MAX_N"); 
  int max_n = env_max_n ? atoi(env_max_n) : 400; 
  char * env_n_step = getenv("VON_MISES_N_STEP"); 
  int n_step = env_n_step ? atoi(env_n_step) : 10; 

 
  char * env_seed = getenv("VON_MISES_SEED"); 
  long seed = env_seed ? atol(env_seed) : 0; 


  char * cutstring = getenv("VON_MISES_CUT"); 
  if (!cutstring) cutstring = "chisq/ndof < 2 && abs(x0.val - mcX) < 20 && abs(y0.val - mcY) < 20"; 


  TRandom3 rand(seed); 

  TChain * signal = new TChain("fit"); 

  std::vector<TChain*> hypos; 


  std::ifstream sigstream(args[2]); 
  std::string tmp; 


  sigstream >> tmp; 

  signal->SetTitle(tmp.c_str()); 


  while (sigstream >> tmp) 
  {
    signal->Add(tmp.c_str()); 
  }
 
  //Create Output Tree
  TFile out(args[1],"RECREATE"); 

  TH2F * signal_hist = new TH2F("signal_dist","Signal Distribution", 
                                hist_nEbins,hist_Emin, hist_Emax, 
                                hist_nphibins, hist_phimin, hist_phimax); 



  signal->Draw(" htphi.val : Ephys.val>> signal_dist",TString::Format("prob2*(%s)",cutstring),"goff"); 
  signal->Draw(" htphi.val  + pi: Ephys.val>> +signal_dist",TString::Format("(1.-prob2)*(%s)",cutstring),"goff"); 

  TH2F * signal_hist_noht = new TH2F("signal_dist_noht","Signal Distribution (no head-tail)", 
                                hist_nEbins,hist_Emin, hist_Emax, 
                                hist_nphibins, hist_phimin, hist_phimax); 

  signal->Draw(" htphi.val : Ephys.val>> signal_dist_noht",cutstring,"goff"); 


  out.cd(); 
  signal_hist->Write(); 
  signal_hist_noht->Write(); 

  printf("Entries in signal: %d\n", (int) signal->GetEntries()); 

  std::vector<TH2*> hypo_hists; 


  for (int i = 3; i <nargs; i++)
  {
    TChain* c = new TChain("fit"); 
    std::ifstream hypostream(args[i]); 
    hypostream >> tmp; 
    c->SetTitle(tmp.c_str()); 
    while (hypostream >> tmp) 
    {
     c->Add(tmp.c_str()); 
    }

    hypos.push_back(c); 

    TH2F * hypo_hist = new TH2F(TString::Format("%s_dist",c->GetTitle()),TString::Format("Distribution for Hypothesis %s", c->GetTitle()), 
                                hist_nEbins,hist_Emin, hist_Emax, 
                                hist_nphibins, hist_phimin, hist_phimax); 

    c->Draw(TString::Format("dmtpc::math::angle::normalize(htphi.val)  : Ephys.val >> %s_dist",c->GetTitle()),TString::Format("prob2*(%s)",cutstring),"goff"); 
    c->Draw(TString::Format("dmtpc::math::angle::normalize(htphi.val + pi)  : Ephys.val >> +%s_dist",c->GetTitle()),TString::Format("(1-prob2)*(%s)",cutstring),"goff"); 

    TH2F * hypo_hist_noht = new TH2F(TString::Format("%s_dist_noht",c->GetTitle()),TString::Format("Distribution for Hypothesis %s (no head-tail)", c->GetTitle()), 
                                hist_nEbins,hist_Emin, hist_Emax, 
                                hist_nphibins, hist_phimin, hist_phimax); 
    c->Draw(TString::Format("htphi.val  : Ephys.val >> %s_dist_noht",c->GetTitle()),cutstring,"goff"); 

    out.cd(); 
    hypo_hist->Write(); 
    hypo_hist_noht->Write(); 
    hypo_hists.push_back(hypo_hist); 
    printf("Entries in %s: %d\n", c->GetTitle(), (int) c->GetEntries()); 
  }



  int n; 
  


  std::vector<double> likelihoods(hypos.size()); 


  TTree * tree = new TTree("vonmises","Parameterized Von Mises Test");  
  tree->Branch("n",&n); 

  TH2* sample_dist = new TH2F("sample_dist","Distribution for Sample", 
                              hist_nEbins,hist_Emin, hist_Emax, 
                              hist_nphibins, hist_phimin, hist_phimax); 


  tree->Branch("sample_dist",&sample_dist); 

  for (unsigned i = 0; i < hypos.size(); i++)
  {
    tree->Branch(TString::Format("vm_likelihood_%s", hypos[i]->GetTitle()), &(likelihoods[i])); 
  }




  TCut cut = TCut (cutstring); 

  // Fill A vector of WIMP tests 
  std::vector<dmtpc::math::ParametricVonMisesHTTest*> hypotests(hypos.size()); 



  for (unsigned i = 0; i < hypos.size(); i++)
  {
    TDirectoryFile * dir = (TDirectoryFile*) out.mkdir(hypos[i]->GetTitle());
    dir->cd(); 

    TF1 * width_fn = new TF1(TString::Format("%s_width_fn", hypos[i]->GetTitle()), width_fn_impl, 0, 200, 4); 

    width_fn->SetParName(0,"k_min"); 
    width_fn->FixParameter(0,0); 
//    width_fn->SetParLimits(0,0,10); 
//    width_fn->SetParError(0,0.1); 
    width_fn->SetParName(1,"k_max"); 
    width_fn->SetParameter(1,10); 
    width_fn->SetParLimits(1,-1,10); 
    width_fn->SetParError(1,1); 
    width_fn->SetParName(2,"k_alpha"); 
    width_fn->SetParLimits(2,0,10); 
    width_fn->SetParameter(2,2); 
    width_fn->SetParError(2,0.5); 
    width_fn->SetParName(3,"k_beta"); 
    width_fn->SetParLimits(3,0,10); 
    width_fn->SetParameter(3,2); 
    width_fn->SetParError(3,0.5); 


    TF1 * ht_fn = new TF1(TString::Format("%s_ht_fn", hypos[i]->GetTitle()), ht_fn_impl, 0, 200, 3); 
    ht_fn->SetParName(0,"ht_x0"); 
    ht_fn->SetParameter(0,150); 
    ht_fn->SetParError(0,5); 
    ht_fn->SetParLimits(0,0,500);
    ht_fn->SetParName(1,"ht_width"); 
    ht_fn->SetParLimits(1,0,500);
    ht_fn->SetParameter(1,50); 
    ht_fn->SetParError(1,5); 
    ht_fn->SetParName(2,"ht_max"); 
    ht_fn->SetParLimits(2,0.5,1);
    ht_fn->SetParameter(2,0.6); 
    ht_fn->SetParError(2,0.1); 






    TF1 * normalization_fn= new TF1(TString::Format("%s_normal_fn", hypos[i]->GetTitle()), norml_fn_impl, 0, 200, 4); 
    normalization_fn->SetParName(0,"norml_tau"); 
    normalization_fn->SetParameter(0,15); 
    normalization_fn->SetParLimits(0,0,500); 
    normalization_fn->SetParError(0,0.1); 
    normalization_fn->SetParName(1,"norml_thresh"); 
    normalization_fn->SetParLimits(1,5,30); 
    normalization_fn->SetParameter(1,15); 
    normalization_fn->SetParError(1,1); 
    normalization_fn->SetParName(2,"norml_thresh_width"); 
    normalization_fn->SetParameter(2,5); 
    normalization_fn->SetParLimits(2,0,10); 
    normalization_fn->SetParError(2,0.5); 

    normalization_fn->SetParName(3,"norml_norml"); 
    normalization_fn->SetParameter(3,225); 
    normalization_fn->SetParError(3,50); 


    TF1 * mean_fn = new TF1(TString::Format("%s_mean_fn",hypos[i]->GetTitle()), mean_fn_impl, 0,200,1); 
    mean_fn->FixParameter(0,mean); 
    mean_fn->SetParName(0,"mean"); 
    mean_fn->Write();

//    TObjArray slices; 


    TH1 *ht_hist = new TH1D(TString::Format("ht_fit_hist_%s", hypo_hists[i]->GetName()), "HT Parameter", hypo_hists[i]->GetNbinsX(), hypo_hists[i]->GetXaxis()->GetXmin(), hypo_hists[i]->GetXaxis()->GetXmax()); 
    TH1 *norm_hist = new TH1D(TString::Format("norm_fit_hist_%s", hypo_hists[i]->GetName()), "Normalization Parameter", hypo_hists[i]->GetNbinsX(), hypo_hists[i]->GetXaxis()->GetXmin(), hypo_hists[i]->GetXaxis()->GetXmax()); 
    TH1 *mean_hist = new TH1D(TString::Format("mean_fit_hist_%s", hypo_hists[i]->GetName()), "Mean Parameter", hypo_hists[i]->GetNbinsX(), hypo_hists[i]->GetXaxis()->GetXmin(), hypo_hists[i]->GetXaxis()->GetXmax()); 
    TH1 *width_hist = new TH1D(TString::Format("width_fit_hist_%s", hypo_hists[i]->GetName()), "Width Parameter", hypo_hists[i]->GetNbinsX(), hypo_hists[i]->GetXaxis()->GetXmin(), hypo_hists[i]->GetXaxis()->GetXmax()); 

    for (int x = 1; x <= hypo_hists[i]->GetNbinsX(); x++) 
    {

      TF1 *vmf= new TF1(TString::Format("vmf_%s_%d",hypo_hists[i]->GetName(),x), &dmtpc::math::angle::von_mises_dist_ht, -TMath::Pi(), TMath::Pi(), 4); 

      TH1 * projhist = hypo_hists[i]->ProjectionY(TString::Format("proj_hist_%s_%d",hypo_hists[i]->GetName(),x), x,x); 
      if (projhist->Integral() < 5) 
        continue; 

      vmf->SetParameter(0,.5); 
      vmf->SetParLimits(0,0,1); 
      vmf->SetParameter(1,projhist->Integral()); 
      vmf->FixParameter(2, mean); 
      vmf->SetParameter(3,1); 

      projhist->Fit(vmf,"LIME"); 
      projhist->Write(); 


      ht_hist->SetBinContent(x, vmf->GetParameter(0)); 
      ht_hist->SetBinError(x, vmf->GetParError(0)); 
      norm_hist->SetBinContent(x, vmf->GetParameter(1)); 
      norm_hist->SetBinError(x, vmf->GetParError(1)); 

      mean_hist->SetBinContent(x, vmf->GetParameter(2)); 
      mean_hist->SetBinError(x, vmf->GetParError(2)); 

      width_hist->SetBinContent(x, vmf->GetParameter(3)); 
      width_hist->SetBinError(x, vmf->GetParError(3)); 

    }

    ht_hist->Fit(ht_fn,"MIVE"); 
    ht_hist->Write(); 
    norm_hist->Fit(normalization_fn,"LMIVE"); 
    norm_hist->Write(); 
    width_hist->Fit(width_fn,"MIVE"); 
    width_hist->Write(); 
    


    normalization_fn->Write(TString::Format("%s_init",normalization_fn->GetName())); 
    ht_fn->Write(TString::Format("%s_init",ht_fn->GetName())); 
    width_fn->Write(TString::Format("%s_init",width_fn->GetName())); 


    TH2F * hypo_fn = new TH2F(TString::Format("%s_model_dist",hypos[i]->GetTitle()),
                                TString::Format("Fit Model for Hypothesis %s", hypos[i]->GetTitle()), 
                                hist_nEbins,hist_Emin, hist_Emax, 
                                hist_nphibins, hist_phimin, hist_phimax); 


    normalization_fn->FixParameter(3,1); // fix normalization 

//    for (int ni = 0; ni < normalization_fn->GetNpar(); ni++)
//    {
//      normalization_fn->FixParameter(ni, normalization_fn->GetParameter(ni)); //fix normalization to avoid explosion! 
//    }



    hypotests[i] = new dmtpc::math::ParametricVonMisesHTTest(2, width_fn, ht_fn, normalization_fn, mean_fn); 

    for (int x = 1; x <= hypo_fn->GetNbinsX(); x++)
    {
      for (int y = 1; y <= hypo_fn->GetNbinsY(); y++)
      {

        double E = hypo_fn->GetXaxis()->GetBinCenter(x); 
        double phi = hypo_fn->GetYaxis()->GetBinCenter(y);  
        hypo_fn->SetBinContent(x,y, hypotests[i]->f(phi,&E)); 
      }
    }

    hypo_fn->Write(TString::Format("%s_init",hypo_fn->GetName())); 
 
    dir->Save(); 
    hypotests[i]->setMaxCalls(50000);
    hypotests[i]->setMaxIter(5000);

    int npass = hypos[i]->Draw("Entry$:Ephys.val:htphi.val:prob2", cut,"goff"); 


    std::vector<dmtpc::math::ParametricVonMisesHTTest::Entry> entries;
    entries.reserve(npass); 


    hypos[i]->SetBranchAddress("result",&r); 

    std::vector<double *> covar_store;
    covar_store.reserve(npass);

    for (int j = 0; j < npass; j++)
    {
      if (!r->covar[phipar][phipar] || ! r->covar[Epar][Epar]) 
      {
        continue;
      }


      dmtpc::math::ParametricVonMisesHTTest::Entry entry; 
      entry.phi = hypos[i]->GetV3()[j]; 
      entry.phi_ht = hypos[i]->GetV4()[j]; 
      entry.X = &(hypos[i]->GetV2()[j]); 
      hypos[i]->GetEntry((int) hypos[i]->GetV1()[j]); 

      double * covar = new double[4]; 
      double gain = r->gain; 


      /*
      for (unsigned mm = 0; mm < 7; mm++)
      {
        for (unsigned nn = 0; nn < 7; nn++)
        {
         std::cout << r->covar[mm][nn] << "\t\t"; 

        }
        std::cout << std::endl; 
      }
        std::cout << std::endl; 
        std::cout << std::endl; 
        */

//      std::cout << r->covar[phipar][phipar] << std::endl; 
      covar[0] = r->covar[phipar][phipar]; 
 //     std::cout << r->covar[phipar][Epar] << std::endl; 
      covar[1] = r->covar[phipar][Epar]/gain; 
  //    std::cout << r->covar[Epar][Epar] << std::endl; 
      covar[3] = r->covar[Epar][Epar]/gain/gain; 
      covar[2] = covar[1]; 
      entries.push_back(entry);
      covar_store.push_back(covar);
    }




    printf("npass: %d, npass ok: %d \n",npass, (int) entries.size()); 
    hypotests[i]->train(entries.size(), &entries[0]);  


    for (unsigned j = 0; j < covar_store.size(); j++)
    {
      delete covar_store[j]; 
    }

    for (int x = 1; x <= hypo_fn->GetNbinsX(); x++)
    {
      for (int y = 1; y <= hypo_fn->GetNbinsY(); y++)
      {

        double E = hypo_fn->GetXaxis()->GetBinCenter(x); 
        double phi = hypo_fn->GetYaxis()->GetBinCenter(y);  
        hypo_fn->SetBinContent(x,y, hypotests[i]->f(phi,&E)); 
      }
    }

    hypo_fn->Write(); 

    normalization_fn->SetParameters(hypotests[i]->getNormalizationPars());
    normalization_fn->SetParErrors(hypotests[i]->getNormalizationParErrs());
    normalization_fn->Write(); 
    ht_fn->SetParameters(hypotests[i]->getHTPars());
    ht_fn->SetParErrors(hypotests[i]->getHTParErrs());
    ht_fn->Write(); 
    width_fn->SetParameters(hypotests[i]->getWidthPars());
    width_fn->SetParErrors(hypotests[i]->getWidthParErrs());
    width_fn->Write(); 
    dir->Save(); 
  }


  out.cd(); 


  printf("calculating cuts...\n"); fflush(stdout); 
  int nsig = signal->Draw("Entry$",cut,"goff"); 


  std::vector<dmtpc::math::ParametricVonMisesHTTest::Entry>entries; 
  entries.reserve(nsig); 
  std::vector<double*>covar_store;
  std::vector<double>E_store;
  std::vector<short> runs;
  std::vector<short> events;


  int run, event; 

  signal->SetBranchAddress("result",&r); 
  signal->SetBranchAddress("run",&run); 
  signal->SetBranchAddress("event",&event); 

  signal->GetEntry(0); 

  for (int i = 0; i < nsig; i++)
  {

    if (!r->covar[phipar][phipar] || ! r->covar[Epar][Epar]) 
    {
      continue;
    }

    dmtpc::math::ParametricVonMisesHTTest::Entry entry; 
    signal->GetEntry((int) signal->GetV1()[i]); 

    entry.phi = r->htphi.val; 
    entry.phi_ht = r->prob2; 
    E_store.push_back(r->Ephys.val); 
    entry.X = &(E_store[i]); 
    double * covar = new double[4]; 
    covar[0] = r->covar[phipar][phipar]; 
    covar[1] = r->covar[phipar][Epar]/r->gain; 
    covar[3] = r->covar[Epar][Epar]/r->gain/r->gain; 
    covar[2] = covar[1]; 
    entry.covar = covar; 
    covar_store.push_back(covar); 
 
    runs.push_back(run); 
    events.push_back(event); 

    entries.push_back(entry);


//    printf("%d\n",i); 
  }

  dmtpc::math::ParametricVonMisesHTTest::Entry * sig_entries = new dmtpc::math::ParametricVonMisesHTTest::Entry[max_n]; 
  short *  sig_run = new short[max_n]; 
  short *  sig_event = new short[max_n]; 
  double *  sig_phi = new double[max_n]; 
  double *  sig_phi_err = new double[max_n]; 
  double *  sig_E = new double[max_n]; 
  double *  sig_E_err = new double[max_n]; 
  double *  sig_phi_E_cov = new double[max_n]; 
  double *  sig_prob = new double[max_n]; 
  std::vector<double*> val_fs(hypos.size()); 
  std::vector<double*> val_d2fs(hypos.size()); 

  for (unsigned h = 0; h < hypos.size(); h++)
  {
     val_fs[h] = new double[max_n]; 
     val_d2fs[h] = new double[max_n]; 
     tree->Branch(TString::Format("%s_f",hypos[h]->GetTitle()),val_fs[h],TString::Format("%s_f[n]/D",hypos[h]->GetTitle())); 
     tree->Branch(TString::Format("%s_d2f",hypos[h]->GetTitle()),val_d2fs[h],TString::Format("%s_d2f[n]/D",hypos[h]->GetTitle())); 
  }

  tree->Branch("sig_run",sig_run,"sig_run[n]/S"); 
  tree->Branch("sig_event",sig_event,"sig_event[n]/S"); 
  tree->Branch("sig_phi",sig_phi,"sig_phi[n]/D"); 
  tree->Branch("sig_phi_err",sig_phi_err,"sig_phi_err[n]/D"); 
  tree->Branch("sig_E",sig_E,"sig_E[n]/D"); 
  tree->Branch("sig_E_err",sig_E_err,"sig_E_err[n]/D"); 
  tree->Branch("sig_phi_E_cov",sig_phi_E_cov,"sig_phi_E_cov[n]/D"); 
  tree->Branch("sig_prob",sig_prob,"sig_prob[n]/D"); 

  for (n = min_n ; n <= max_n; n+=n_step)
  {

    printf("n = %d\n",n); 
    for (int i = 0; i < ntrials; i++)
    {
      sample_dist->Reset(); 
      printf("trial = %d\n",i); 

      std::set<unsigned> signal_idx; 
      int sig_i = 0;
      while (signal_idx.size() < unsigned(n)) 
      {
        int r = rand.Integer(entries.size());
        signal_idx.insert(r); 
      }

      for (std::set<unsigned>::iterator it = signal_idx.begin(); it!= signal_idx.end(); it++)
      {
        

        sig_entries[sig_i] = entries[*it]; 
        sig_event[sig_i] = events[*it]; 
        sig_phi[sig_i] = entries[*it].phi; 
        sig_phi_err[sig_i] = sqrt(entries[*it].covar[0]); 
        sig_prob[sig_i] = entries[*it].phi_ht;
        sig_E[sig_i] = entries[*it].X[0]; 
        sig_E_err[sig_i] = sqrt(entries[*it].covar[2]); 
        sig_phi_E_cov[sig_i] = sqrt(entries[*it].covar[1]); 
        sig_run[sig_i++] =  runs[*it]; 
        
        sample_dist->Fill(entries[*it].X[0], dmtpc::math::angle::normalize(entries[*it].phi), entries[*it].phi_ht); 
        sample_dist->Fill(entries[*it].X[0], dmtpc::math::angle::normalize(entries[*it].phi+TMath::Pi()), 1.-entries[*it].phi_ht); 

      }

      for (unsigned h= 0; h < hypos.size(); h++)
      {

          likelihoods[h] = hypotests[h]->loglikelihood(n, sig_entries,0,val_fs[h], val_d2fs[h]); 

      }

      tree->Fill(); 

    }

    tree->AutoSave(); 
  }


  tree->Write(); 
  out.Close(); 

}

