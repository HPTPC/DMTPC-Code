#ifndef DMTPC_DAQ_SCOPE_HANDLER_HH
#define DMTPC_DAQ_SCOPE_HANDLER_HH

#include <vector>

class TThread; 
namespace dmtpc
{
  namespace core
  {
    class RunConfig;
    class ScopeConfig;
    class ScopeChannelConfig;
  }
  namespace daq
  {
    class Scope; 
    class Buffer; 

    class ScopeHandler
    {
      public: 

      ScopeHandler(const dmtpc::core::RunConfig * runCfg, 
		   const std::vector<dmtpc::core::ScopeConfig> * scopeCfgs); 
        int start(); 
        int stop(); 
        bool done(); 
        bool allConsumed(); 
        Buffer* getBuffer(int i) { return buffers[i]; } 
        unsigned getNScopes() const { return scopes.size(); } 
        const Scope * getScope(int i) const  { return scopes.at(i); } 
       
      private: 
        std::vector<Scope*> scopes; 
        std::vector<Buffer*>  buffers;
        const std::vector<dmtpc::core::ScopeConfig> * scopeCfgs;
        TThread * scope_thread; 
        volatile bool stopme; 
        volatile bool isDone;
        const dmtpc::core::RunConfig * runCfg;
    }; 

  }
}


#endif
