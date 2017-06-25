#ifndef DMTPC_CORE_WRITABLE_DATASET_HH
#define DMTPC_CORE_WRITABLE_DATASET_HH
class TTimeStamp; 
class TFile;
#include "Dataset.hh" 


namespace dmtpc
{
  namespace core
  {
    class WritableDataset : public Dataset
    {

      public: 
        WritableDataset(const char * file, int ncam, TTimeStamp* start = 0, TTimeStamp * end = 0, const char * config = "",  bool overwrite = true); 
        virtual ~WritableDataset(); 
        int addBias(const Image ** biasframes, const Image ** os = 0); 
        int addEvent(const Event * ev); 
        void flush(); 
        TFile * getFile() { return file; } 
        void commit(); 
      private: 
        bool committed; 
    }; 
  }
}

#endif
