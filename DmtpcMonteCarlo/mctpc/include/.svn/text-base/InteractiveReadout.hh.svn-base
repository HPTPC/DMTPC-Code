#ifndef DMTPC_MC_MCTPC_INTERACTIVE_READOUT_HH
#define DMTPC_MC_MCTPC_INTERACTIVE_READOUT_HH

#include "Readout.hh" 

namespace dmtpc
{
  namespace mc
  {
    namespace mctpc
    {
        
      class InteractiveReadout : public Readout
      {
        public:
          InteractiveReadout(const char * filter = 0, int w = 800, int h = 600); 
          virtual int read(const std::vector<dmtpc::mc::mctpc::Signal>* in); 

        private:
          const char * filter; 
          int w; 
          int h; 
          int id; 

      }; 
    }
  }
}

#endif
