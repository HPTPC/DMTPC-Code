#include "DecayChain.hh"
#include <fstream>
#include <istream>
#include <sstream>
#include <iostream>
#include "TRandom3.h"
#include <cmath>
#include <stdlib.h>

ClassImp(DecayChain); 

dmtpc::physics::DecayChain::~DecayChain()
{
  if (!init) return; 
  delete total_alpha; 
  delete total_beta_minus; 
  delete total_beta_plus; 
  abundances.clear(); 
}

dmtpc::physics::DecayChain::DecayChain(const char * file, double maxtime, double binsize, double dt)
{
  this->maxtime = maxtime; 
  this->binsize = binsize; 
  this->dt = dt; 
  init = false; 

  ifstream in(file);  
  
  if (!in.is_open()){
    cerr <<"File: "<<file<<" not found!"<<endl;
    return; 
  }

  string line; 

  bool inbraces = false; 

  vector<double> probs; 
  vector<DecayType> types; 
  vector<double> Qs; 
  vector<const char *> ids; 

  string name; 
  double weight; 
  double halflife; 

  string id; 
  double Q; 
  string type; 
  double prob; 

  while (!in.eof()) 
   {
     getline(in,line); 
     if (line[0] =='#' || line == "") continue; 
     if (line[0] == '{') 
     {
        inbraces = true; 
        probs.clear(); 
        types.clear(); 
        Qs.clear(); 
        for (unsigned i = 0; i < ids.size(); i++) free((char *) ids[i]); 
        ids.clear(); 
        continue; 
     }

     if (line[0]=='}')
     {
       inbraces = false; 
       addIsotope(name.c_str(), weight, halflife, probs.size(), &probs[0], &ids[0], &Qs[0], &types[0]); 
       continue; 
     }

     istringstream linestr(line); 
     if (inbraces) 
     {
        linestr >> id;  
        linestr >> prob;
        linestr >> Q; 
        linestr >> type;
          
        probs.push_back(prob); 
        Qs.push_back(Q); 
        char * cstr = strdup(id.c_str()); 
        ids.push_back(cstr); 
        
        if (!strcasecmp(type.c_str(),"alpha") || !strcasecmp(type.c_str(),"a"))
        {
          types.push_back(ALPHA); 
        }
        else if (!strcasecmp(type.c_str(),"beta_plus") || 
                 !strcasecmp(type.c_str(),"e+") || 
                 !strcasecmp(type.c_str(),"positron") || 
                 !strcasecmp(type.c_str(),"B+"))
        {
          types.push_back(BETA_PLUS); 
        }
        else if (!strcasecmp(type.c_str(),"beta_minus") || 
                 !strcasecmp(type.c_str(),"e-") || 
                 !strcasecmp(type.c_str(),"B-") || 
                 !strcasecmp(type.c_str(),"e") || 
                 !strcasecmp(type.c_str(),"electron") || 
                 !strcasecmp(type.c_str(),"B"))
        {
          types.push_back(BETA_MINUS); 
        }
     }
     else
     {
        linestr >> name >> halflife >> weight;
     }
  }

  compute(); 
}

void dmtpc::physics::DecayChain::addIsotope(const char * name, double initial_weight, double halflife, int nproducts, const double * product_probabilities, const char * const * product_names, const double * decay_Q,  const DecayType * decay_type) 
{
  if (isotope_ids.count(string(name)) > 0)
  {
    cerr << "Chain already contains " << name << " . Skipping. " << endl; 
    return; 
  }

  if (init)
  {
    cerr << "Chain already computed. Please start a new chain!" << endl; 
    return; 
  }
  isotope_ids[string(name)] = isotopes.size();  
  isotopes.push_back(string(name));
  halflives.push_back(halflife); 
  initial.push_back(initial_weight);

  vector<string> ids(nproducts); 
  vector<double> probs(nproducts); 
  vector<DecayType> types(nproducts); 
  vector<double> Qs(nproducts); 

  for (int i = 0; i < nproducts;i++)
  {
    ids[i] = string(product_names[i]);
    probs[i] = product_probabilities[i];
    types[i] = decay_type[i]; 
    Qs[i] = decay_Q[i]; 

  }

  this->product_names.push_back(ids); 
  this->product_probabilities.push_back(probs);  
  product_types.push_back(types);  
  product_Q.push_back(Qs); 

  total_abundance += initial_weight; 

  cout << "Added " << name << std::endl; 
}


