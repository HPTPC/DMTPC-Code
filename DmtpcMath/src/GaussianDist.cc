/** \file GaussianDist.cc
\brief Implementation of dmtpc::math::dist::Gaussian
\author Jeremy Lopez
*/
#include "GaussianDist.hh"
#include "TMath.h"
#include <cmath>

using dmtpc::math::dist::Gaussian;

Gaussian::Gaussian()
{
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

Gaussian::Gaussian(double m, double s)
{
  fMean = m;
  fSigma = s;
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
const double INV_SQRT_TWOPI = 0.3989422804014326779399460599343819L;
const double INV_SQRT_TWO =   0.7071067811865475244008443621048490L;
const double SQRT_TWO = 1.4142135623730950488016887242096981L;

double
Gaussian::pdf(double x) const
{
  return exp(- (x-fMean)*(x-fMean)/(2*fSigma*fSigma) ) * INV_SQRT_TWOPI / fSigma;
}

double
Gaussian::cdf(double x) const
{return 0.5 + 0.5 * erf( (x-fMean)*INV_SQRT_TWO/(fSigma) );}

double
Gaussian::inverseCDF(double x) const
{
  return fMean+SQRT_TWO*fSigma*TMath::ErfInverse(2*x-1);
}

