#include <cstdio>

#include "TrackFitter.hh"
#include "TChain.h" 
#include "TMath.h"
#include "TFile.h" 
#include <fstream>
#include <istream>
#include "EnergyTest.hh" 
#include <set>
#include "TRandom3.h"
#include <fenv.h>



const char * red_text = "\e[31m"; 
const char * blue_text = "\e[34m"; 
const char * normal_text = "\e[0m"; 


int main(int nargs, char **args)
{

//  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);

  if (nargs < 4)
  {
    fprintf(stderr,"Usage: FitEnergyTest out.root filesSignal.txt filesHypothesis1.txt filesHypothesis2.txt... \n" ); 
    return 1; 
  }
  
  char * env_ntrials = getenv("ENERGY_NTRIALS"); 
  int ntrials = env_ntrials ? atoi(env_ntrials) : 100; 
  char * env_min_n = getenv("ENERGY_MIN_N"); 
  int min_n = env_min_n ? atoi(env_min_n) : 10; 
  char * env_max_n = getenv("ENERGY_MAX_N"); 
  int max_n = env_max_n ? atoi(env_max_n) : 400; 
  char * env_n_step = getenv("ENERGY_N_STEP"); 
  int n_step = env_n_step ? atoi(env_n_step) : 10; 
  char * env_seed = getenv("ENERGY_SEED"); 
  long seed = env_seed ? atol(env_seed) : 0; 

  char * cut = getenv("ENERGY_CUT"); 
  if (!cut) cut = "Ephys.val > 0 && chisq/ndof < 2 && abs(x0.val - mcX) < 30 && abs(y0.val - mcY) < 30"; 



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
    printf("Entries in %s: %d\n", c->GetTitle(), (int) c->GetEntries()); 
  }


  //Create Output Tree
  TFile out(args[1],"RECREATE"); 

  int n; 
  std::vector<double> scores(hypos.size()); 
  std::vector<double> aa(hypos.size()); 
  std::vector<double> bb(hypos.size()); 
  std::vector<double> ab(hypos.size()); 
  std::vector<double> ba(hypos.size()); 


  TTree * tree = new TTree("Energy","Energy Distance Test");  
  tree->Branch("n",&n); 

  for (unsigned i = 0; i < hypos.size(); i++)
  {
    tree->Branch(TString::Format("score_%s", hypos[i]->GetTitle()), &(scores[i])); 
    tree->Branch(TString::Format("aa_%s", hypos[i]->GetTitle()), &(aa[i])); 
    tree->Branch(TString::Format("bb_%s", hypos[i]->GetTitle()), &(bb[i])); 
    tree->Branch(TString::Format("ab_%s", hypos[i]->GetTitle()), &(ab[i])); 
    tree->Branch(TString::Format("ba_%s", hypos[i]->GetTitle()), &(ab[i])); 
  }


  // Fill A vector of WIMP tests 
  std::vector<dmtpc::math::EnergyTest*> hypotests(hypos.size()); 

  for (unsigned i = 0; i < hypos.size(); i++) hypotests[i] = new dmtpc::math::EnergyTest(2,1); 



  const char * xnames[2]; 
  xnames[0] = "Ephys.val"; 
  xnames[1] = "htphi.val"; 
  const char * xerrupnames[2]; 
  xerrupnames[0] = "10./sqrt(Ephys.val/100)"; 
  xerrupnames[1] = "0.1/sqrt(Ephys.val/100)"; 
  const char * xerrdnnames[2]; 
  xerrdnnames[0] = "10./sqrt(Ephys.val/100)"; 
  xerrdnnames[1] = "0.1/sqrt(Ephys.val/100)"; 
  const char * weight_names ="prob2"; 

  const char * alt_xnames[2]; 
  alt_xnames[0] = "Ephys.val"; 
  alt_xnames[1] = "htphi.val +  pi"; 
  const char * alt_xerrupnames[2]; 
  alt_xerrupnames[0] = "10./sqrt(Ephys.val/100)"; 
  alt_xerrupnames[1] = "0.1/sqrt(Ephys.val/100)"; 
  const char * alt_xerrdnnames[2]; 
  alt_xerrdnnames[0] = "10./sqrt(Ephys.val/100)"; 
  alt_xerrdnnames[1] = "0.1/sqrt(Ephys.val/100)"; 
  const char * alt_weight_names = "1-prob2"; 



  for (unsigned i = 0; i < hypos.size(); i++)
  {
    hypotests[i]->setAValues(hypos[i],xnames,xerrdnnames,xerrupnames,weight_names,cut,false); 
    hypotests[i]->treatAsAngle(1,true); 
//    hypotests[i]->setScale(0,100); 
  }

  //append reversed version with opposite phi
  for (unsigned i = 0; i < hypos.size(); i++)
  {
    hypotests[i]->setAValues(hypos[i],alt_xnames,alt_xerrdnnames,alt_xerrupnames,alt_weight_names,cut,true); 
  }



  int nsig = signal->Draw("Entry$",cut,"goff"); 


  double* E = new double[nsig]; 
  double* Eerr = new double[nsig]; 
  double* phi= new double[nsig]; 
  double* phiErr= new double[nsig]; 
  double* prob= new double[nsig]; 


  double * sig_E = new double [2*max_n]; 
  double * sig_EErrdn = new double [2*max_n]; 
  double * sig_EErrup = new double [2*max_n]; 
  double * sig_phi = new double [2*max_n]; 
  double * sig_phiErr = new double [2*max_n]; 
  double * sig_prob = new double [2*max_n]; 
  double * sig_x[2]; sig_x[0] = sig_E; sig_x[1] = sig_phi; 
  double * sig_xerrdn[2]; sig_xerrdn[0] = sig_EErrdn; sig_xerrdn[1] = sig_phiErr; 
  double * sig_xerrup[2]; sig_xerrup[0] = sig_EErrup; sig_xerrup[1] = sig_phiErr; 

  dmtpc::analysis::TrackFitter::Result *r = new dmtpc::analysis::TrackFitter::Result;

  signal->SetBranchAddress("result",&r); 

  signal->GetEntry(0); 

  for (int i = 0; i < nsig; i++)
  {
    signal->GetEntry((int) signal->GetV1()[i]); 
    E[i] = r->Ephys.val; 
    Eerr[i] = 10/sqrt(E[i]/100); 
    phi[i] = r->htphi.val;
    phiErr[i] = 0.1/sqrt(E[i]/100); 
    prob[i] = r->prob; 
//    printf("%d\n",i); 
  }


  for (n = min_n ; n <= max_n; n+=n_step)
  {

    printf("n = %d\n",n); 
    for (int i = 0; i < ntrials; i++)
    {
      printf("trial = %d\n",i); 

      std::set<int> signal_idx; 
      int sig_i = 0;
      while (signal_idx.size() < unsigned(n)) 
      {
        int r = rand.Integer(nsig);
        signal_idx.insert((int) signal->GetV1()[r]); 
      }

      for (std::set<int>::iterator it = signal_idx.begin(); it!= signal_idx.end(); ++it)
      {
        
        sig_E[sig_i] = E[*it]; 
        sig_EErrdn[sig_i] = Eerr[*it];
        sig_EErrup[sig_i] =  Eerr[*it];
        sig_phi[sig_i] = phi[*it]; 
        sig_phiErr[sig_i] = phiErr[*it];
        sig_prob[sig_i++] =  prob[*it]; 

        sig_E[sig_i] = E[*it]; 
        sig_EErrdn[sig_i] = Eerr[*it];
        sig_EErrup[sig_i] =  Eerr[*it];
        sig_phi[sig_i] = phi[*it]; 
        sig_phiErr[sig_i] = phiErr[*it];
        sig_prob[sig_i++] =  1-prob[*it]; 


      }

      for (unsigned j = 0; j < hypos.size(); j++)
      {
        hypotests[j]->setBValues(n, sig_x, sig_xerrdn, sig_xerrup, sig_prob); 
        scores[j] = hypotests[j]->testStatistic(); 
        aa[j] = hypotests[j]->getAA(); 
        bb[j] = hypotests[j]->getBB(); 
        ab[j] = hypotests[j]->getAB(); 
        ba[j] = hypotests[j]->getBA(); 
        printf("score %s:%s%f%s\n",hypos[j]->GetTitle(), scores[j] < 0 ? red_text: blue_text, scores[j], normal_text ); 
      }

      tree->Fill(); 


    }
  }

  tree->Write(); 
  out.Close(); 

}