void dmtpc::physics::DecayChain::compute()
{

  //Collapse transient isotopes into parents
  
  bool reset = false; 
  for (unsigned i = 0; i < isotopes.size(); i++)
  {
    if (reset) 
    {
      i = 0; 
      reset = false; 
    }

    for (unsigned j = 0; j < product_names[i].size(); j++)
    {

      if (!isotope_ids.count(string(product_names[i][j]))) 
      {
        cout << "Didn't find " << string(product_names[i][j]) << "?" << endl; 
        continue; 
      }
      int id = isotope_ids[product_names[i][j]];  

      //Transient detected! 
      if (halflives[id] > 0 &&  halflives[id] < dt)
      {
        cout << "Transient Detected! " << isotopes[id] << " decays moved to " << isotopes[i] << endl; 

        double p = product_probabilities[i][j]; 
        product_names[i].erase(product_names[i].begin() + j); 
        product_probabilities[i].erase(product_probabilities[i].begin() + j); 
        product_types[i].erase(product_types[i].begin() + j); 
        product_Q[i].erase(product_Q[i].begin() + j); 

        for (unsigned k = 0; k < product_names[id].size(); k++)
        {
          product_names[i].push_back(product_names[id][k]); 
          product_probabilities[i].push_back(product_probabilities[id][k] * p); 
          product_types[i].push_back(product_types[id][k]); 
          product_Q[i].push_back(product_Q[id][k]); 
        }

        reset = true;  
        break; 
      }
    }
  }


  //Convert product names to ids for speed 
  product_ids.clear(); 
  for (unsigned i = 0; i < isotopes.size(); i++)
  {
    vector<int> ids; 
    for (unsigned j = 0; j < product_names[i].size(); j++)
    {
      cout << isotopes[i] << " --> " << product_names[i][j] << std::endl; 
      if (isotope_ids.count(string(product_names[i][j])))
        ids.push_back(isotope_ids[product_names[i][j]]); 
      else
        ids.push_back(-1); 
    }
    product_ids.push_back(ids); 
  }

  
  vector<double> x(initial); 
  vector<double> dx(initial.size()); 
  double ln2 = log(2); 

  int nbins = (int) (maxtime/binsize + 0.5) ; 
  abundances.clear(); 
  for (unsigned i = 0; i < isotopes.size(); i++)
  {
    TString name (TString::Format("%s_abundance",isotopes[i].c_str())); 
    abundances.push_back(new TH1D(name,name,nbins, 0, maxtime)); 
  }

  total_alpha = new TH1D("total_alpha_rate","total_alpha_rate",nbins,0,maxtime); 
  total_beta_minus = new TH1D("total_beta_minus","total_beta_minus",nbins,0,maxtime); 
  total_beta_plus = new TH1D("total_beta_plus","total_beta_plus",nbins,0,maxtime); 

  int bin = 0; 
  for (double t = 0; t <= maxtime; t+=dt)
  {
    if (t / binsize >= bin)
    {

      double alpha_rate = 0; 
      double beta_minus_rate = 0; 
      double beta_plus_rate = 0; 

      for (unsigned i = 0; i < isotopes.size(); i++)
      {
        abundances[i]->SetBinContent(bin,x[i]); 

        for (unsigned j = 0; j < product_ids[i].size(); j++)
        {
          if (product_types[i][j] == ALPHA) 
          {
            alpha_rate += x[i]*product_probabilities[i][j] / halflives[i];  
          }
          else if (product_types[i][j] == BETA_MINUS) 
          {
            beta_minus_rate += x[i]*product_probabilities[i][j] / halflives[i];  
          }
          else if (product_types[i][j] == BETA_PLUS) 
          {
            beta_plus_rate += x[i]*product_probabilities[i][j] / halflives[i];  
          }
        }
      }

      total_alpha->SetBinContent(bin,alpha_rate); 
      total_beta_minus->SetBinContent(bin,beta_minus_rate); 
      total_beta_plus->SetBinContent(bin,beta_plus_rate); 
      bin++; 
    }

    for (unsigned i = 0; i < isotopes.size(); i++)
    {
      dx[i] = 0; 
    }

    for (unsigned i = 0; i < isotopes.size(); i++)
    {
      if (x[i] == 0) continue; 
      if (halflives[i] < 0) continue; //Stable!
      double this_dx; 
      this_dx = x[i] * ln2 * dt/ halflives[i]; 
      dx[i] -= this_dx; 

      for (unsigned j = 0; j < product_ids[i].size(); j++)
      {
          int target = product_ids[i][j];
          if (target >= 0)
          {
             dx[target] += this_dx * product_probabilities[i][j]; 
          }
      }

    }

    for (unsigned i = 0; i < isotopes.size(); i++)
    {
      x[i] += dx[i]; 
    }

  }

  init = true; 
}


