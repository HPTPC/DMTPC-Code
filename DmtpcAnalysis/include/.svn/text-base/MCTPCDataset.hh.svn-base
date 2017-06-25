#ifndef DMTPC_ANALYSIS_MCTPC_DATASET_HH
#define DMTPC_ANALYSIS_MCTPC_DATASET_HH

#include "MCTruth.hh" 

class TTree; 
class TFile; 
class TH2; 

namespace dmtpc
{

  namespace analysis
  {
      class MCTPCDataset
      {

        public: 
          MCTPCDataset(const char * file = 0); 
          virtual ~MCTPCDataset(); 

          bool open( const char * file); 
          void close(); 
          MCTruth * getEvent(int i); 
          MCTruth * truth() { return truthptr; } 
          TTree * getTree() { return tree; } 

          TH2 * getTruthImage(int i); 




        private: 
          MCTruth * truthptr; 
          TTree * tree; 
          TFile * f; 

      }; 
  }
}

    
     
#endif

