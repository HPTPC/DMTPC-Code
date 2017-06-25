#include "VectorDistribution.hh" 
#include "TMath.h" 
#include "Astro.hh"

TVector3 * dmtpc::mc::mctpc::VectorDistribution::fillVector(TVector3 * fillme)
{
  makeVector(); 

  if (!fillme) fillme = new TVector3(x,y,z); 
  else fillme->SetXYZ(x,y,z); 


  return fillme; 
}


void dmtpc::mc::mctpc::IsotropicVectorDistribution::makeVector()
{
  rng.Sphere(x,y,z,r);

}void dmtpc::mc::mctpc::UniformCylinder::makeVector()
{
  

  while (true)
  {
    x = rng.Uniform(x0-r,x0+r); 
    y = rng.Uniform(y0-r,y0+r); 
    if (pow(x0-x,2) + pow(y0-y,2) < r*r)
      break;
  }

  z = rng.Uniform(z0,z1); 
}



void dmtpc::mc::mctpc::UniformBox::makeVector()
{
  x = rng.Uniform(x0,x1); 
  y = rng.Uniform(y0,y1); 
  z = rng.Uniform(z0,z1); 
}


void dmtpc::mc::mctpc::CollimatedDistribution::makeVector() 
{

  double phi = rng.Rndm() * 2 * TMath::Pi(); 
  double theta = asin(rng.Uniform(0,sin(collimation_angle)));

  TVector3 unit = direction->Unit(); 
  TVector3 orth = unit.Orthogonal(); 
  TVector3 orth2 = unit.Cross(orth); 
  TVector3 vec = unit * cos(theta) + sin(theta) * cos(phi) * orth + sin(theta) * sin(phi) * orth2; 

  x = vec.x(); 
  y = vec.y();
  z = vec.z(); 

}



void dmtpc::mc::mctpc::CygnusVectorDistribution::makeVector()
{

  double phi, theta; 
  dmtpc::physics::astro::getWimpDirInDetectorCoords(&phi, &theta, time, latitude, longitude, z_axis, x_axis); 

  x = cos(phi) * cos(theta);
  y = sin(phi) * cos(theta); 
  z = sin(theta); 

}





