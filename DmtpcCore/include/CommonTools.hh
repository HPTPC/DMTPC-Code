#ifndef DMTPC_CORE_COMMON_TOOLS_HH
#define DMTPC_CORE_COMMON_TOOLS_HH


/** this namespace contains a variety of utility functions used throughout the codebase */ 


class TChain; 

namespace dmtpc
{
  namespace core
  {
    namespace common
    {
      int addFilesToChain(TChain * ch, const char * file_list); 
    }
  }
}



#endif
