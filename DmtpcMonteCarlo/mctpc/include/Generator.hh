#ifndef DMTPC_MC_MCTPC_GENERATOR_HH
#define DMTPC_MC_MCTPC_GENERATOR_HH

#include <vector>
#include <utility>
#include "Particle.hh" 
#include "MCTruth.hh"

namespace dmtpc
{
  namespace mc
  {
    namespace mctpc
    {
      class Generator
      {

        public:
	int generate(dmtpc::analysis::MCTruth * truth) {particles.clear(); return doGenerate( truth); } 
          const std::vector<Particle> *getParticles() const { return &particles; } 
          virtual ~Generator() {;}


        protected: 
	virtual int doGenerate(dmtpc::analysis::MCTruth * truth) = 0; 
          std::vector<Particle> particles; 
      }; 


    }
  }
}


#endif
