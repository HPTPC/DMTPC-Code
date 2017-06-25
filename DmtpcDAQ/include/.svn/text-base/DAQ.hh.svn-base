#ifndef DMTPC_DAQ_DAQ_HH
#define DMTPC_DAQ_DAQ_HH

#include "RunConfig.hh"
#include "CameraConfig.hh"
#include "ScopeConfig.hh"

namespace dmtpc
{
  namespace daq
  {
    class Writer; 
    class ScopeHandler; 
    class CameraHandler; 
    class EventBuilder; 
    class TriggerHandler; 
    class Stats; 

    class DAQ
    {
      public: 

        DAQ(void);
        virtual ~DAQ(); 

        int init(const char* xmlConfName, const char* outName, int runnum);
        int finish();
        int run(int ndark, int nlight, bool continuous=false); 

        const Writer * getWriter() const { return writer; } 
        const dmtpc::core::RunConfig * getRunConfig() const { return &runConfig; }
        const dmtpc::core::CameraConfig * getCameraConfig(int cam) const { return &vCameraConfig.at(cam); }
        const dmtpc::core::ScopeConfig * getScopeConfig(int scope) const { return &vScopeConfig.at(scope); }
        int nCams() { return vCameraConfig.size(); }
        int nScopes() { return vScopeConfig.size(); }
        int stop(); 
        int dark(int n); 
        int event(int n); 

      private:
        dmtpc::core::RunConfig runConfig;
        std::vector<dmtpc::core::CameraConfig > vCameraConfig;
        std::vector<dmtpc::core::ScopeConfig >  vScopeConfig;

        ScopeHandler * scope_handler; 
        CameraHandler * ccd_handler; 
        EventBuilder * builder; 
        TriggerHandler * trigger; 
        Writer * writer; 
        Stats * stats; 
        int success; 
        bool interrupted; 

        unsigned int utime_bias_first;
        unsigned int utime_exposure_first;
        unsigned int utime_exposure_last;
    }; 
  }
}


#endif

