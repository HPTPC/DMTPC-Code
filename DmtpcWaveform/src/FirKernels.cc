/**\file FirKernels.hh
\brief Implementation of dmtpc::waveform::fir namespace
\author Jeremy Lopez
*/
#include "FirKernels.hh"

#include "TMatrixD.h"
#include "TVectorD.h"
#include "TMath.h"

#include <vector>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif
#define PI M_PI
using std::vector;
namespace dmtpc{
namespace waveform{
namespace fir{

vector<double>
raisedCosine(int halfwidth, double a0, double a1, double a2, double a3,double a4)
{
  vector<double> kernel(1+2*halfwidth);
  for (int i = -halfwidth; i<=halfwidth; i++)
  {
    kernel[i+halfwidth] = 
      a0 + a1 * cos(PI*i/halfwidth) + a2 * cos(2*PI*i/halfwidth)
         + a3 * cos(3*PI*i/halfwidth) + a4 * cos(4*PI*i/halfwidth);
  }
  return kernel;
}

vector<double> raisedCosine(int halfwidth, double a[])
{return raisedCosine(halfwidth,a[0],a[1],a[2],a[3],a[4]);}

vector<double> blackman(int halfwidth, double alpha)
{return raisedCosine(halfwidth,0.5*(1-alpha),0.5,0.5*alpha,0,0);}

vector<double> blackmanHarris(int halfwidth)
{return raisedCosine(halfwidth,0.35875,0.48829,0.14128,0.01168,0);}

vector<double> blackmanNutall(int halfwidth)
{return raisedCosine(halfwidth,0.3635819,0.4891775,0.1365995,0.0106511,0);}

vector<double>
bohman(int halfwidth)
{
  vector<double> kernel(1+2*halfwidth);
  for (int i = -halfwidth; i<=halfwidth; i++)
  {
    kernel[i+halfwidth] = (1-fabs(i)/halfwidth) * cos( PI*fabs(i)/halfwidth)
                 + 1./PI * sin(PI*fabs(i)/halfwidth);
  }
  return kernel;

}

vector<double>
breitWigner(int halfwidth, double gamma)
{
  vector<double> kernel(1+2*halfwidth,0);
  if (gamma==0)
  {
    kernel[halfwidth] = 1;
    return kernel;
  }
  for (int i = -halfwidth; i<=halfwidth; i++)
    kernel[i+halfwidth] = 1./(1 + (i*i)/gamma*gamma);
  
  return kernel;
}

vector<double> cosine(int halfwidth)
{return raisedCosine(halfwidth);}

vector<double>
exponential(int halfwidth, double x0)
{
  vector<double> kernel(1+2*halfwidth);
  for (int i = -halfwidth; i<=halfwidth; i++)
    kernel[i+halfwidth] = exp(-abs(i)/x0);
  return kernel;
}

vector<double>
fermiDirac(int halfwidth, double T, double mu)
{
  vector<double> kernel(1+2*halfwidth);
  for (int i = -halfwidth; i<=halfwidth; i++)
    kernel[i+halfwidth] = 
      1./(exp( (abs(i)-mu)/T )+1);
  return kernel;
}

vector<double> flattop(int halfwidth)
{return raisedCosine(halfwidth,1,1.93,1.29,0.388,0.032);}

vector<double>
gaussian(int halfwidth, double sigma)
{
  vector<double> kernel(1+2*halfwidth);
  for (int i = -halfwidth; i<=halfwidth; i++)
    kernel[i+halfwidth] = exp( -(i*i/(2*sigma*sigma) )  );
  return kernel;
}

vector<double> hamming(int halfwidth)
{return raisedCosine(halfwidth,0.54,0.46,0,0,0);}

vector<double> hann(int halfwidth)
{return raisedCosine(halfwidth,0.5,0.5,0,0,0);}

vector<double>
kaiser(int halfwidth, double a)
{
  vector<double> kernel(1+2*halfwidth);
  for (int i = -halfwidth; i<=halfwidth; i++)
    kernel[i+halfwidth] =
      TMath::BesselI0(PI*a*sqrt( 1-i*i*1.0/(halfwidth*halfwidth) ) );
  return kernel;
}

vector<double>
lanczos(int halfwidth)
{
  vector<double> kernel(1+2*halfwidth);
  for (int i = -halfwidth; i<=halfwidth; i++)
    kernel[i+halfwidth] = i==0?1:
      sin(i*PI/halfwidth) * halfwidth/(i*PI);
  return kernel;
}

vector<double> nutall(int halfwidth)
{return raisedCosine(halfwidth,0.355768,0.487396,0.144232,0.012604,0);}

vector<double>
rectangle(int halfwidth)
{
  vector<double> kernel(1+2*halfwidth,(1./(1+2*halfwidth)));
  return kernel;
}

vector<double>
savitzkyGolay(int halfwidth, int order, int nder)
{

  double derFact = TMath::Factorial(nder);
  TMatrixD A(2*halfwidth+1,order+1);
  for (int i = -halfwidth; i<=halfwidth; i++)
    for (int j = 0; j<=order; j++)
      A[i+halfwidth][j] = j==0 ? 1 : pow(1.0*i,1.0*j);

  TMatrixD At(order+1,2*halfwidth+1);
  At.Transpose(A);
  //Least square method: coefficients equal a smoothing function
  //and smoothed derivatives up to the given order.
  TMatrixD AinvAt = (At*A).Invert()*At;

  vector<double> kernel(1+2*halfwidth);
  for (int i = -halfwidth; i<=halfwidth; i++)
    kernel[i+halfwidth] = AinvAt[nder][i+halfwidth]*derFact;  
  return kernel;
}

vector<double> sinc(int halfwidth)
{return lanczos(halfwidth);}

vector<double>
sincSquared(int halfwidth)
{
  vector<double> kernel(1+2*halfwidth);
  for (int i = -halfwidth; i<=halfwidth; i++)
    kernel[i+halfwidth] = i==0?1:pow(sin(i*PI/halfwidth) * halfwidth/(i*PI),2.);

  return kernel;
}

vector<double>
triangle(int halfwidth)
{
  vector<double> kernel(1+2*halfwidth);
  for (int i = -halfwidth; i<=halfwidth; i++)
    kernel[i+halfwidth] = 1 - fabs(i)/halfwidth;
  return kernel;
}

vector<double>
exponentialConv(double x0, double cut)
{
  int N = (int)(-x0*log(cut));
  vector<double> kernel(1+2*N);
  for (int i = -N; i<=N; i++)
    kernel[i+N] = i==0?2*(1-exp(-0.5/x0)) :
      exp(-(abs(i)-0.5)/x0) - exp(-(abs(i)+0.5)/x0);
  return kernel;
}

vector<double>
gaussianConv(double sigma, double cut)
{
  int N = (int) (sigma*sqrt(-2*log(cut)));
  vector<double> kernel(1+2*N);
  for (int i = -N; i<=N; i++) kernel[i+N] =
    erf((abs(i)+0.5)/(sqrt(2)*sigma)) - erf((abs(i)-0.5)/(sqrt(2)*sigma));
  
  return kernel;
}

vector<double>
triangleConv(int halfwidth)
{
  vector<double> kernel(1+2*halfwidth);
  for (int i = -halfwidth; i<=halfwidth; i++)
    kernel[i+halfwidth] = i==0?1-0.25/(halfwidth+0.5) :
      1-fabs(i)/(halfwidth+0.5);
  return kernel;
}

}//end fir
}//end waveform
}//end dmtpc

