#ifndef DMTPC_MC_WIMPSPECTRUM_RNG_HH
#define DMTPC_MC_WIMPSPECTRUM_RNG_HH

class TRandom; 

namespace dmtpc
{
  namespace mc
  {
    namespace wimpspectrum
    {
      namespace rng
      {
        TRandom * rng(); 
        void setSeed(long seed); 
      }
    }

  }
}


#endif
