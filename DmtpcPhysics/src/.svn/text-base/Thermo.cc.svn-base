#include "Thermo.hh"


double dmtpc::physics::thermo::gas_number_density(double P, double T)
{
  return P * torr2pascal / (kB * T) * 1e-6;  
}

double dmtpc::physics::thermo::gas_density(double P, double  MM, double T)
{
  return P * torr2pascal / (R*T) * MM * 1e-6; 
}
