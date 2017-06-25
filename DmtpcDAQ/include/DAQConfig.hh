#ifndef DMTPC_DAQ_CONFIG_HH
#define DMTPC_DAQ_CONFIG_HH

#include "Config.hh"
#include "Location.hh"

namespace dmtpc
{
  namespace daq
  {
    class DAQConfig : public dmtpc::core::Config
    {
      public:
        DAQConfig(const char * file = 0); 
        virtual ~DAQConfig() {;}
        const dmtpc::core::Location * getLocation() const {return &location; }
      private:
        dmtpc::core::Location location; 
    };

    enum SHUTTER_MODE
    {
      ALWAYS_CLOSED, 
      ALWAYS_OPEN, 
      AUTO
    }; 

    enum EXPOSE_MODE
    {
      SIMPLE, 
      CONCURRENT
    }; 

  }
}


#endif
