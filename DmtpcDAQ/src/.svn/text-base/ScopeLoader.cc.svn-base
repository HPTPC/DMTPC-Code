#include "ScopeLoader.hh"
#include "ScopeConfig.hh"
#include <iostream>


#ifdef _SCOPE_CAEN_ 
#include "Scope_CAEN.hh" 
#include "CAENDigitizer.h"
#endif


std::vector<std::string> dmtpc::daq::ScopeLoader::getListOfDrivers()
{
  std::vector<std::string> ret; 
  std::cout << "Getting list of scope drivers..." << std::endl;
#ifdef _SCOPE_CAEN_ 
  std::cout << "Getting digitizer driver name..." << std::endl;
  ret.push_back(CAENDigitizer::getDriverNameStatic()); 
#endif

    return ret; 
}

void dmtpc::daq::ScopeLoader::printListOfDrivers()
{
  std::vector<std::string> drivers = getListOfDrivers(); 

  for (unsigned i = 0; i < drivers.size() ;i++)
  {
    std::cout << drivers[i] << std::endl; 
  }
}

std::vector<dmtpc::daq::Scope*> dmtpc::daq::ScopeLoader::loadScopes(const std::vector<dmtpc::core::ScopeConfig> * scopeCfgs)
{
  std::cout << "ScopeLoader::loadScopes()" << std::endl; 

  std::vector<Scope * > ret; 

  unsigned int nScopes = scopeCfgs->size();

  if (nScopes==0)
  {
    std::cerr << "WARNING: Scopes disabled by config" << std::endl; 
    return ret; 
  }

#ifdef _SCOPE_CAEN_ 
  for (unsigned int i = 0; i<nScopes; i++)
    {
      int handle;
      const dmtpc::core::ScopeConfig * sc =  &(scopeCfgs->at(i));
      int conn_type=1;
      if (sc->connection_type=="usb") conn_type=0;
      else if (sc->connection_type=="pcielink") conn_type=1;
      else if (sc->connection_type=="vmelink") conn_type=2;
      else {
	printf("ScopeLoader::loadScopes() connection_type =%s= unknown, Abort\n",sc->connection_type.Data());
	exit(77);
      }
      if (int err = CAEN_DGTZ_OpenDigitizer((CAEN_DGTZ_ConnectionType) conn_type, sc->link_num,sc->conet_node,0,&handle)) 
	{
	  std::cout << "For board " << i << " out of " << nScopes << std::endl;
	  fprintf(stderr,"Error in CAEN_DGTZ_OpenDigitizer: %d\n",err);
	}
      else ret.push_back(new CAENDigitizer(handle,i));
    }
#endif

  if (!ret.size()) 
  {
    std::cerr << "WARNING: no scopes loaded" << std::endl; 
  }
  return ret; 
}


