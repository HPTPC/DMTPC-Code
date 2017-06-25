#ifndef DMTPC_MONTECARLO_AMP_SOLVERELMER_HH
#define DMTPC_MONTECARLO_AMP_SOLVERELMER_HH

#include "Solver.hh" 

namespace dmtpc
{
namespace mc
{
namespace amp
{


  class SolverElmer : public Solver 
  {

    public:

      SolverElmer(const char * elmerpath = 0); 
      virtual ~SolverElmer(); 
      virtual int solve(const Model * model); 
      virtual int loadSolution(const Model * model); 
      int makeFieldSif(const Model * m); 
      int makeWeightingSifs(const Model * m); 
      int makeDielectrics(const Model *m);
      bool solved(const Model * m) const; 

    private: 
      char * elmerpath; 
  }; 

}}}
#endif


