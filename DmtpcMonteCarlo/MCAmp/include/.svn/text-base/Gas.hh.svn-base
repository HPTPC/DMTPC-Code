#ifndef DMTPC_MC_AMP_GAS_HH
#define DMTPC_MC_AMP_GAS_HH

#include "MediumMagboltz.hh" 
#include <map>


namespace dmtpc
{
  namespace mc
  {
    namespace amp
    {
      class GasLoader
      {
        public: 

          GasLoader(const char * gasdir = "./gas"); 
          virtual ~GasLoader(); 

          Garfield::MediumMagboltz * getCF4(double pressure=60, double temp=298); 
        private:

          const char * gasdir; 
          std::map<char *, Garfield::MediumMagboltz*> loaded; 

      }; 

    }

  }

}


#endif
