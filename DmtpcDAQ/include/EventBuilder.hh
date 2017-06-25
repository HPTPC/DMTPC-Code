#ifndef DMTPC_DAQ_EVENT_BUILDER_HH
#define DMTPC_DAQ_EVENT_BUILDER_HH

#include <list> 
#include "TMutex.h" 


class TThread; 

namespace dmtpc
{
  namespace core
  {
    class Event; 
    class RunConfig;
  }

  namespace daq
  {
    class Stats; 
    class CameraHandler; 
    class ScopeHandler; 
    class TriggerHandler; 

    class EventBuilder 
    {
    
      public: 
      EventBuilder(const dmtpc::core::RunConfig * runCfg, CameraHandler * ccd_handler, 
		   ScopeHandler * scope_handler, TriggerHandler * trigger, Stats * stats); 
        virtual ~EventBuilder(); 
        int start(); 
        int available() const { return !built.empty(); } 
        int finished() const { return done && built.empty(); } 
        dmtpc::core::Event* pop(); 

      private:
        const dmtpc::core::RunConfig * runCfg;
        CameraHandler * ccds; 
        ScopeHandler * scopes; 
        TriggerHandler * trigger; 
        Stats * stats; 
        std::list<dmtpc::core::Event*> built; 
        std::vector<dmtpc::core::Event*> almost_dead;
        TMutex build_lock;
        unsigned almost_dead_idx; 
        unsigned dead; 
        volatile bool almost_dead_populated; 
        volatile bool done; 
        TThread * th; 

    };
  }
}
#endif
