#include "MultiMeshAvalanche.hh" 
#include <cmath> 


dmtpc::mc::mctpc::MultiMeshAvalanche::Particle::Params(preset p)
{

  if (p == CANARY_650)
  {
    nmeshes = 2; 
    gain = {65.e3}; 
    frac_scint = 0.3; 
    avalanche_width = {0.06}; 
    pitch = {0.254}; 
    mesh_center_width = {0.254/10}; 
    wire_width = {0.03}; 
    fraction_attached = 0.05; 
    gap = {0.5}; 
    expo_lambda = {27}; 
    electron_drift_speed = 0.1; 
    ion_drift_speed = 1.5e-3; 
    scintillation_time = 1; 

  }

  else if (p == M3)
  {
    nmeshes = 2; 
    gain = {100e3};
    frac_scint = 0.3; 
    avalanche_width = {0.06}; 
    pitch = {0.254}; 
    mesh_center_width = {0.254/10}; 
    wire_width = {0.03}; 
    fraction_attached = 0.05; 
    gap = {0.5}; 
    expo_lambda = {27}; 
    electron_drift_speed = 0.1; 
    ion_drift_speed = 1.5e-3; 
    scintillation_time = 1; 
  }




}


int dmtpc::mc::mctpc::MultiMeshAvalanche::doProcess(unsigned size, const Particle * in) 
{
  double total_gain = 1; 
  double partial_gain = 1; 
  for (unsigned i = 0; i < mmap.gain->size(); i++)
  {
    total_gain *= mmap.gain[i]; 
    if ( partial_gain < mmap.gain[i])
      partial_gain = mmap_gain[i]; 
  }

  particles.reserve(1.1*mmap.frac_scint * total_gain * size * mmap.acceptance ); 

  std::vector<Particle> temporary_electrons;
  temporary_electrons.reserve(partial_gain*1.1); 

  for (int i = 0; i < size; i++)
  {

    if (mmap.xycut && pow(p.x-mmap.x0cut,2) + pow(p.y - mmap.y0cut,2) > mmap.r2cut) continue; 

    //transform to mesh frame
    Particle p = in[i]; 

    if (! p.type == Particle::electron) continue; 
    temporary_electrons.clear(); 
    temporary_electrons.push_back(p); 

    for (int m = 0; m < mmap.nmeshes -1; m++)
    {

      double x= p.x * cos(rot) + sin(rot) * p.y - x0; 
      double y= -p.x * sin(rot) + cos(rot) * p.y - y0; ; 

      //move to nearest mesh center

      x = round(x/pitch) * pitch; 
      y = round(y/pitch) * pitch; 

      //compute gain
      
      int ne = int (gain *Polya(1,&rng)); 
      //generate visible photons

      for (int j = 0; j < ne; j++)
      {
        if (rng.Uniform() > frac_scint) continue; 
        Particle photon; 
        double xx = rng.Gaus(x,width); 
        double yy = rng.Gaus(y,width); 

        if ( (xx-x)/pitch >  ( 0.5 - wire_width/pitch))
          continue; //assumed to be blocked by mesh
        if ( (yy-y)/pitch >  ( 0.5 - wire_width/pitch))
          continue; //assumed to be blocked by mesh

        //transform back to lab frame
        xx+=x0; 
        yy+=y0; 

        photon.x =  xx * cos(rot) - yy*sin(rot); 
        photon.y =  yy * cos(rot) + xx*sin(rot); 

        photon.z = 0; 
        photon.vx=0; 
        photon.vy=0; 
        photon.vz=0; 
        
        photon.type = Particle::photon; 
        particles.push_back(photon); 

      }
    }
  }

  return particles.size(); 
}
