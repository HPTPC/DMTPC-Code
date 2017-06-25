#include "RNG.hh"
#include "TRandom3.h"


static TRandom3 rgen(0);

TRandom * dmtpc::mc::wimpspectrum::rng::rng()
{
  return &rgen;
}

void dmtpc::mc::wimpspectrum::rng::setSeed(long seed)
{
  rgen.SetSeed(seed); 
}


