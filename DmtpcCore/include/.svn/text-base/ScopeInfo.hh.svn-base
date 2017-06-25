#ifndef DMTPC_CORE_SCOPE_INFO
#define DMTPC_CORE_SCOPE_INFO

#include <stdint.h>
#include <vector>
#include "TObject.h" 
#define DMTPC_SCOPE_CHANNEL_NAME_LENGTH 16 

using std::vector; 

namespace dmtpc
{
  namespace core
  {
    class ScopeChannelInfo 
    {
      public:
        char name[DMTPC_SCOPE_CHANNEL_NAME_LENGTH]; 
        double vmin; 
        double vmax; 
        size_t nbits; 
        size_t nbytes_raw; //sizeof(int) for how it's stored
        size_t sample_rate; //khz 
        size_t nsamples; 
        size_t nsamples_pretrigger; 
        size_t channel;  
        size_t scope; 

        ClassDef(ScopeChannelInfo,2); 
    };

    class ScopeTraceInfo
    {
      public:
        uint32_t secs; // Should someone deal with y2k38 problem?  
        uint32_t nsecs; 
        size_t channel; 
        size_t scope;

        ClassDef(ScopeTraceInfo,2); 
    };

    class ScopeInfo 
    {
      public:
        vector<ScopeChannelInfo> channel_info; 
        size_t scope_number;

        ClassDef(ScopeInfo,1); 
    }; 
  }
}

#endif


