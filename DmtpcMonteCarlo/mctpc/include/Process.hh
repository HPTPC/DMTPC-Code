#ifndef DMTPC_MC_MCTPC_PROCESS_HH
#define DMTPC_MC_MCTPC_PROCESS_HH

#include <vector>
#include "Particle.hh" 
#include "Signal.hh" 

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
      class Process
      {

        public:
          /* for some reason cint fucks up if you don't include the namespace here... */ 
          int process(unsigned n, const dmtpc::mc::mctpc::Particle* in) { particles.clear();  return doProcess(n, in) ; }
	  virtual void prepareSignals(dmtpc::analysis::MCTruth * truth) { ; } 
          const std::vector<Particle>* getParticles() const { return &particles; } 
          const std::vector<Signal>* getSignals() const { return &signals; } 
          Signal getSignal(int i) const { return signals[i]; } 
          virtual void clearSignals() { signals.clear();  } 
          virtual ~Process() {;}

        protected: 
          virtual int doProcess(unsigned n,  const Particle* in) = 0; 
          std::vector<dmtpc::mc::mctpc::Particle> particles; 
          std::vector<Signal> signals; 
      }; 


    }
  }
}


#endif
