#ifndef DMTPC_ANALYSIS_ANALYSIS_CONFIG_HH
#define DMTPC_ANALYSIS_ANALYSIS_CONFIG_HH


#include "Config.hh" 

//config file for various analysis programs, e.g. PlaylistFitter 


namespace dmtpc
{
  namespace analysis
  {
    class AnalysisConfig  : public dmtpc::core::Config
    {
      public:
        AnalysisConfig(const char * file = 0); 
        virtual ~AnalysisConfig(){ ; }

    }; 
  }
}






#endif 