TGraph dmtpc::physics::DecayChain::getEnergyProbabilitiesTGraph(double t, DecayType type) const 
{

  if (!init)
  {
    cerr << "Decay Chain not computed yet. Call compute() first." << endl; 
    return TGraph(0); 
  }

  vector<pair<double,double> > pairs = getEnergyProbabilities(t,type); 
  TGraph ret (pairs.size()); 

  for (unsigned i = 0; i < pairs.size() ; i++)
  {
    ret.SetPoint(i,pairs[i].first, pairs[i].second); 
  }

  return ret; 
}

vector<pair<double,double> > dmtpc::physics::DecayChain::getEnergyProbabilities(double t, DecayType type) const
{

  vector<pair<double,double> >  ret;
  if (!init)
  {
    cerr << "Decay Chain not computed yet. Call compute() first." << endl; 
    return ret; 
  }



  double total; 
  switch(type)
  {
    case ALPHA: total = total_alpha->Interpolate(t); break; 
    case BETA_MINUS: total = total_beta_minus->Interpolate(t); break; 
    case BETA_PLUS:  total = total_beta_plus->Interpolate(t); break; 
    default: cerr << "Bad type! " << endl; return ret; 
  }

  for (unsigned i = 0; i < isotopes.size(); i++)
  {
    double x = abundances[i]->Interpolate(t); 
    if (x == 0) continue; 
    for (unsigned j = 0; j < product_ids[i].size(); j++)
    {
      if (type == product_types[i][j])
      {
        double p = x * product_probabilities[i][j] / halflives[i]; 
        double E = product_Q[i][j]; 
        pair<double,double> pr(E,p/total); 
        ret.push_back(pr); 
      }
    }
  }

  return ret; 
}

double dmtpc::physics::DecayChain::getRandomEnergy(double t, DecayType type) const 
{
  static TRandom3 rand; 
  if (!init)
  {
    cerr << "Decay Chain not computed yet. Call compute() first." << endl; 
    return 0; 
  }


  double total; 
  switch(type)
  {
    case ALPHA: total =  total_alpha->Interpolate(t) ; break; 
    case BETA_MINUS: total = total_beta_minus->Interpolate(t); break; 
    case BETA_PLUS: total =  total_beta_plus->Interpolate(t); break; 
    default: return 0; 
  }

  double p = rand.Rndm() * total;

  double current = 0; 

  for (unsigned i = 0; i < isotopes.size(); i++)
  {
    double x = abundances[i]->Interpolate(t) ; 
    if (x ==0) continue; 
    for (unsigned j = 0; j < product_ids[i].size(); j++)
    {
      if (type == product_types[i][j])
      {
        current += x * product_probabilities[i][j] / halflives[i]; 
        if (current > p)
        {
          return product_Q[i][j]; 
        }
      }
    }
  }

  cerr << "No energy generated. Probably a bug :( " << endl; 
  return 0; 
}

double dmtpc::physics::DecayChain::getAbundance(const char * isotope, double t) const
{
  const TH1D * h = getAbundance(isotope); 
  if (h == 0) return 0; 
  return ((TH1D*)h)->Interpolate(t);
}


const TH1D * dmtpc::physics::DecayChain::getAbundance(const char * isotope) const
{
  if (!init)
  {
    cerr << "Decay Chain not computed yet. Call compute() first." << endl; 
    return 0; 
  }
  string iso(isotope);  
  return isotope_ids.count(iso) ? abundances[(*(isotope_ids.find(iso))).second] : 0; 
}

