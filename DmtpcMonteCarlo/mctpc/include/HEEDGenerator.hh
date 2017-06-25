#ifndef DMTPC_MC_MCTPC_HEED_GENERATOR_HH
#define DMTPC_MC_MCTPC_HEED_GENERATOR_HH

#include "TVector3.h"
#include "Generator.hh"

namespace Garfield
{

  class Sensor; 
  class Medium;
  class TrackHeed; 

}

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
      class ScalarDistribution;
      class ParticleDistribution; 
      class HEEDGenerator : public Generator 
      {

        public: 
          HEEDGenerator(Garfield::Sensor * sensor); 
          HEEDGenerator(Garfield::Medium * m, const TVector3 * center = 0, const TVector3 * halflengths = 0, const TVector3 * efield = 0); 
          virtual ~HEEDGenerator(); 
          void setE(double E_in_keV){ E = E_in_keV; }
          void setTime(double t) { time = t; } 
          void setPosition(TVector3 pos) { v = pos;}
          void setDirection(TVector3 dir) { vx = dir;}
          void setProjectileDirection(TVector3 dir) {pvx = dir ;}
          void setSourceDirection(TVector3 dir){ svx = dir; } 
          void setParticleDistribution(ParticleDistribution * pd) { dist = pd; } 
 

        private: 

	  int doGenerate(dmtpc::analysis::MCTruth * truth); 
          void * delete_me; //opaque on purpose to avoid needing knowledge of it at interface level 
          Garfield::TrackHeed * t; 
          const Garfield::Sensor * sensor; 

          TVector3 v; 
          TVector3 vx; 
          TVector3 pvx; 
          TVector3 svx; 
          double time; 
          double E; 
          ScalarDistribution *ndist; 
          ParticleDistribution * dist; 
      }; 
    }
  }
}


#endif
