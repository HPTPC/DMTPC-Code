/** \file PoissonDist.cc
\brief Implementation of dmtpc::math::dist::Poisson
\author Jeremy Lopez
*/
#include "PoissonDist.hh"
#include "TMath.h"
#include <cmath>

using dmtpc::math::dist::Poisson;

Poisson::Poisson()
{
  fMean = 1;
  fIsDiscrete = true;
  fHasMean = true;
  fHasMode = true;
  fHasMedian = true;
  fHasVar = true;
  fHasSkew = true;
  fHasKurt = true;
  fHasCDF = true;
  fHasInvCDF = false;
}


Poisson::Poisson(double m)
{
  fMean = m;
  fIsDiscrete = true;
  fHasMean = true;
  fHasMode = true;
  fHasMedian = true;
  fHasVar = true;
  fHasSkew = true;
  fHasKurt = true;
  fHasCDF = true;
  fHasInvCDF = false;
}

double
Poisson::pdf(int x) const
{
  return exp(-fMean) * pow(fMean,x)/TMath::Factorial(x);
}

double
Poisson::cdf(int x) const
{
  double sum = 0;
  for (int i = 0 ; i < x; i++)
  {
    sum+=pow(fMean,i)/TMath::Factorial(i);
  }

  sum*=exp(-fMean);
  return sum;
}

double
Poisson::mean() const {return fMean;}

double
Poisson::median() const {return floor(fMean+ 1./3 - 0.02/fMean);}

double
Poisson::mode() const {return floor(fMean);}

double
Poisson::variance() const {return fMean;}

double
Poisson::stddev() const {return sqrt(fMean);}

double
Poisson::skewness() const {return 1./sqrt(fMean);}

double
Poisson::kurtosis() const {return 1./fMean;}

double 
Poisson::inverseCDF(double x __attribute__((unused)) ) const {return -1;}

