#include "ParticleDistribution.hh"
#include "CollisionReader.hh"
#include "TMath.h"
#include "Geometry.hh"
#include "WimpGen.hh"
#include "RecoilKinematics.hh"

#include <cstdio>
#include <cmath>


void dmtpc::mc::mctpc::IndependentDistribution::IndependentDistribution::draw()
{

  //don't fill projectileDirection 
  position_dist->fillVector(&position); 
  direction_dist->fillVector(&direction); 
  E = E_dist->value(); 
  time = t_dist->value(); 
  projectileE = 0; 
  cosThetaCM = -2; 
}


double dmtpc::mc::mctpc::
CollimatedNeutronSourceDistribution:: getXSIntegral(int N) 
{

  double answer = 0; 
  for (int i = 0; i < N; i++)
  {
    double E = Edist->value(); 
    answer += endf_cs->crossSection(E); 
  }

 return answer/N; 
}


dmtpc::mc::mctpc::
CollimatedNeutronSourceDistribution::
CollimatedNeutronSourceDistribution( const TVector3 * source_position, const TVector3 * source_direction, 
                                      VectorDistribution * position_distribution, 
                                      ScalarDistribution * energy_distribution, 
                                      dmtpc::physics::endf::CrossSection * endf_cs, 
                                      dmtpc::physics::endf::AngularDistribution * endf_diff, 
                                      double collimation_degrees,
                                      double distance_reference, 
                                      ScalarDistribution * tdist, long seed)
            :  
               Edist(energy_distribution), tdist(tdist),posdist(position_distribution), 
               endf_cs(endf_cs), endf_diff(endf_diff), sourcepos(*source_position),distance_ref(distance_reference), 
                rand(seed)

{
  collimation = cos (collimation_degrees * M_PI/180.); 
  this->source_direction = source_direction->Unit(); 
  minRecoilE = 1;
  maxRecoilE = 250; 
  recoilMass = MASS_19F; //keV/c^2
  projectileM = MASS_N; 
}

void dmtpc::mc::mctpc::WIMPDistribution::draw()
{

  //pick a time

  time = time_distribution->value(); 
  wind_distribution->setTime(time); 


  TVector3 wind; 

  while (true)
  {
    wind_distribution->fillVector(&wind); 
    source_direction = -wind; 
    g->setWindDirection(&wind); 
    g->generate(); 
    E = g->getE(); 
    if (E > Emin && E < Emax) break; 
  }


  direction.SetMagThetaPhi(1, g->getTheta(), g->getPhi());  
  projectile_direction.SetMagThetaPhi(1,g->getThetaW(), g->getPhiW());

  position_distribution->fillVector(&position); 

  projectileE = 0; 
  projectileM = g->getWimpMass();
  cosThetaCM = -2; 
}

void dmtpc::mc::mctpc::CollimatedNeutronSourceDistribution::draw()
{

  //pick a position 
  int niter = 1;  
  while (true)
  {
     if (niter % 1000 == 0) 
     {
       fprintf(stderr,"Warning: %d attempts with no valid positions, perhaps collimation misaligned?",niter); 
     }

     posdist->fillVector(&position); 
//     printf("[%f,%f,%f]\n", position.x(), position.y(), position.z()); 

     TVector3 displacement =  position - sourcepos; 

     double dist = displacement.Mag(); 

     //neglect points closer than distance_Rf
     if (dist  < distance_ref) continue; 
     
     //scale according to 1/r^2 
     if ( rand.Rndm() > pow(distance_ref/dist,2))  continue; 

     projectile_direction = displacement.Unit(); 

     //woohoo, this is good! 
     if (projectile_direction.Dot(source_direction) >= collimation)
     {
       break; 
     }
  }




  

  // scale by cross section and ensure we can generate the recoil with minimum energy

  double cosrecoil = 0; 

  double Eneutron = Edist->value(); 

  while (true)
  {

    while(!endf_cs->acceptEnergy(Eneutron, &rand))  
    {
      Eneutron = Edist->value(); 
    }


    while (true)
    {
      double scattercos = endf_diff->randomCosine(Eneutron, &rand); 
      E =  dmtpc::physics::recoil::recoilEnergyFromCosScatterCMS(scattercos, Eneutron, MASS_N, recoilMass); 
      cosrecoil =  dmtpc::physics::recoil::cosRecoilAngle(E, Eneutron, MASS_N, recoilMass); 
      if (fabs(cosrecoil) <=1)
      {
        cosThetaCM = scattercos; 
        break; 
      }
      else
      {
        printf("%f\n",cosrecoil); 
      }
        
    }

    if (E >= minRecoilE && E<= maxRecoilE)
    {
      break; 
    }
  }

  projectileE = Eneutron; 

  double sinrecoil = sqrt(1-cosrecoil * cosrecoil); 
  double phi = rand.Rndm() * 2 * M_PI; 

  TVector3 orth = projectile_direction.Orthogonal().Unit(); 
  TVector3 orth2 = projectile_direction.Cross(orth); 

  direction = projectile_direction * cosrecoil + orth * sinrecoil * cos(phi) + orth2 * sinrecoil * sin(phi); 
  time = tdist ? tdist->value() : 0; 
}


