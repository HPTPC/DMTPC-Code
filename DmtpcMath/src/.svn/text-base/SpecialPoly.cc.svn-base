/** \file SpecialPoly.cc
\brief Implementation of various special/orthogonal polynomial sequences
\author Jeremy Lopez
*/
#include "SpecialPoly.hh"
#include "MathConstants.hh"
#include "TMath.h"
#include <cmath>

namespace dmtpc
{
namespace math{

double
legendrePoly(double x, int n)
{
  if (n==0) return 1;
  else if (n==1) return x;

  double Pn_2 = 1,Pn_1 = x,Pn = x;
  int lastN = 1;
  while (lastN<n)
  {
    Pn = ( (2*lastN+1)*x*Pn_1 - lastN*Pn_2)/(lastN+1);
    lastN++;
    Pn_2 = Pn_1;
    Pn_1 = Pn;
  }
  return Pn;
}

void
legendrePoly(double x, int nmax,double* out)
{
  out[0] = 1;
  if (nmax==0) return;
  out[1] = x;
  if (nmax==1) return;

  double Pn_2 = 1,Pn_1 = x, Pn = x;
  int lastN = 1;
  while (lastN<nmax)
  {
    Pn = ( (2*lastN+1)*x*Pn_1 - lastN*Pn_2)/(lastN+1);
    lastN++;
    Pn_2 = Pn_1;
    Pn_1 = Pn;
    out[lastN] = Pn;
  }

}

double
legendreNorm(int n1, int n2)
{
  return n1==n2? 2./(2*n1+1) : 0;
}

double
assocLegendrePoly(double x, int l, int m)
{
  if (m==0) return legendrePoly(x,l);
  if (fabs(x)>= 1) return 0;
  
  //Double factorial with sign
  double fac = (l-1)%2==0?1:-1;
  for (int i = 2*(l-1)-1; i>0; i-=2)
    fac *= i;

  double xfac = sqrt(1-x*x);
  double PLL = fac*pow(xfac,l-1);
  double Plm_2 = -(2*l+1)*xfac*PLL;//P_{l}^{l}
  if (l==m) return Plm_2;
  if (l==-m)
  {
    fac = m%2==0 ? 1 : -1;
    fac *= TMath::Factorial(l+m)/TMath::Factorial(l-m);
    return Plm_2 * fac;
  }//l==-m
  double Plm_1 = x*(2*l+1)*PLL;//P_{l}^{l-1}
  
  if (l==m+1) return Plm_1;
  if (l==-m-1){
    fac = m%2==0 ? 1 : -1;
    fac *= TMath::Factorial(l+m)/TMath::Factorial(l-m);
    return Plm_1 * fac;
  }
  double Plm = Plm_1;
  //Recursion step
  for (int mm = l-1; mm==abs(m); mm--)
  {
    Plm = - (Plm_2 + 2*(mm+1)*x/xfac*Plm_1 )/ ( (l+mm+1.)*(l-mm) );
    Plm_2 = Plm_1;
    Plm_1 = Plm;
  }

  if (m<0)
  {
    fac = m%2==0 ? 1 : -1;
    fac *= TMath::Factorial(l+m)/TMath::Factorial(l-m);
    Plm *= fac;
  }
  return Plm;
}

double 
sphericalHarmonicLegendre(double theta, int l, int m)
{
  return assocLegendrePoly(cos(theta),l,m) *
         sqrt( 
           (2*l+1)/(4*PI) * 
           TMath::Factorial(l-m)/TMath::Factorial(l+m)
         );

}

double 
sphericalHarmonicSin(double theta,double phi,int l, int m)
{return INV_PI*sphericalHarmonicLegendre(theta,l,m)*sin(m*phi);}

double 
sphericalHarmonicCos(double theta,double phi,int l, int m)
{return INV_PI*sphericalHarmonicLegendre(theta,l,m)*cos(m*phi);}

double 
sphericalHarmonicNorm(int l1,int l2, int m1, int m2)
{
  return l1==l2&&m1==m2 ? 1. : 0.;
}

double
hermitePoly(double x, int n)
{
  if (n==0) return 1;
  else if (n==1) return x;

  double Pn_2 = 1,Pn_1 = 2*x,Pn = 2*x;
  int lastN = 1;
  while (lastN<n)
  {
    Pn = 2*x*Pn_1 - 2*lastN*Pn_2;
    lastN++;
    Pn_2 = Pn_1;
    Pn_1 = Pn;
  }
  return Pn;
}

void
hermitePoly(double x, int nmax,double* out)
{
  out[0] = 1;
  if (nmax==0) return;
  out[1] = 2*x;
  if (nmax==1) return;

  double Pn_2 = 1,Pn_1 = 2*x, Pn = 2*x;
  int lastN = 1;
  while (lastN<nmax)
  {
    Pn = 2*x*Pn_1 - 2*lastN*Pn_2;
    lastN++;
    Pn_2 = Pn_1;
    Pn_1 = Pn;
    out[lastN] = Pn;
  }

}

double
hermiteNorm(int n1, int n2)
{
  return n1==n2? SQRT_PI * pow(2,n1)*TMath::Factorial(n1) : 0;
}

double
laguerrePoly(double x, int n)
{
  if (n==0) return 1;
  else if (n==1) return 1-x;

  double Pn_2 = 1,Pn_1 = 1-x,Pn = 1-x;
  int lastN = 1;
  while (lastN<n)
  {
    Pn = ( (2*lastN+1-x)*Pn_1 - lastN*Pn_2)/(lastN+1);
    lastN++;
    Pn_2 = Pn_1;
    Pn_1 = Pn;
  }
  return Pn;
}

void
laguerrePoly(double x, int nmax,double* out)
{
  out[0] = 1;
  if (nmax==0) return;
  out[1] = 1-x;
  if (nmax==1) return;

  double Pn_2 = 1,Pn_1 = 1-x, Pn = 1-x;
  int lastN = 1;
  while (lastN<nmax)
  {
    Pn = ( (2*lastN+1-x)*Pn_1 - lastN*Pn_2)/(lastN+1);
    lastN++;
    Pn_2 = Pn_1;
    Pn_1 = Pn;
    out[lastN] = Pn;
  }

}

double
laguerreNorm(int n1, int n2)
{
  return n1==n2? 1 : 0;
}

double
chebyshevFirstPoly(double x, int n)
{
  if (n==0) return 1;
  else if (n==1) return x;

  double Pn_2 = 1,Pn_1 = x,Pn = x;
  int lastN = 1;
  while (lastN<n)
  {
    Pn = 2*x*Pn_1 - Pn_2;
    lastN++;
    Pn_2 = Pn_1;
    Pn_1 = Pn;
  }
  return Pn;
}

void
chebyshevFirstPoly(double x, int nmax,double* out)
{
  out[0] = 1;
  if (nmax==0) return;
  out[1] = x;
  if (nmax==1) return;

  double Pn_2 = 1,Pn_1 = x, Pn = x;
  int lastN = 1;
  while (lastN<nmax)
  {
    Pn = 2*x*Pn_1 - Pn_2;
    lastN++;
    Pn_2 = Pn_1;
    Pn_1 = Pn;
    out[lastN] = Pn;
  }

}

double
chebyshevFirstNorm(int n1, int n2)
{
  if (n1==0&&n2==0) return PI;
  return n1==n2? PI_2 : 0;
}


double
chebyshevSecondPoly(double x, int n)
{
  if (n==0) return 1;
  else if (n==1) return 2*x;

  double Pn_2 = 1,Pn_1 = 2*x,Pn = 2*x;
  int lastN = 1;
  while (lastN<n)
  {
    Pn = 2*x*Pn_1 - Pn_2;
    lastN++;
    Pn_2 = Pn_1;
    Pn_1 = Pn;
  }
  return Pn;
}

void
chebyshevSecondPoly(double x, int nmax,double* out)
{
  out[0] = 1;
  if (nmax==0) return;
  out[1] = 2*x;
  if (nmax==1) return;

  double Pn_2 = 1,Pn_1 = 2*x, Pn = 2*x;
  int lastN = 1;
  while (lastN<nmax)
  {
    Pn = 2*x*Pn_1 - Pn_2;
    lastN++;
    Pn_2 = Pn_1;
    Pn_1 = Pn;
    out[lastN] = Pn;
  }

}

double
chebyshevSecondNorm(int n1, int n2)
{
  return n1==n2? PI_2 : 0;
}




}//math
}//dmtpc






