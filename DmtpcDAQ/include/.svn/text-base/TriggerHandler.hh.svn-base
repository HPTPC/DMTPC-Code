#ifndef DMTPC_DAQ_TRIGGER_HANDLER_HH
#define DMTPC_DAQ_TRIGGER_HANDLER_HH

#include <vector>

namespace dmtpc
{
  namespace core
  {
    class WritableEvent; 
    class TriggerInfo; 
    class RunConfig;
  }
  namespace daq
  {
    class Trigger; 
    //class DAQConfig; 
    class TriggerHandler
    {
      public: 
        //evaluates the event, and writes the trigger info 
       
        TriggerHandler(const dmtpc::core::RunConfig * runCfg); 
        bool eval(dmtpc::core::WritableEvent * checkme); 

        //unfortunately, the prebuffer can't easily be implemented inside here and must be implemented in the event builder
        unsigned getPreBufferSize() { return pretrigger; } 

      private: 
        std::vector<Trigger*> triggers; 
        unsigned pretrigger; 
        unsigned posttrigger; 
        unsigned since_last_trigger; 
    }; 

  }
}

#endif
