#ifndef DMTPC_CORE_WRITABLE_EVENT_HH
#define DMTPC_CORE_WRITABLE_EVENT_HH

#include "Event.hh" 
class TTimeStamp; 

namespace dmtpc
{
  namespace core
  {

    class WritableEvent : public Event
    {
      public: 
        std::vector<Image *>* ccdDataPtr() {return &_ccdData;}
        //std::vector<Image *>* overscanPtr() {return &_overscan;}
        std::vector<dmtpc::core::Waveform *>* scopeDataPtr() {return &scopeData;}
        std::vector<dmtpc::core::ScopeInfo *>* scopeConfigsPtr() {return &scopeConfigs;}
        void setRunNumber(int run) { runNumber = run; } 
        void setEventNumber(int evt) { eventNumber = evt; } 
        void setLocation(Location * loc) { location = loc; }
        void setTriggerInfo(TriggerInfo * tinfo) { trigger = tinfo; } 
        void setTimeStamp(TTimeStamp * time) { timestamp = time; } 
        void setDetectorConfig(DetectorConfig * config) {detectorConfig = config; } 
      private:
        ClassDef(WritableEvent,2); 
    }; 
  }
}


#endif
