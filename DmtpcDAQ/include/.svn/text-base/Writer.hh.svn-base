#ifndef DMTPC_DAQ_WRITER_HH
#define DMTPC_DAQ_WRITER_HH
#include "TTimeStamp.h"
#include "TThread.h"

namespace dmtpc
{
  namespace core
  {
    class WritableDataset; 
    class RunConfig;
  }
  namespace daq
  {
    class CameraHandler; 
    class EventBuilder; 

    class Writer
    {
      public: 
        Writer(const dmtpc::core::RunConfig * runCfg, CameraHandler * handler, EventBuilder * builder); 
        virtual ~Writer(); 

        /* Empty the builder buffer, but don't wait for more events. Will join the writing thread.  */ 
        int flush(); 

        /* Start checking the builder buffer for more events */ 
        int start(); 

        /* Start checking the bias buffer for more bias frames */ 
        int dark(); 
        int flushDark(); 


      private: 
        const dmtpc::core::RunConfig * runCfg;
        CameraHandler * ccd_handler; 
        EventBuilder * builder; 
        dmtpc::core::WritableDataset * d; 
        volatile bool flushing; 
        TThread  th; 
        TThread  dark_th; 
        TTimeStamp * stamps[2]; 
        char * name; 
    }; 
  }
}; 


#endif
