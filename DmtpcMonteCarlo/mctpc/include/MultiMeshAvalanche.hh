#ifndef DMTPC_MC_MCTPC_MULTI_AVALANCHE_HH
#define DMTPC_MC_MCTPC_MULTI_AVALANCHE_HH

#include "Process.hh" 
#include "TRandom3.h" 
#include <vector>


namespace dmtpc
{
  namespace mc
  {
    namespace mctpc
    {

      class MultiMeshAvalanche : public Process
      {
        public:

          struct Params
          {

            enum preset
            {
              CANARY_650,
              M3 
            }; 

            int nmeshes; 
            const double * gain;  //mean gas gain
            const double * avalanche_width; //mean width of avalanches
            const double * pitch ; //mesh pitch, in mm 
            const double * wire_width; //mesh wire width, in mm 
            const double * gap; // gap size in mm
            const double * expo_lambda; // spatial exponential distribution slope constant. Should be approx effective townsend coefficient in mm^-1 
            const double * x0; 
            const double * y0; 
            const double * rotation; 

            double electron_drift_speed; //drift speed  in mm / ns

            bool xycut ;  //cut on position? 

            double frac_scint;  //fraction of ion decays 
            double fraction_attached; //fraction of electrons getting attached prior to first excitation
            double ion_drift_speed; //drift speed  in mm / ns
            double scintillation_time; //scintillation time in ns
            double *  mesh_center_width; // electrons get moved to mesh center + Gaus(this) 
            double x0cut; //center for position cut
            double y0cut; //center for position cut
            double r2cut; //r^2 for cut
            double acceptance;  //acceptance of photons in amplification region... 


            Params(preset p = M3) ;



          }; 


          MultiMeshAvalanche(Params & p); 

          void setSeed(long seed) { rng.SetSeed(seed); } 
//          void setTheta(double th) { theta = theta; } 

        protected:
          virtual int doProcess(const std::vector<Particle> * in); 

        private:
          Params mmap; 
          TRandom3 rng; 

      };
    }
  }
}

#endif
