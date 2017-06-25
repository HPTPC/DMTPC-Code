#ifndef DMTPC_MC_MCTPC_PARTICLE_DISTRIBUTION_HH
#define DMTPC_MC_MCTPC_PARTICLE_DISTRIBUTION_HH

#include "TVector3.h" 
#include "TRandom3.h"
#include "WimpGen.hh" 
#include "VectorDistribution.hh"
#include "ScalarDistribution.hh"
#include "ENDF_CrossSection.hh"
#include "ENDF_AngularDistribution.hh"

namespace dmtpc
{
  namespace mc
  {
    namespace retrim
    {
      class CollisionReader; 
    }
    namespace mctpc
    {

      const double MASS_19F = 17696896.8;
      const double MASS_12C = 11177928.7;
      const double MASS_N = 939565.378;

      class ParticleDistribution
      {
        public:
          virtual void draw() = 0 ; 


          double getE() const { return E; }
          double getTime() const { return time; }
          const TVector3* getPosition() const { return &position; }
          const TVector3* getDirection() const { return &direction; }
          const TVector3* getProjectileDirection() const { return &projectile_direction; }
          const TVector3* getSourceDirection() const { return &source_direction; }
          double getProjectileE() const { return projectileE; } 
          double getCosThetaCM() const { return cosThetaCM; } 
          double getProjectileM() const { return projectileM; } 
          virtual ~ParticleDistribution(){;}

        protected: 
          TVector3 position; 
          TVector3 direction; 
          TVector3 projectile_direction; 
          TVector3 source_direction; 
          double E; 
          double projectileE; 
          double time; 
          double cosThetaCM; 
          double projectileM; 


      }; 


      class IndependentDistribution : public ParticleDistribution
      {

        public:
          IndependentDistribution (VectorDistribution* pos_dist, VectorDistribution * dir_dist, ScalarDistribution * Edist, ScalarDistribution * tdist) 
            : position_dist(pos_dist), direction_dist(dir_dist), E_dist(Edist), t_dist(tdist) {;}

          virtual void draw(); 

        private: 
          VectorDistribution *position_dist; 
          VectorDistribution *direction_dist; 
          ScalarDistribution *E_dist; 
          ScalarDistribution *t_dist; 
      };


      class CollimatedNeutronSourceDistribution :  public ParticleDistribution
      {

        public: 
          CollimatedNeutronSourceDistribution( const TVector3 * source_position, const TVector3 * source_direction,  
                                               VectorDistribution * position_distribution, 
                                               ScalarDistribution * energy_distribution, 
                                               dmtpc::physics::endf::CrossSection * endf_cs, 
                                               dmtpc::physics::endf::AngularDistribution * endf_diff, 
                                               double collimation_degrees = 180, 
                                               double distance_reference = 1, // distance used to decide acceptance. 
                                               ScalarDistribution * tdist = 0, long seed = 0); 

          virtual void draw(); 

          //keV
          void setRecoilELimits(double min, double max) { maxRecoilE = max; minRecoilE = min; } 

          //keV
          void setRecoilMass(double mass) {recoilMass = mass; } 

          /* compute the xs integral (using MC integration... lol) */
          double getXSIntegral(int nsamples = 10000); 

        private: 


          double maxRecoilE, minRecoilE; 
          ScalarDistribution * Edist; 
          ScalarDistribution * tdist; 
          VectorDistribution * posdist; 
          dmtpc::physics::endf::CrossSection * endf_cs; 
          dmtpc::physics::endf::AngularDistribution * endf_diff; 
          TVector3 sourcepos;
          double distance_ref; 
          double recoilMass; 
          double collimation;
          TRandom3 rand;

      }; 

      class WIMPDistribution : public ParticleDistribution
      {

        public:
          WIMPDistribution(dmtpc::mc::wimpspectrum::WimpGen * gen, VectorDistribution * wind_distribution, ScalarDistribution * time_distribution, VectorDistribution * position_distribution) 
            : g(gen), wind_distribution(wind_distribution), time_distribution(time_distribution), position_distribution(position_distribution) {Emin = 0; Emax = 300;}

          void setELimits(double min, double max) { Emin = min; Emax = max; } 

          virtual void draw(); 
        private:

          dmtpc::mc::wimpspectrum::WimpGen * g; 
          VectorDistribution * wind_distribution; 
          ScalarDistribution * time_distribution;
          VectorDistribution * position_distribution;
          double Emin, Emax;
      }; 



      class AngledAlphaDistribution : public ParticleDistribution
      {
        public:

          /* srim tree here is derived from a COLLISON.txt using makeIonCollisionTree in retrim*/ 
          AngledAlphaDistribution(dmtpc::mc::retrim::CollisionReader * coll, const TVector3 * source_position, const TVector3 * collimator_direction); 
          void setZPlaneHeight(double z) { z_plane_height = z; computeMeanXY(); } 
          void setCollimatorBoreRadius(double r) { collimator_bore_r = r; } 
          void setCollimatorLength(double l) { collimator_length = l; } 
          void setDistanceSpread(double s) { distance_spread = s; } 
          void setXYCut(double max_distance) { xy_cut = max_distance*max_distance; }
          // if lower threshold width, use that in threshold (simulating physical threshold effect) 
          void setELimits(double min, double max, double lower_threshold_width = 0) { Emin = min; Emax = max; ELowerThresholdWidth = lower_threshold_width; } 
          void disableCathodeGrid() { cathode_grid_enabled = false; } 
          void enableCathodeGrid(double pitch = 25.4/50., double radius = 0.015, double x0 = 0, double y0 = 0, double rot=0) 
          { 
            cathode_grid_enabled = true; 
            cathode_grid_pitch = pitch; 
            cathode_grid_wire_radius = radius;
            cathode_grid_x0 = x0;
            cathode_grid_y0 = y0;
            cathode_grid_rotation = rot;
          } 
            
          virtual void draw(); 

        private: 

          dmtpc::mc::retrim::CollisionReader * coll; 
          int i; 
          TVector3 source_position; 
          TVector3 collimator_direction; 
          double z_plane_height; 
          double collimator_bore_r; 
          double collimator_length ;
          double distance_spread; 
          double xy_cut; 
          TRandom3 rand; 
          double mean_x; 
          double mean_y; 
          double Emin, Emax; 
          double ELowerThresholdWidth; 
          bool cathode_grid_enabled; 
          double cathode_grid_pitch; 
          double cathode_grid_wire_radius; 
          double cathode_grid_x0; 
          double cathode_grid_y0; 
          double cathode_grid_rotation; 

          void computeMeanXY(); 


      }; 

    }
  }
}


#endif
