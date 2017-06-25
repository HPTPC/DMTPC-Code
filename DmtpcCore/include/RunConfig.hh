#ifndef DMTPC_CORE_RUN_CONFIG
#define DMTPC_CORE_RUN_CONFIG

#include "TNamed.h"
#include "Location.hh"
#include <stdint.h>

namespace dmtpc
{
  namespace core
  {
    struct RunConfig  : public TObject
    {
       RunConfig() { 
	 clear(); 
	 
	 // initialize some default settings
	 scope_wait_timeout = 100;
	 trigger_pretrigger = 10;
	 trigger_posttrigger = 10;
	 compression_algo = 1; // 1 = "lzma": see ROOT documentation
	 compression_level = 5;
	 ccd_light_shutter_mode="auto";
	 db_config = "db.conf";
       }  

       virtual ~RunConfig(){};

       const Location * getLocation() const {return &location; }

       void clear() {
	memset(this,0,sizeof(RunConfig));
      }

      void print(int flag=0) const { // depth of printing
	printf("RunConfig dump: out_dir=%s\n",out_dir.Data());
	printf("    runNo=%d, tag=%s\n",run_num,run_tag.Data());
	if (flag) {;}
      }

      Bool_t ccd_threaded_readout;
      Bool_t ccd_external_trigger;
      Bool_t ccd_dark_shutter_open;

      Int_t run_num;
      Int_t ccd_buffer_size;
      Int_t scope_buffer_size;
      Int_t scope_wait_timeout;

      Int_t trigger_pretrigger;
      Int_t trigger_posttrigger;
      Int_t compression_algo;
      Int_t compression_level;
   
      TString ccd_light_shutter_mode;
      TString out_dir;
      TString run_tag;
      TString output_filename;
      TString db_config;

      dmtpc::core::Location location;

      ClassDef(RunConfig,3); 
    };
  }
}



#endif

