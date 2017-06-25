#include "VelocityDistribution.hh"
#include "TVector3.h" 
#include <cmath>
#include "RNG.hh"
#include "TRandom3.h" 
#include <assert.h>


void dmtpc::mc::wimpspectrum::VelocityDistribution::v(TVector3  *v, const TVector3 * vE)
{
  double vEMag = vE->Mag(); 

  TVector3 unit = vE->Unit(); 
  TVector3 orth1 = unit.Orthogonal(); 
  TVector3 orth2 = unit.Cross(orth1); 

  double phi =  2 *M_PI * rng::rng()->Rndm(); 
  double cosp = cos(phi); 
  double sinp = sin(phi); 
  TVector3 vwimp; 
  TVector3 vgal; 

  double G = g(0,vEMag); 

  while(true)
  {
     double vTry = (vEsc + vEMag) * rng::rng()->Rndm() ; 
     double costh = 2*rng::rng()->Rndm() - 1; 
     double sinth = sqrt(1-costh*costh); 

     vwimp= unit * vTry * costh  + orth1 *vTry * cosp * sinth + orth2 * vTry * sinth * sinp; 
     vgal = vwimp + *vE; 
     double v = vwimp.Mag();
     double V = vgal.Mag(); 

     if (V > vEsc) continue; 

     double F = v*v*v * f(V); 

     if (gDependsOnv) 
       G = g(v,vEMag); 

     assert(F < G); 

     if (rng::rng()->Rndm()  * G < F) break; 
  }

  v->SetXYZ(vwimp.X(), vwimp.Y(), vwimp.Z()); 

}


dmtpc::mc::wimpspectrum::MaxwellBoltzmannDistribution::MaxwellBoltzmannDistribution(double vesc , double v0) : v02inv(1./(v0*v0)), v0(v0)
{
  vEsc = vesc; 
  gDependsOnv = false; 

}

double dmtpc::mc::wimpspectrum::MaxwellBoltzmannDistribution::f(double v) const
{
  return  exp(-v*v*v02inv); 
}

double dmtpc::mc::wimpspectrum::MaxwellBoltzmannDistribution::g(double v, double vE) const
{
  double vmax = 0.5*( vE +  sqrt(vE*vE + 6*v0*v0)); 
  return vmax*vmax*vmax * f(-(vmax*vmax - 2 * vmax * vE + vE*vE) /v0/v0); 
}

