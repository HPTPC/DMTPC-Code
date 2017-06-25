#ifndef DMTPC_MC_MCTPC_TRIM_GENERATOR_HH
#define DMTPC_MC_MCTPC_TRIM_GENERATOR_HH

#include "Generator.hh"
#include "TVector3.h" 

namespace dmtpc
{
  namespace mc
  {
    namespace retrim
    {
      class TrackMaker; 
      class IonizationModel; 
    }

    namespace mctpc
    {
        
      class TRIMGenerator : public Generator
      {

        public:
          TRIMGenerator(dmtpc::mc::retrim::TrackMaker * track_maker, dmtpc::mc::retrim::IonizationModel * ioniz_model); 
          void setPosition(TVector3 pos);
          void setDirection(TVector3 dir);
          void setProjectileDirection(TVector3 dir);
          void setCurrentTrack(int i) { current_track = i; } 


        protected: 
	  virtual int doGenerate(dmtpc::analysis::MCTruth * truth);
          dmtpc::mc::retrim::TrackMaker * trim; 
          dmtpc::mc::retrim::IonizationModel * ioniz; 
          int current_track; 
          TVector3 vx; 
          TVector3 v; 
          TVector3 pvx; 
          int truth_offset; 
      }; 


    }
  }
}



#endif
