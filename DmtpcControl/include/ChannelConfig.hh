#ifndef DMTPC_CONTROL_CHANNEL_CONFIG_HH
#define DMTPC_CONTROL_CHANNEL_CONFIG_HH

#include "Config.hh"

namespace dmtpc
{
  namespace control 
  {
    class ChannelConfig : public dmtpc::core::Config 
    {

      public: 
        ChannelConfig (const char * file); 

    }; 

  }
}


#endif
