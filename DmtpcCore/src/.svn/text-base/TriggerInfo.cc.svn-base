#include "TriggerInfo.hh" 


ClassImp(dmtpc::core::TriggerInfo); 

dmtpc::core::TriggerInfo::TriggerInfo(reason why, int npass, const char ** passed_triggers) 
{
  _why = why; 


  for (int i = 0; i < npass; i++)
  {
    passed.push_back(TString(passed_triggers[i])); 
  }
}
