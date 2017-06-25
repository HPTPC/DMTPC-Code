/** \file InverseGaussianDist.cc
\brief Implementation of dmtpc::math::dist::InverseGaussian
\author Jeremy Lopez
*/
#include "InverseGaussianDist.hh"
#include "TMath.h"
#include <cmath>

using dmtpc::math::dist::InverseGaussian;

InverseGaussian::InverseGaussian()
{
  fMean = 1;
  fLambda = 1;
  fIsDiscrete = false;
  fHasMean = true;
  fHasMode = true;
  fHasMedian = false;
  fHasVar = true;
  fHasSkew = true;
  fHasKurt = true;
  fHasCDF = true;
  fHasInvCDF = false;
}


InverseGaussian::InverseGaussian(double m,double l)
{
  fMean = m;
  fLambda = l;
  fIsDiscrete = true;
  fHasMean = true;
  fHasMode = true;
  fHasMedian = false;
  fHasVar = true;
  fHasSkew = true;
  fHasKurt = true;
  fHasCDF = true;
  fHasInvCDF = false;
}

const double INV_SQRT_TWOPI = 0.3989422804014326779399460599343819L;
const double INV_SQRT_TWO =   0.7071067811865475244008443621048490L;
const double SQRT_TWO = 1.4142135623730950488016887242096981L;

double
InverseGaussian::pdf(double x) const
{
  if (x<=0) return 0;
  return INV_SQRT_TWOPI/(fLambda*x) * exp( - (log(x)-fMean)*(log(x)-fMean) / (2*fLambda*fLambda)); 
}

double
InverseGaussian::cdf(double x) const
{
  if (x<=0) return 0;
  return 0.5*(1 + erf(INV_SQRT_TWO*(sqrt(fLambda/x)*(x/fMean-1) ) ) )
        +0.5*exp(2*fLambda/fMean) * (1+erf(INV_SQRT_TWO*(-sqrt(fLambda/x)*(x/fMean+1) ) ) );
}

double
InverseGaussian::inverseCDF(double x __attribute__((unused)) ) const
{
  return -1;
}

double
InverseGaussian::mean() const {return fMean;}

double
InverseGaussian::median() const {return -1;}

double
InverseGaussian::mode() const
{
  double val = 1.5 * fMean/fLambda;
  return fMean* (sqrt(1+val*val) -val);
}

double
InverseGaussian::variance() const
{
  return fMean*fMean*fMean/fLambda;
}

double
InverseGaussian::stddev() const {return sqrt(variance());}

double
InverseGaussian::skewness() const
{
  return 3*sqrt(fMean/fLambda);
}

double
InverseGaussian::kurtosis() const
{
  return 15*fMean/fLambda;
}

