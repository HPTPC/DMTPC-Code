#ifndef DMTPC_CORE_TRIGGER_INFO_HH
#define DMTPC_CORE_TRIGGER_INFO_HH

#include <list>
#include "TObject.h" 
#include "TString.h"

namespace dmtpc
{
namespace core
{
  class TriggerInfo : public TObject
  {

    public: 

      enum reason
      {
        PASS,
        PRE, 
        POST
      }; 

      TriggerInfo(reason why = PASS, int npass = 0, const char ** passed_triggers = 0); 
      reason why() { return _why; } 
      const std::list<TString> * triggersPassed() const { return &passed; } 

    private: 
      reason _why; 
      std::list<TString> passed; 


      ClassDef(TriggerInfo,1); 

  }; 
}
}


#endif
