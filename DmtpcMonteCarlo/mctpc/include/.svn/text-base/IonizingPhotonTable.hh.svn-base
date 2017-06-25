#ifndef DMTPC_MC_MCTPC_IONIZING_PHOTON_TABLE_HH
#define DMTPC_MC_MCTPC_IONIZING_PHOTON_TABLE_HH

#include <vector>
#include "TRandom3.h"


namespace dmtpc
{
  namespace mc
  {
    namespace mctpc
    {

      class IonizingPhotonTable
      {

        public:

          IonizingPhotonTable(unsigned n = 0,const double * p = 0, const double * E = 0, const double * t = 0, double P=30, const char * mat = "CF4"); 

         //returns a photon energy, probably zero! if non-zero, the pointers are filled 
          bool draw(double * E, double * lambda = 0, double * t = 0) const;
          void add(double p, double E, double t=1); 
          void setSeed(long seed) { rand.SetSeed(seed); }



        private:

          std::vector<double> ps; 
          std::vector<double> Es; 
          std::vector<double> ts; 
          std::vector<double> lambdas; 
          std::vector<double> etas; 
          mutable TRandom3 rand;
          double N; 
          const char * material;
          double max_p; 

      };



    }
  }
}

#endif
