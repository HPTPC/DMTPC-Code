#ifndef DMTPC_MONTECARLO_AMP_SOLVER_HH
#define DMTPC_MONTECARLO_AMP_SOLVER_HH


namespace Garfield
{
  class ComponentBase; 
}

namespace dmtpc
{
namespace mc
{
namespace amp
{

  class Model; 

  class Solver 
  {
    public:
      virtual int solve(const Model * model) = 0; 
      virtual int loadSolution(const Model * model) =0; 
      Garfield::ComponentBase * fieldMap() { return fm; } 
      virtual bool solved(const Model * m) const { return false; }  

    protected: 
      Garfield::ComponentBase * fm; 

  }; 


}}}
#endif


