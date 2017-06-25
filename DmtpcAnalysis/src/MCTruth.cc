#include "MCTruth.hh" 

#include "TH2.h"

ClassImp(dmtpc::analysis::MCTruth); 

double dmtpc::analysis::MCTruth::xcam(double x,double y, int cam )
{
  double xx =  (x - xoffsets[cam] ) * xscale[cam]; 
  double yy =  (y - yoffsets[cam] ) * yscale[cam];

  double p = rotation.size() > 0 ? rotation[cam] : 0;

  return xx * cos(p) + yy * sin(p)+ truth_images[cam]->GetNbinsX()/2 * truth_images[cam]->GetXaxis()->GetBinWidth(1); 

}

double dmtpc::analysis::MCTruth::ycam(double x, double y, int cam )
{ 
  double xx =  (x - xoffsets[cam] ) * xscale[cam];
  double yy =  (y - yoffsets[cam] ) * yscale[cam]; 

  double p = rotation.size() > 0 ? rotation[cam] : 0;

  return yy * cos(p) - xx * sin(p) + truth_images[cam]->GetNbinsY()/2 * truth_images[cam]->GetYaxis()->GetBinWidth(1);; 

}

double dmtpc::analysis::MCTruth::x0cam(int cam )
{ return xcam(x0,y0,cam);}
double dmtpc::analysis::MCTruth::y0cam(int cam )
{ return ycam(x0,y0,cam); }

dmtpc::analysis::MCTruth::MCTruth()
{
  edge = false; 
  x0 = 0; 
  y0 = 0; 
  z0 = 0; 
  time = 0; 
  pM = 0; 
  dx = 0; 
  dy = 0; 
  dz = 0; 
  cosThetaCM = -2; 
  pdx = 0;
  pdy = 0;
  species = 0; 
  pdz = 0;
  pE = 0; 
  sdx = 0;
  sdy = 0;
  sdz = 0;
  ionization = 0; 
  nprimary = 0; 
  avalanche_visible_photon_xy = 0; 
  avalanche_visible_photon_t = 0; 
  recoilEnergy = 0; 
  pressure = 0; 
}

void dmtpc::analysis::MCTruth::clear()
{

  primary_electron_x.clear(); 
  primary_electron_y.clear(); 
  primary_electron_z.clear(); 

  diffused_electron_x.clear(); 
  diffused_electron_y.clear(); 
  diffused_electron_z.clear(); 

  for (unsigned i = 0; i < truth_images.size(); i++)
  {
    delete truth_images[i]; 
  }

  for (unsigned i = 0; i < truth_traces.size(); i++)
  {
    delete truth_traces[i]; 
  }

  truth_images.clear(); 
  truth_traces.clear(); 
  nphoton.clear(); 
  nadu.clear(); 
  nadu_raw.clear(); 
  xoffsets.clear();
  yoffsets.clear(); 
  rotation.clear(); 
  xscale.clear(); 
  yscale.clear(); 
  noise.clear(); 
  individual_avalanche_gain.clear(); 


  if (avalanche_visible_photon_xy) 
  {
    delete avalanche_visible_photon_xy; 
    avalanche_visible_photon_xy = 0; 
  }
  if (avalanche_visible_photon_t) 
  {
    delete avalanche_visible_photon_t; 
    avalanche_visible_photon_t = 0; 
  }
}

