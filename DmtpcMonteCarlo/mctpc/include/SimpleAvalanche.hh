#ifndef DMTPC_MC_MCTPC_SIMPLE_AVALANCHE_HH
#define DMTPC_MC_MCTPC_SIMPLE_AVALANCHE_HH

#include "Process.hh" 
#include "TH2.h"
#include <gsl/gsl_rng.h>
#include <stdint.h>

namespace dmtpc
{
  namespace mc
  {
    namespace mctpc
    {

      class SimpleAvalanche : public Process
      {
        public:

          struct Params
          {

            double gain;  //mean gas gain
            double frac_scint;  //fraction of ion decays 
            double avalanche_width; //mean width of avalanches
            double pitch ; //mesh pitch, in mm 
            double wire_width; //mesh wire width, in mm 
            double fraction_attached; //fraction of electrons getting attached prior to first excitation
            double gap; // gap size in mm
            double expo_lambda; // spatial exponential distribution slope constant. Should be approx effective townsend coefficient in mm^-1 
            double electron_drift_speed; //drift speed  in mm / ns
            double ion_drift_speed; //drift speed  in mm / ns
            double scintillation_time; //scintillation time in ns
            double mesh_center_width; // electrons get moved to mesh center + Gaus(this) 
            bool xycut ;  //cut on position? 
            double x0cut; //center for position cut
            double y0cut; //center for position cut
            double r2cut; //r^2 for cut
            double acceptance;  //acceptance of photons in amplification region... 


            Params() 
            {
              gain = 65e3; 
              frac_scint = 0.3; 
              avalanche_width = 0.06; 
              pitch = 0.254; 
              mesh_center_width = 0.254/10.; 
              wire_width = 0.03; 
              fraction_attached = 0.05; 
              gap = 0.435; 
              expo_lambda = 27; 
              electron_drift_speed = 0.1; 
              ion_drift_speed = 1.5e-3; 
              scintillation_time = 1; 
              xycut = false; 
              x0cut = 0; 
              y0cut = 0; 
              r2cut = 150 * 150; 
              acceptance = 1; 
            }



          }; 

          SimpleAvalanche(Params& p, double x0 = 0, double y0=0, double rot = 0, double xy_bin_size = 0.0254, 
                                     double xy_size = 10, double t_bin_size = 0.1, double t0 = 0, double t1 = 1000) ;
          virtual ~SimpleAvalanche(); 

          void setSeed(long seed) { gsl_rng_set(rand,seed); } 
          void setShapeParameter(double m) {alpha = m; } 
          virtual void clearSignals(); 
	  virtual void prepareSignals(dmtpc::analysis::MCTruth * truth); 

        protected:
          virtual int doProcess(unsigned size, const Particle * in); 

        private:
          Params sap; 

          double x0; 
          double y0; 
          double alpha; 
          double rot; 
          gsl_rng * rand; 
          TH2I avalanche_hist_xy; 
          double xy_size; 
          double xy_bin_size; 
          int max_xy_bin_x; 
          int max_xy_bin_y; 
          int min_xy_bin_x; 
          int min_xy_bin_y; 
          TH1I avalanche_hist_t; 
          uint64_t ntotal, nphotons,  nseen; 
          std::vector<uint32_t> nes; 

      };
    }
  }
}

#endif
