#include "Driver.hh" 
#include "TTree.h"
#include "Process.hh" 
#include "Readout.hh" 
#include "Generator.hh" 


void dmtpc::mc::mctpc::Driver::run(int n)
{

  //generate cumulative sums 
  std::vector<double> cumsums(generator_weights.size()) ; 

  double last = 0; 
  for (unsigned g = 0; g < cumsums.size(); g++)
  {
    last += generator_weights[g]; 
    cumsums[g] = last; 
  }

    
  dmtpc::analysis::MCTruth truth;

  //figure out  which processes go to the same readout
  

  std::vector<Readout*> readouts; 
  std::vector<Process*> processes_with_readouts; 
  std::vector<std::vector<Process*> > process_for_readouts; 

  for (unsigned i = 0; i < readout_pairs.size(); i++)
  {
    Readout  * r = readout_pairs[i].second; 
    Process  * p = readout_pairs[i].first; 

    bool found = false; 
    for (unsigned j = 0; j < readouts.size(); j++)
    {
      if (readouts[j] == r)
      {
        process_for_readouts[j].push_back(p); 
        found = true;
        break; 
      }

    }

    if (!found)
    {

      readouts.push_back(r); 
      process_for_readouts.push_back(std::vector<Process*>()); 
      process_for_readouts[process_for_readouts.size()-1].push_back(p); 

    }

    found = false; 
    for (unsigned j = 0; j < processes_with_readouts.size(); j++)
    {
      if (processes_with_readouts[j] == p)
      {
        found = true; 
        break; 
      }
    }

    if (!found)
      processes_with_readouts.push_back(p);
  }


  for (int i = 0; i < n; i++)
  {
    truth.clear(); 
    
    //pick a generator

    unsigned g = 0; 


    if (gen_pairs.size() > 1) 
    {
      double gpicker =  rand.Uniform() * sum_weights; 

      for (g = 0; g < cumsums.size(); g++)
      {
        if (gpicker <= cumsums[g]) 
          break; 
      }
    }
    
    gen_pairs[g].first->generate(&truth); 


    unsigned ngenerated = gen_pairs[g].first->getParticles()->size(); 
    unsigned npartitions = max_generated < 0 ? 1 :  1 + ( gen_pairs[g].first->getParticles()->size() - 1) / max_generated; 
    unsigned last_size = npartitions == 1 ? ngenerated : ngenerated % max_generated; 

    for (unsigned j =0; j < npartitions; j++)
    {

      unsigned this_size = j == npartitions -1 ? last_size : max_generated; 
      unsigned start = j * max_generated; 
      const Particle * ptr = & (gen_pairs[g].first->getParticles()->operator[](start)); 

      gen_pairs[g].second->process(this_size, ptr); 

      for (unsigned p = 0; p < process_pairs.size(); p++)
      {
         process_pairs[p].second->process(process_pairs[p].first->getParticles()->size(), & ( process_pairs[p].first->getParticles()->operator[](0))); 
      }
    }


    for (unsigned p = 0; p < processes_with_readouts.size(); p++)
    {
      processes_with_readouts[p]->prepareSignals(&truth); 
    }

    for (unsigned r = 0; r < readouts.size(); r++)
    {
       std::vector<Signal> all_signals;  
       for (unsigned j = 0; j < process_for_readouts[r].size(); j++)
       {
         const std::vector<Signal>* sigs = process_for_readouts[r][j]->getSignals(); 
         all_signals.insert(all_signals.end(), sigs->begin(), sigs->end()); 
       }

       readouts[r]->read(&all_signals, &truth); 
    }


    for (unsigned p = 0; p < processes_with_readouts.size(); p++)
    {
      processes_with_readouts[p]->clearSignals(); 
    }

  }

}



