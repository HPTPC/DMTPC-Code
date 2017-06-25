#ifndef DMTPC_MC_MCTPC_READOUT_HH
#define DMTPC_MC_MCTPC_READOUT_HH

#include <vector>
#include <utility>
#include "Signal.hh" 

namespace dmtpc
{
  namespace analysis
  {
    class MCTruth;
  }

  namespace mc
  {
    namespace mctpc
    {
      class Readout
      {
        public:
	  virtual void read(const std::vector<dmtpc::mc::mctpc::Signal>* in, dmtpc::analysis::MCTruth * truth) = 0; 
          virtual ~Readout() {; }
      }; 
    }
  }
}


#endif
