#include "IonizingPhotonTable.hh"
#include "Thermo.hh"
#include "OpticalData.hh"

#include <iostream>



dmtpc::mc::mctpc::IonizingPhotonTable::IonizingPhotonTable(unsigned n, const double *p, const double *E, const double *t,double P, const char * mat)
{
  N = dmtpc::physics::thermo::gas_number_density(P); 
  material = mat; 
  max_p = 0; 
  for (unsigned i = 0; i < n; i++)
  {
    add(p[i], E[i]); 
  }

}

void dmtpc::mc::mctpc::IonizingPhotonTable::add(double p, double E, double t)
{

  static Garfield::OpticalData  od; 

  Es.push_back(E); 
  ts.push_back(t);



  double cs, eta; 
  od.GetPhotoabsorptionCrossSection(material, E, cs, eta); 

  p *= eta; 
  max_p += p; 
  ps.push_back(max_p); 

  etas.push_back(eta); 
  lambdas.push_back(10./(cs * N)); 

  printf("Added photon with p=%f, E=%f, eta =%f, lambda%f, t=%f\n", p,E, eta,lambdas[lambdas.size()-1], t); 
  if (max_p > 1)
  {
    std::cerr<<"WARNING: IonizingPhotonTable cumulative sums exceed 1" << std::endl; 
  }

}


bool dmtpc::mc::mctpc::IonizingPhotonTable::draw(double * E, double * lambda, double *t) const
{
  if (!ps.size()) return false; 
  double r = rand.Rndm(); 

  if (r > max_p) return false; 

  for (unsigned i = 0; i < ps.size(); i++)
  {
    if (r <= ps[i])
    {
      if (lambda) *lambda = lambdas[i];  
      if (t) *t = ts[i];  
      *E= Es[i]; 
      return true; 
    }
  }


  return false; 
}
