#ifndef DMTPC_DAQ_SCOPE_LOADER_HH
#define DMTPC_DAQ_SCOPE_LOADER_HH

#include <vector>
#include <string>

namespace dmtpc
{
  namespace core
  {
    class ScopeConfig;
    class ScopeChannelConfig;
  }
  namespace daq
  {
    class Scope; 
  
    namespace ScopeLoader
    {
      std::vector<std::string> getListOfDrivers();  
      void printListOfDrivers();  
      std::vector<Scope*> loadScopes(const std::vector<dmtpc::core::ScopeConfig> * scopeCfgs); 
    }
  }
}

#endif
