#include <cstdio>

#include "TrackFitter.hh"
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

char * Eerr_fn_str = " (Ephys.err < 2) * 3 + (Ephys.err > 7) * 5.5 + (Ephys.err <= 7 && Ephys.err >= 2) * ( 3 + (Ephys.err -2 ) * 2.5/5)" ; 

static double phierr_fn(double E, double phierr)
{

  double Eprediction = 21.88 / E; 

  if (phierr > 0.55) phierr = 0.55; 

  double phiPrediction = 2.36 * pow(phierr+ 0.0072,0.896); 

  return Eprediction/2 + phiPrediction/2; 

}

char * phierr_fn_str = "TMath::Min(TMath::Pi()/2, 0.5 * ( 21.88/Ephys.val) + 0.5 * ( 2.36 * (TMath::Min(htphi.err,0.55) + 0.0072)^0.896))"; 


int main(int nargs, char **args)
{

//  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);

  if (nargs < 4)
  {
    fprintf(stderr,"Usage: FitEnergyTest out.root filesSignal.txt filesHypothesis1.txt filesHypothesis2.txt... \n" ); 
    return 1; 
  }
  
  char * env_ntrials = getenv("ENERGY_NTRIALS"); 
  int ntrials = env_ntrials ? atoi(env_ntrials) : 200; 

  char * env_min_n = getenv("ENERGY_MIN_N"); 
  int min_n = env_min_n ? atoi(env_min_n) : 10; 
  char * env_max_n = getenv("ENERGY_MAX_N"); 
  int max_n = env_max_n ? atoi(env_max_n) : 400; 
  char * env_n_step = getenv("ENERGY_N_STEP"); 
  int n_step = env_n_step ? atoi(env_n_step) : 10; 

  char * env_min_Emin = getenv("ENERGY_MIN_EMIN"); 
  int min_Emin = env_min_Emin ? atoi(env_min_Emin) : 20; 
  char * env_max_Emin = getenv("ENERGY_MAX_EMIN"); 
  int max_Emin = env_max_Emin ? atoi(env_max_Emin) : 60; 
  char * env_Emin_step = getenv("ENERGY_EMIN_STEP"); 
  int Emin_step = env_Emin_step ? atoi(env_Emin_step) : 20; 

  char * env_use_ht = getenv("ENERGY_USE_HT"); 
  bool use_ht = env_use_ht ? atoi(env_use_ht) : true; 
 
  char * env_seed = getenv("ENERGY_SEED"); 
  long seed = env_seed ? atol(env_seed) : 0; 


  char * cutstring = getenv("ENERGY_CUT"); 
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



  if (!use_ht)
  {
    signal->Draw("htphi.val  : Ephys.val>> signal_dist",cutstring,"goff"); 
  }
  else
  {
    signal->Draw(" dmtpc::math::angle::normalize(htphi.val) : Ephys.val>> signal_dist",TString::Format("prob2*(%s)",cutstring),"goff"); 
    signal->Draw(" dmtpc::math::angle::normalize(htphi.val  + pi): Ephys.val>> +signal_dist",TString::Format("(1.-prob2)*(%s)",cutstring),"goff"); 
  }

  out.cd(); 
  signal_hist->Write(); 

  printf("Entries in signal: %d\n", (int) signal->GetEntries()); 


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

    c->Draw(TString::Format("dmtpc::math::angle::normalize(htphi.val) : Ephys.val >> %s_dist",c->GetTitle()),TString::Format("prob2*(%s)",cutstring),"goff"); 
    c->Draw(TString::Format("dmtpc::math::angle::normalize(htphi.val + pi)  : Ephys.val >> +%s_dist",c->GetTitle()),TString::Format("(1-prob2)*(%s)",cutstring),"goff"); 

    out.cd(); 
    hypo_hist->Write(); 
    printf("Entries in %s: %d\n", c->GetTitle(), (int) c->GetEntries()); 
  }



  int n; 
  double Emin; 
  


  std::vector<double> scores(hypos.size()); 
  std::vector<double> aa(hypos.size()); 
  std::vector<double> bb(hypos.size()); 
  std::vector<double> ab(hypos.size()); 
  std::vector<double> ba(hypos.size()); 

  double rayleigh_score; 
  double rayleigh_axial_score; 

  TTree * tree = new TTree("Energy","Energy Distance Test");  
  tree->Branch("n",&n); 
  tree->Branch("Emin",&Emin); 

  TH2* sample_dist = new TH2F("sample_dist","Distribution for Sample", 
                              hist_nEbins,hist_Emin, hist_Emax, 
                              hist_nphibins, hist_phimin, hist_phimax); 

  tree->Branch("sample_dist",&sample_dist); 
  tree->Branch("rayleigh_score",&rayleigh_score); 
  tree->Branch("rayleigh_axial_score",&rayleigh_axial_score); 

  for (unsigned i = 0; i < hypos.size(); i++)
  {
    tree->Branch(TString::Format("energy_score_%s", hypos[i]->GetTitle()), &(scores[i])); 
//    tree->Branch(TString::Format("watson_Phi_score_%s", hypos[i]->GetTitle()), &(scores[i])); 
//    tree->Branch(TString::Format("energy_aa_%s", hypos[i]->GetTitle()), &(aa[i])); 
//    tree->Branch(TString::Format("energy_bb_%s", hypos[i]->GetTitle()), &(bb[i])); 
//    tree->Branch(TString::Format("energy_ab_%s", hypos[i]->GetTitle()), &(ab[i])); 
 //   tree->Branch(TString::Format("energy_ba_%s", hypos[i]->GetTitle()), &(ba[i])); 
  }


  const char * xnames[2]; 
  xnames[0] = "Ephys.val"; 
  xnames[1] = "htphi.val"; 
  const char * xerrdnnames[2]; 
  xerrdnnames[0] = Eerr_fn_str; 
  xerrdnnames[1] = phierr_fn_str;
  const char * weight_names = use_ht ? "prob2" : "1" ; 

  const char * alt_xnames[2]; 
  alt_xnames[0] = "Ephys.val"; 
  alt_xnames[1] = "htphi.val +  pi"; 
  const char ** alt_xerrdnnames = xerrdnnames; 
  const char * alt_weight_names = "1-prob2"; 

  double * sig_E = new double [2*max_n]; 
  double * sig_EErrdn = new double [2*max_n]; 
  double * sig_EErrup = new double [2*max_n]; 
  double * sig_phi = new double [2*max_n]; 
  double * sig_phiErr = new double [2*max_n]; 
  double * sig_prob = new double [2*max_n]; 
  double * sig_x[2]; sig_x[0] = sig_E; sig_x[1] = sig_phi; 
  double * sig_xerrdn[2]; sig_xerrdn[0] = sig_EErrdn; sig_xerrdn[1] = sig_phiErr; 
  double * sig_xerrup[2]; sig_xerrup[0] = sig_EErrup; sig_xerrup[1] = sig_phiErr; 


  for (Emin = min_Emin; Emin <= max_Emin; Emin += Emin_step)
  {


    TCut cut = TCut (cutstring) && TCut(TString::Format("Ephys.val > %f",Emin)); 

    // Fill A vector of WIMP tests 
    std::vector<dmtpc::math::EnergyTest*> hypotests(hypos.size()); 

    for (unsigned i = 0; i < hypos.size(); i++)
    {
      if (Emin != min_Emin) delete hypotests[i]; 
      hypotests[i] = new dmtpc::math::EnergyTest(2,1); 
    }


    for (unsigned i = 0; i < hypos.size(); i++)
    {
      hypotests[i]->setAValues(hypos[i],xnames,xerrdnnames,0,weight_names,cut,false); 
      hypotests[i]->treatAsAngle(1,true); 
      hypotests[i]->setScale(0,10*(hist_Emax-hist_Emin)/TMath::Pi()); 
    }

    if (use_ht)
    {
      //append reversed version with opposite phi
      for (unsigned i = 0; i < hypos.size(); i++)
      {
        hypotests[i]->setAValues(hypos[i],alt_xnames,alt_xerrdnnames,0,alt_weight_names,cut,true); 
      }
    }



    int nsig = signal->Draw("Entry$",cut,"goff"); 


    std::vector<double>E(nsig); 
    std::vector<double>Eerr(nsig); 
    std::vector<double>phi(nsig); 
    std::vector<double> phiErr(nsig); 
    std::vector<double>prob(nsig); 


    dmtpc::analysis::TrackFitter::Result *r = new dmtpc::analysis::TrackFitter::Result;

    signal->SetBranchAddress("result",&r); 

    signal->GetEntry(0); 

    for (int i = 0; i < nsig; i++)
    {
      signal->GetEntry((int) signal->GetV1()[i]); 
      E[i] = r->Ephys.val; 
      Eerr[i] = Eerr_fn(r->Ephys.err); 
      phi[i] = r->htphi.val;
      phiErr[i] = phierr_fn(E[i], r->htphi.err); 
      prob[i] = use_ht ? r->prob2 : 1; 
  //    printf("%d\n",i); 
    }


    for (n = min_n ; n <= max_n; n+=n_step)
    {

      printf("n = %d\n",n); 
      printf("Eminn = %f\n",Emin); 
      for (int i = 0; i < ntrials; i++)
      {
        sample_dist->Reset(); 
        printf("trial = %d\n",i); 

        std::set<unsigned> signal_idx; 
        int sig_i = 0;
        while (signal_idx.size() < unsigned(n)) 
        {
          int r = rand.Integer(nsig);
          signal_idx.insert(r); 
        }

        for (std::set<unsigned>::iterator it = signal_idx.begin(); it!= signal_idx.end(); it++)
        {
          

          sig_E[sig_i] = E[*it]; 
          sig_EErrdn[sig_i] = Eerr[*it];
          sig_EErrup[sig_i] =  Eerr[*it];
          sig_phi[sig_i] = phi[*it]; 
          sig_phiErr[sig_i] = phiErr[*it];
          sig_prob[sig_i++] =  prob[*it]; 

          
          sample_dist->Fill(E[*it], phi[*it], prob[*it]); 

          if (use_ht)
          {
            sig_E[sig_i] = E[*it]; 
            sig_EErrdn[sig_i] = Eerr[*it];
            sig_EErrup[sig_i] =  Eerr[*it];
            sig_phi[sig_i] = dmtpc::math::angle::normalize(phi[*it] + TMath::Pi()); 
            sig_phiErr[sig_i] = phiErr[*it];
            sig_prob[sig_i++] =  1.-prob[*it];
            sample_dist->Fill(E[*it], TMath::Pi() + +phi[*it], 1.-prob[*it]); 
         }

//          printf("sig_i: %d\n",sig_i); 
        }

        rayleigh_score = dmtpc::math::angle::rayleigh(dmtpc::math::angle::resultant(use_ht ? 2*n : n,sig_phi, sig_prob),use_ht ? 2*n : n); 
        rayleigh_axial_score= dmtpc::math::angle::rayleigh(dmtpc::math::angle::resultant_axial(n,sig_phi),n); 

        for (unsigned j = 0; j < hypos.size(); j++)
        {
          hypotests[j]->setBValues(use_ht ? 2*n : n, sig_x, sig_xerrdn, sig_xerrup, sig_prob); 
          scores[j] = hypotests[j]->testStatistic(); 
          aa[j] = hypotests[j]->getAA(); 
          bb[j] = hypotests[j]->getBB(); 
          ab[j] = hypotests[j]->getAB(); 
          ba[j] = hypotests[j]->getBA(); 
          printf("score %s:%s%f%s\n",hypos[j]->GetTitle(), scores[j] < 0 ? red_text: blue_text, scores[j], normal_text ); 
        }

        tree->Fill(); 

      }

      tree->AutoSave(); 
    }

  }

  tree->Write(); 
  out.Close(); 

}

