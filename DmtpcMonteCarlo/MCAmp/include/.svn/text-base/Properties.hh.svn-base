#ifndef DMTPC_MC_AMP_PROPERTIES_HH
#define DMTPC_MC_AMP_PROPERTIES_HH

#include "Amplifier.hh" 

namespace Garfield
{
class Medium; 
}

namespace dmtpc
{
  namespace mc
  {
    namespace amp
    {


      struct SurfaceProperties
      {
        SurfaceProperties() { voltage = 0; voltageDefined = false; isElectrode = false; amp= 0; } 
        double voltage; 
        bool voltageDefined; 
        bool isElectrode; 
        Amplifier * amp; 
      }; 

      struct VolumeProperties
      {
        VolumeProperties() { description = 0; epsilon = 1; medium = 0; conductivity=0; } 

        const char * description; 
        double epsilon; 
        double conductivity; 
        Garfield::Medium * medium; 
      }; 
    }
  }
}


#endif
