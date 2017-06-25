#ifndef DMTPC_MATH_MULTINORMAL_SAMPLER_HH
#define DMTPC_MATH_MULTINORMAL_SAMPLER_HH
#include "TDecompChol.h"
#include "TRandom3.h" 
#include "TMatrixDSym.h" 
#include "TVectorD.h" 

namespace dmtpc
{
  namespace math
  {

    class MultiNormalSampler
    {

      public:
        MultiNormalSampler (unsigned ndim = 2, unsigned nsamples = 1000, long seed = 0); 


        bool sample(const double * means, const double * covar); 
        bool sample(const TVectorD * means, const TMatrixDSym * c); 

        const double * getPoints() const; 
        void fillPoints(double * n)  const; 

      private:
        bool sample(); 
        TDecompChol chol; 
        TMatrixDSym C; 
        TVectorD z; 
        TVectorD u; 
        TVectorD v; 
        TRandom3 rand; 
        unsigned nsamples; 
        unsigned ndim;

    };
  }
}

#endif