void dmtpc::mc::mctpc::AngledAlphaDistribution::draw()
{
  int start_i = i; 

  double E0,E1,x0,x1,y0,y1,z0,z1; 
  bool accept = false;

  double max_angle = atan2(collimator_bore_r,collimator_length); 

  TVector3 orth = collimator_direction.Orthogonal(); 

  //randomize axes 
  orth.Rotate(rand.Uniform(0,2*TMath::Pi()),collimator_direction);
  TVector3 orth2 = collimator_direction.Cross(orth); 

  while(true)
  {
 
    int ncoll = coll->nCollisions(i); 

    double source_pos_r = rand.Uniform(0,collimator_bore_r); 
    double source_pos_phi = rand.Uniform(0,2*TMath::Pi()); 
    double source_dir_sin_theta = rand.Uniform(0,sin(max_angle)); 
    double source_dir_cos_theta = sqrt(1-pow(source_dir_sin_theta,2)); 
    double source_dir_phi = rand.Uniform(0,2*TMath::Pi()); 

    TVector3 orth_random = orth; 
    orth_random.Rotate(source_dir_phi, source_direction); 
    source_direction = collimator_direction * source_dir_cos_theta + orth_random * source_dir_sin_theta; 

    double rdy = source_pos_r * cos(source_pos_phi); 
    double rdz = source_pos_r * sin(source_pos_phi); 



    TVector3 last; 
    TVector3 pt; 

    double xx = rand.Gaus(0,distance_spread); 
    for (int j = 1; j < ncoll; j++)
    {
      E0 = coll->getCollision(i,j-1)->Ei; 

      x0 = coll->getCollision(i,j-1)->x;
      y0 = (coll->getCollision(i,j-1)->y + rdy) ; 
      z0 = (coll->getCollision(i,j-1)->z + rdz) ; 



//      // hit collimator, roughly 
//      if (x0 < collimator_length && y0*y0 + z0*z0 > collimator_bore_r2) 
//      {
//        break; 
//      }

      x0 += xx;  // account for spread; 

      E1 = coll->getCollision(i,j)->Ei; 
      x1 = coll->getCollision(i,j)->x + xx; 
      y1 = coll->getCollision(i,j)->y + rdy; 
      z1 = coll->getCollision(i,j)->z + rdz; 

      //transform to lab 
      
      if(j==1)
      {
        last = source_position + source_direction* x0 + orth * y0 + orth2 * z0; 
      }

      pt = source_position + source_direction* x1 + orth * y1 + orth2 * z1; 

      //check if hit collimator


      if (dmtpc::math::linePointDistance(&pt, &source_position, &collimator_direction) < collimator_bore_r && dmtpc::math::pointPlaneDistance(&pt, &source_position, &collimator_direction) < collimator_length); 


//      printf("%f %f\n",last.z(), pt.z()); 
      //check for z crossing
      if ((pt.z() - z_plane_height > 0) == (last.z() - z_plane_height > 0))
      {
        last = pt; 
        continue; 
      }

      //find x,y of crossing 
      double frac = fabs( (z_plane_height - last.z()) / (pt.z() - last.z())); 

      //check E 
      E = frac * E0 + (1-frac)*E1; 

      if (E > Emax) 
        break; 

      if (ELowerThresholdWidth == 0 && E < Emin) 
        break; 

      if (ELowerThresholdWidth && rand.Gaus(E,ELowerThresholdWidth) < Emin) 
        break; 

      double xcross = frac * last.x() + (1-frac) * pt.x(); 
      double ycross = frac * last.y() + (1-frac) * pt.y(); 

      //check if too far
      if (pow(xcross-mean_x,2) + pow(ycross-mean_y,2) > xy_cut)
      {
        printf("%d too far\n",i); 
        break; 
      }

      if (cathode_grid_enabled) 
      {
        fprintf(stderr,"cathode grid not yet implemented\n"); 

      }



      position.SetXYZ(xcross,ycross,z_plane_height); 
      projectileE = E; 
      time = i; 

      TVector3 dir(pt.x()-last.x(), pt.y()-last.y(), pt.z()-last.z()); 
      direction = dir.Unit(); 
      projectile_direction = direction; 

      printf("%d\n",i); 
      accept = true; 
      break; 
    }


    i = (i+1) % coll->nTracks(); 
    if (accept) break; 
    if (i == start_i) 
    {
      fprintf(stderr,"WARNING in AngledAlphaDistribution: exhausted tracks?\n"); 
      orth.Rotate(rand.Uniform(0,2*TMath::Pi()), collimator_direction);
      orth2 = collimator_direction.Cross(orth); 
    }
  }
}

void dmtpc::mc::mctpc::AngledAlphaDistribution::computeMeanXY()
{

  //intersection of z plane with vector from source position

  double t = (z_plane_height - source_position.z()) / collimator_direction.z(); 

  if (t < 0) 
  {
    fprintf(stderr,"WARNING in AngledAlphaDistribution: source pointing wrong way?\n"); 
  }

  mean_x = source_position.x() + t * collimator_direction.x(); 
  mean_y = source_position.y() + t * collimator_direction.y(); 

}

dmtpc::mc::mctpc::AngledAlphaDistribution::AngledAlphaDistribution(dmtpc::mc::retrim::CollisionReader *c, const TVector3 * source_position, const TVector3 * source_direction)
: coll(c), source_position(*source_position), z_plane_height(0), collimator_bore_r(1), collimator_length(1.06*25.4), distance_spread(5), xy_cut(20*20), rand(0)
{

  this->collimator_direction = source_direction->Unit();
  this->source_direction = this->collimator_direction; 
  computeMeanXY(); 
  i = 0; 
  setELimits(20,250); 
  disableCathodeGrid(); 

}


