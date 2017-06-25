#ifndef DMTPC_MC_MCTPC_CACHED_MCAMP_AVALANCHE_HH
#define DMTPC_MC_MCTPC_CACHED_MCAMP_AVALANCHE_HH

#include <stdint.h>
#include <vector>
#include "Process.hh"
#include "TH2.h"
#include <gsl/gsl_rng.h>
class TChain; 

#include "MCAmpAvalancheCache.hh"
#include "IonizingPhotonTable.hh"



namespace dmtpc
{
  namespace analysis
  {
    class MCTruth;
  }

  namespace mc
  {
    namespace mctpc
    { 
      class CachedMCAmpAvalanche : public Process
      {
        public: 
          struct Params
          {
            double gain; // set to zero to use from chain, otherwise will be scaled
            double frac_scint; 
            double x0cut; 
            double y0cut; 
            double r2cut; 
            double acceptance; 
            double scintillation_time; 
            bool xycut; 
            bool randomize_angle;
            bool randomize_offset;
            double max_offset; 
            double gap_top; 
            double gap_bottom; 
            double electron_drift_speed;
            IonizingPhotonTable photon_table; 
            MCAmpAvalancheCache::Params cache_params; 
            double dz;

            Params()
            {
              gain = 0; 
              frac_scint = 0.3; 
              xycut = false; 
              x0cut=0;y0cut=0;r2cut=50*50; 
              acceptance=1; 
              scintillation_time = 10; 
              randomize_angle = false; 
              randomize_offset = false; 
              max_offset = 0.127; 
              electron_drift_speed = 0.1; 
              gap_top = 0.5; 
              gap_bottom = -0.5; 
              electron_drift_speed = 0.1; 
              dz = 0.0107;

            }
          };
          
          CachedMCAmpAvalanche(TChain *c, const char * cut, Params &p, double x0=0, double y0=0, double rot = 0, 
                                double xy_bin_size = 0.0254, double xy_size=10, double t_bin_size = 0.1, double tmax = 5000,
                                const char * cachedir ="cache"); 
          ~CachedMCAmpAvalanche(); 
          void setSeed(long seed) { gsl_rng_set(rand,seed); } 
 

        protected:
          virtual int doProcess(unsigned size, const Particle * in); 
	  virtual void prepareSignals(dmtpc::analysis::MCTruth * truth); 
          virtual void clearSignals(); 


        private:
          MCAmpAvalancheCache cache; 
          Params par; 
          gsl_rng * rand; 
          TH2I avalanche_hist_xy; 
          TH1I avalanche_hist_t; 
          uint64_t ntotal, nphotons,  nseen; 
          std::vector<uint32_t> nes; 
          double gain_scale_factor; 
          double avg_gain; 

          void randomizeAngle(); 
          void randomizeOffset(); 
          double x0, y0; 
          double orig_x0,orig_y0;
          double rot; 
          double xy_size; 
          double xy_bin_size; 
          int max_xy_bin_x; 
          int max_xy_bin_y; 
          int min_xy_bin_x; 
          int min_xy_bin_y; 


      }; 
    }
  }
}

#endif
