#ifndef DMTPC_MC_MCTPC_MULTI_TRIM_GENERATOR_HH
#define DMTPC_MC_MCTPC_MULTI_TRIM_GENERATOR_HH

#include "TRandom3.h"
#include "Generator.hh"
#include <vector>
#include "TH1.h"
#include "TVector3.h" 

namespace dmtpc
{
  namespace mc
  {
    namespace retrim
    {
      class TrackMaker; 
      class IonizationModel; 
      class TableReader; 
    }

    namespace mctpc
    {
      class ParticleDistribution; 

      class MultiTRIMGenerator : public Generator
      {

        public:
                     MultiTRIMGenerator(int ntrim, dmtpc::mc::retrim::TrackMaker ** track_maker, dmtpc::mc::retrim::IonizationModel * ioniz_model, int species = 0, double P = 60); 
          void setE(double E); 
          void setTime(double t) { time = t; } 
          void setPosition(TVector3 pos);
          void setDirection(TVector3 dir);
          void setProjectileDirection(TVector3 dir);
          void setSourceDirection(TVector3 dir);
          void setParticleDistribution(ParticleDistribution * pd) { dist = pd; } 
          void setCurrentTrack(int i) { current_track[current_trim]= i; } 
          void setSeed(long seed) { rand.SetSeed(seed); } 

          /** WARNING NEVER DELETES TRACK MAKER OR COLLISON READER **/
          static MultiTRIMGenerator * makeFromDir(const char * collision_dir, dmtpc::mc::retrim::IonizationModel * ioniz_model, int nsrimtables, const dmtpc::mc::retrim::TableReader** tables, double srim_fract = 0.98, int species = 0, double P = 60); 

        protected: 
	virtual int doGenerate(dmtpc::analysis::MCTruth * truth);
          std::vector<dmtpc::mc::retrim::TrackMaker *> trim; 
          ParticleDistribution * dist; 
          std::vector<int> current_track; 
          int current_trim; 
          TH1I energy_indices; 
          dmtpc::mc::retrim::IonizationModel * ioniz; 
          double time; 
          double pressure; 
          TVector3 vx; 
          TVector3 v; 
          TVector3 pvx; 
          TVector3 svx; 
          int species; 
          int truth_offset; 
          TRandom3 rand; 
      }; 


    }
  }
}



#endif
