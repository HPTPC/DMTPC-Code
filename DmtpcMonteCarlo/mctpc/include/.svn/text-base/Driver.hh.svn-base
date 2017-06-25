#ifndef DMTPC_MC_MCTPC_DRIVER_HH
#define DMTPC_MC_MCTPC_DRIVER_HH

#include <vector>
#include <utility>
#include "TFile.h" 
#include "TRandom3.h" 

class TTree; 

namespace dmtpc
{
  namespace mc
  {
    namespace mctpc
    {

      class Generator;
      class Process;
      class Readout; 

      class Driver
      {
        public: 

          /* maxgen controls how many particles from generators will be processed at a time. 
           * negative is unlimited, may want to limit if RAM use becomes a problem */ 
          Driver(int maxgen = -1, long seed = 0) : rand(seed) 
          { max_generated = maxgen; } 

          void plug(Generator * in, Process * out, double weight = 1)  
          { 
            gen_pairs.push_back(std::pair<Generator*,Process*>( in,out)); 
            generator_weights.push_back(weight);
            sum_weights += weight;
          }

          void plug(Process * in, Process * out ) 
          { process_pairs.push_back(std::pair<Process*,Process*>( in,out)); }
          void plug( Process *in, Readout * out ) 
          { readout_pairs.push_back(std::pair<Process*,Readout*> (in,out)); }

          void run(int n); 
        
        private:

          int max_generated; 
          std::vector<double> generator_weights; 
          double sum_weights; 
          std::vector<std::pair<Generator*,Process*> > gen_pairs; 
          std::vector<std::pair<Process*,Process*> > process_pairs; 
          std::vector<std::pair<Process*, Readout*> > readout_pairs; 
          TRandom3 rand; 

      };

    }
  }
}


#endif
