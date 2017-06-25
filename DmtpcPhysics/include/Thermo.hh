#ifndef DMTPC_PHYSICS_THERMO_HH
#define DMTPC_PHYSICS_THERMO_HH

namespace dmtpc
{
  namespace physics
  {
    namespace thermo
    {

      const double kB = 1.3806488e-23; 
      const double nA = 6.0221413e23; 
      const double R = kB * nA; 
      const double torr2pascal = 133.322368; 

      /* Returns the number density in cm^-3 */
      double gas_number_density(double pressure_in_T=60, double T_in_K=298);  

      /*Returns the density in g/cm^3 */
      double gas_density(double pressure_in_T=60, double amu=88, double T_in_K=298); 

    }
  }
}



#endif
