#ifndef DMTPC_MC_MCTPC_TOY_GENERATOR_HH
#define DMTPC_MC_MCTPC_TOY_GENERATOR_HH

#include "Generator.hh"

namespace dmtpc
{
  namespace mc
  {

    namespace mctpc
    {
      class ParticleDistribution; 

      class ToyGenerator : public Generator
      {

        public:
          ToyGenerator(ParticleDistribution * pd, int Z = 0) {dist = pd; species = Z; quenching = 0; }
          void setParticleDistribution(ParticleDistribution * pd) { dist = pd; } 
          void setQuenching(double (*q)(double)) { quenching = q; } 

        protected: 
	  virtual int doGenerate(dmtpc::analysis::MCTruth * truth);
          int species; 
          ParticleDistribution * dist; 
          
        private:
          double (*quenching) (double); 

      }; 


    }
  }
}




#endif
