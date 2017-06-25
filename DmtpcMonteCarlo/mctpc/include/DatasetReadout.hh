#ifndef DMTPC_MC_MCTPC_DATASET_READOUT_HH
#define DMTPC_MC_MCTPC_DATASET_READOUT_HH

#include <vector>
#include "WritableDataset.hh" 
#include "WritableEvent.hh" 
#include "Readout.hh"
#include "TTimeStamp.h" 

#include "Signal.hh"
#include "TString.h" 
#include "TTree.h" 


namespace dmtpc
{
  namespace mc
  {
    namespace mctpc
    {
      class DatasetReadout : public Readout
      {

        public:

          DatasetReadout(const char * outdir = ".", TTimeStamp * start = 0, TTimeStamp * end = 0); 

	virtual void read(const std::vector<dmtpc::mc::mctpc::Signal>* in, dmtpc::analysis::MCTruth * truth);

          void defineCamera(const char * name, const char * biasname = ""); 

          virtual ~DatasetReadout(); 

        private:

          std::vector<const char *> cameras; 
          std::vector<const char *> camera_biases; 
          TTimeStamp *start; 
          TTimeStamp *end; 
          dmtpc::core::WritableDataset * d; 
          int file_index; 
          int event; 
          const char * dir;

          TTree * truth_tree; 
	  dmtpc::analysis::MCTruth * truthptr; 




      }; 
    }
  }
}


#endif
