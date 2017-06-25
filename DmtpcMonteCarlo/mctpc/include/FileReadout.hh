#ifndef DMTPC_MC_MCTPC_FILE_READOUT_HH
#define DMTPC_MC_MCTPC_FILE_READOUT_HH

#include "Readout.hh" 
#include "TFile.h" 

namespace dmtpc
{
  namespace mc
  {
    namespace mctpc
    {
        
      class FileReadout : public Readout
      {
        public:
          FileReadout(const char * out, const char * filter = 0); 
          
	virtual void read(const std::vector<dmtpc::mc::mctpc::Signal>* in, dmtpc::analysis::MCTruth * truth); 

        private:
          const char * filter; 
          TFile f; 
          int counter; 

      }; 
    }
  }
}

#endif
