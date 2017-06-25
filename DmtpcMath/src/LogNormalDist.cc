/** \file LogNormalDist.cc
\brief Implementation of dmtpc::math::dist::LogNormal
\author Jeremy Lopez
*/
#include "LogNormalDist.hh"
#include "TMath.h"
#include <cmath>

using dmtpc::math::dist::LogNormal;

LogNormal::LogNormal()
{
  fMean = 1;
  fSigma = 1;
  fIsDiscrete = false;
  fHasMean = true;
  fHasMode = true;
  fHasMedian = true;
  fHasVar = true;
  fHasSkew = true;
  fHasKurt = true;
  fHasCDF = true;
  fHasInvCDF = true;
}


LogNormal::LogNormal(double m,double s)
{
  fMean = m;
  fSigma = s;
  fIsDiscrete = true;
  fHasMean = true;
  fHasMode = true;
  fHasMedian = true;
  fHasVar = true;
  fHasSkew = true;
  fHasKurt = true;
  fHasCDF = true;
  fHasInvCDF = true;
}

const double INV_SQRT_TWOPI = 0.3989422804014326779399460599343819L;
const double INV_SQRT_TWO =   0.7071067811865475244008443621048490L;
const double SQRT_TWO = 1.4142135623730950488016887242096981L;

double
LogNormal::pdf(double x) const
{
  if (x<=0) return 0;
  return INV_SQRT_TWOPI/(fSigma*x) * exp( - (log(x)-fMean)*(log(x)-fMean) / (2*fSigma*fSigma)); 
}

double
LogNormal::cdf(double x) const
{
  if (x<=0) return 0;
  return 0.5 + 0.5 * erf( INV_SQRT_TWO*(log(x) - fMean)/fSigma );
}

double
LogNormal::inverseCDF(double x) const
{
  double y = SQRT_TWO * TMath::ErfInverse(2*x-1);
  return exp(fMean + fSigma*y);
}

double
LogNormal::mean() const {return exp(fMean+0.5*fSigma*fSigma);}

double
LogNormal::median() const {return exp(fMean);}

double
LogNormal::mode() const {return exp(fMean-fSigma*fSigma);}

double
LogNormal::variance() const
{
  return (exp(fSigma*fSigma)-1)*exp(2*fMean+fSigma*fSigma);
}

double
LogNormal::stddev() const {return sqrt(variance());}

double
LogNormal::skewness() const
{
  double Ess = exp(fSigma*fSigma);
  return (Ess+2)*sqrt(Ess-1);
}

double
LogNormal::kurtosis() const
{
  double Ess = exp(fSigma*fSigma);
  return Ess*Ess*Ess*Ess + 2 * Ess*Ess*Ess + 3 * Ess*Ess - 6;
}
