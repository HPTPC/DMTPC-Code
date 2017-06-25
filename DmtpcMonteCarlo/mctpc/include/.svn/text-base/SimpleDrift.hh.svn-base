#ifndef DMTPC_MC_MCTPC_SIMPLE_DRIFT_HH
#define DMTPC_MC_MCTPC_SIMPLE_DRIFT_HH

#include "Process.hh" 
#include "TRandom3.h"

namespace dmtpc
{

  namespace mc
  {
    namespace mctpc
    {
      class SimpleDrift : public Process
      {

      public: 
        //mm, ns 
        SimpleDrift(double zmin, double zmax, double latDz, double longDz, double driftspeed = 0.1) 
          : z0(zmin), z1(zmax), Dz_lat(latDz), Dz_long(longDz), driftspeed(driftspeed), rng(0) 
        {xy_cut = false;}
        void setSeed(long seed) { rng.SetSeed(seed); } 

        virtual void clearSignals(); 
        virtual void prepareSignals(dmtpc::analysis::MCTruth * truth); 
        virtual void setXYCut(bool on, double x0, double y0, double r) { xy_cut = on; x0cut = x0; y0cut = y0; r2cut = r*r; }

      protected:
        virtual int doProcess(unsigned size, const Particle *in); 

      private:
        double z0;
        double z1; 
        double Dz_lat; 
        double Dz_long; 
        double driftspeed; 
        TRandom3 rng; 
        bool xy_cut; 
        double x0cut; 
        double y0cut; 
        double r2cut; 

        std::vector<double> diffused_x; 
        std::vector<double> diffused_y; 
        std::vector<double> diffused_z; 

      };
    }
  }
}


#endif
