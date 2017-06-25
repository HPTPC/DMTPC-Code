#include "SimpleDrift.hh"
#include "MCTruth.hh"

#include <cmath>

int dmtpc::mc::mctpc::SimpleDrift::doProcess(unsigned size, const Particle * in) 
{
  particles.reserve(size); 

  for (unsigned i = 0; i < size; i++)
  {
    Particle p = in[i]; 

    if (p.type == Particle::electron && p.z > z0 && p.z < z1) 
    {
      if (xy_cut && pow(p.x-x0cut,2) + pow(p.y-y0cut,2) > r2cut) continue; 
      double delta_z = p.z- z0; 
      double sigma_lat = sqrt(Dz_lat * (delta_z)); 
      double sigma_long = sqrt(Dz_long * (delta_z)); 

      double delta_t =  (delta_z + rng.Gaus(0,sigma_long)) /driftspeed; 

      p.z = z0; 
      p.t += delta_t; 
      p.x += rng.Gaus(0,sigma_lat);
      p.y += rng.Gaus(0,sigma_lat); 
      p.vx = 0; 
      p.vy = 0; 
      p.vz = -driftspeed; 

      diffused_x.push_back(p.x); 
      diffused_y.push_back(p.y); 
      diffused_z.push_back(p.z + driftspeed*delta_t); 

      particles.push_back(p); 
    }

  }

  return particles.size(); 
}


void dmtpc::mc::mctpc::SimpleDrift::prepareSignals(dmtpc::analysis::MCTruth * truth)
{

  truth->diffused_electron_x = diffused_x; 
  truth->diffused_electron_y = diffused_y; 
  truth->diffused_electron_z = diffused_z; 

}

void dmtpc::mc::mctpc::SimpleDrift::clearSignals()
{
  diffused_x.clear(); 
  diffused_y.clear(); 
  diffused_z.clear(); 
}

