/** \file CrystalBallDist.cc
\brief Implementation of dmtpc::math::dist::CrystalBall
\author Jeremy Lopez
*/
#include "CrystalBallDist.hh"
#include "TMath.h"
#include <cmath>

using dmtpc::math::dist::CrystalBall;

CrystalBall::CrystalBall()
{
  fMean = 1;
  fSigma = 1;
  fAlpha = 10;
  fN= 1;
  fIsDiscrete = false;
  fHasMean = false;
  fHasMode = false;
  fHasMedian = false;
  fHasVar = false;
  fHasSkew = false;
  fHasKurt = false;
  fHasCDF = false;
  fHasInvCDF = false;
}


CrystalBall::CrystalBall(double m,double s,double a,double n)
{
  fMean = m;
  fSigma = s;
  fAlpha = a;
  fN = n;
  fIsDiscrete = false;
  fHasMean = false;
  fHasMode = false;
  fHasMedian = false;
  fHasVar = false;
  fHasSkew = false;
  fHasKurt = false;
  fHasCDF = false;
  fHasInvCDF = false;
}


const double INV_SQRT_TWO =   0.7071067811865475244008443621048490L;
const double SQRT_PI_OVER_TWO = 1.2533141373155002512078826424055226L;

double
CrystalBall::pdf(double x) const
{
  double C = fN/fabs(fAlpha) * exp(-0.5*fAlpha*fAlpha)/(fN-1);
  double D = SQRT_PI_OVER_TWO * (1+erf(INV_SQRT_TWO*fabs(fAlpha))  );
  double N = 1./(fSigma*(C+D) );
  double y = (x - fMean)/fSigma;
  if (y > -fAlpha)
    return N*exp(-0.5*y*y );
  double A = pow(fN/fabs(fAlpha),fN)*exp( - 0.5*fAlpha*fAlpha);
  double B = fN/fabs(fAlpha) - fabs(fAlpha);
  return N * A * pow(B-y,-fN);
}


double
CrystalBall::cdf(double x __attribute__((unused)) ) const
{return -1;}

double
CrystalBall::inverseCDF(double x __attribute__((unused)) ) const
{return -1;}

double
CrystalBall::mean() const {return -1;}

double
CrystalBall::median() const {return -1;}

double
CrystalBall::mode() const {return -1;}

double
CrystalBall::variance() const {return -1;}

double
CrystalBall::stddev() const {return -1;}

double
CrystalBall::skewness() const {return -1;}

double
CrystalBall::kurtosis() const 
{return -1;}
