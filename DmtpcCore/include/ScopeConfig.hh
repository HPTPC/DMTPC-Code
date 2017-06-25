#ifndef DMTPC_CORE_SCOPE_CONFIG
#define DMTPC_CORE_SCOPE_CONFIG

#include <stdint.h>
#include <vector>
#include "TObject.h" 
#include "TString.h"
//#define DMTPC_SCOPE_CHANNEL_NAME_LENGTH 16 

using std::vector; 

namespace dmtpc
{
  namespace core
  {
    class ScopeChannelConfig : public TObject
    {
    public:
      ScopeChannelConfig() { clear(); }
      virtual ~ScopeChannelConfig() {};

      void clear() { memset(this,0,sizeof(ScopeChannelConfig)); }

      void print(int flag=0) { // depth of printing
	printf("ScopeChannelConfig dump: channel_id=%d\n",channel_id);
	printf("     dc_offset = %d, threshold = %d  name=%s\n",dc_offset,trigger_threshold,name.Data());
	if (flag) {;}
      }

      Bool_t pulse_polarity_falling;
      Bool_t trigger_polarity_falling;

      Int_t vmin;
      Int_t vmax;
      Int_t dc_offset;
      Int_t trigger_threshold;
      Int_t channel_id;
      Int_t scope;
      
      TString input_type;
      TString name;

      ClassDef(ScopeChannelConfig,1); 
    };

    class ScopeConfig 
    {
    public:

      ScopeConfig() { clear(); }
      virtual ~ScopeConfig() {};

      void clear() { memset(this,0,sizeof(ScopeConfig)); }

      void print(int flag=0) { // depth of printing
	printf("ScopeConfig dump: scope_id=%d\n",scope_id);
	printf("     record_length = %d, post_trigger = %d\n",record_length,post_trigger);
	printf("     no. enabled channels = %d\n",scopeChannelConfigs.size());
	if (flag>1) {
	  for (int i=0; i<scopeChannelConfigs.size(); i++) scopeChannelConfigs.at(i).print();
	}
      }
      Int_t scope_id;
      Int_t record_length;
      Int_t post_trigger;
      Int_t num_events_BLT;
      Int_t link_num;
      Int_t conet_node;

      TString connection_type;
      vector<ScopeChannelConfig> scopeChannelConfigs; 
      
      ClassDef(ScopeConfig,1); 
    }; 
  }
}

#endif


      /*
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
      */

