#ifndef DMTPC_MC_MCTPC_SAMPLED_MCAMP_AVALANCHE_HH
#define DMTPC_MC_MCTPC_SAMPLED_MCAMP_AVALANCHE_HH

#include <vector>
#include "IonizingPhotonTable.hh"
#include "Process.hh"
#include "TH2.h"
#include <gsl/gsl_rng.h>
#include <stdint.h>
class TChain; 

/* avalanche sampled from MCAmp.
 *
 * Much of this code is directly copied from SimpleAvalanche.
 * Perhaps an Avalanche class with the shared functionality would be useful... */ 

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
      class SampledMCAmpAvalanche : public Process
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
            Params()
            {
              gain = 0; 
              frac_scint = 0.3; 
              xycut = false; 
              x0cut=0;y0cut=0;r2cut=50*50; 
              acceptance=1; 
              randomize_angle = false; 
              randomize_offset = false; 
              max_offset = 0.127; 
              gap_top = 0.5; 
              gap_bottom = -0.5; 
              electron_drift_speed = 0.1; 
            }
          };
          
          SampledMCAmpAvalanche(TChain *c, const char * cut, Params &p, double x0=0, double y0=0, double rot = 0, 
                                double xy_bin_size = 0.0254, double xy_size=10, double t_bin_size = 0.1, double t0 = 0, double t1 = 1000); 
          ~SampledMCAmpAvalanche(); 
          void setSeed(long seed) { gsl_rng_set(rand,seed); } 
 


        protected:
          virtual int doProcess(unsigned size, const Particle * in); 
	  virtual void prepareSignals(dmtpc::analysis::MCTruth * truth); 
          virtual void clearSignals(); 

        private:
          TChain * chain; 
          Params par; 
          gsl_rng * rand; 
          TH2I avalanche_hist_xy; 
          TH1I avalanche_hist_t; 
          uint64_t ntotal, nphotons,  nseen; 
          std::vector<int> nes; 
          double gain_scale_factor; 
          double avg_gain; 

          void randomizeAngle(); 
          void randomizeOffset(); 
          int aval_ne; 
          double aval_x0; 
          double aval_y0; 
          std::vector<double> aval_xi; 
          std::vector<double> aval_yi; 
          std::vector<double> aval_zi; 
          std::vector<double> aval_ti; 
          std::vector<int> aval_visible; 
          std::vector<int> pass_cut; 
          double x0,y0;
          double orig_x0, orig_y0; 
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

