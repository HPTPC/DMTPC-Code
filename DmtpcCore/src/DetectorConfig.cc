#include "DetectorConfig.hh" 
#include <iostream>
#include <cstring>

ClassImp(dmtpc::core::DetectorConfig); 


dmtpc::core::DetectorConfig::DetectorConfig(int n, double * vals, const char ** descs, DetectorConfig::type * typs)
{
  values.reserve(n); 
  desc.reserve(n); 
  for (int i = 0; i < n; i++)
  {
    desc.push_back(TString(descs[i])); 
    values.push_back(vals[i]); 
    types.push_back(typs[i]); 
  }
}
  

int dmtpc::core::DetectorConfig::get(const char * key, double * val, type * type) const
{
  
  for (unsigned i = 0; i < desc.size(); i++)
  {
    if (!strcasecmp(desc[i].Data(), key))
    {

      if (val) *val = values[i]; 
      if (type) *type = types[i]; 

      return 0;

    }
  }

  std::cerr << " No item " << key << "found in DetectorConfig!" << std::endl; 
  return 1; 
}


double dmtpc::core::DetectorConfig::getValue(const char * key) const
{
  double val; 
  get(key,&val,0); 
  return val; 
}


int dmtpc::core::DetectorConfig::list(std::vector<const char *> *fillme) const
{
  if (!fillme) return 1; 

  fillme->reserve(desc.size()); 
  for (unsigned i = 0; i < desc.size(); i++)
  {
    fillme->push_back(desc[i].Data()); 
  }

  return 0; 
}
