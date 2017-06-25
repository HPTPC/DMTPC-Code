/** \file FirFilter.cc
\brief Implementation of dmtpc::waveform::FirFilter class
\author Jeremy Lopez
*/
#include "FirFilter.hh"

#include <cmath>
#ifndef M_PI
  #define M_PI 3.14159265358979323846L
#endif



namespace
{
  const static long double PI = M_PI;
}

namespace dmtpc
{

template <class REALTYPE>
waveform::FirFilter<REALTYPE>::FirFilter(size_t n,const double* kernel) : fFIR(n)
{
  if (kernel!=0)
    for (size_t i = 0; i < n; i++)
      fFIR[i] = kernel[i];
}

template <class REALTYPE>
waveform::FirFilter<REALTYPE>::FirFilter(const std::vector<double>& kernel) : fFIR(kernel.size())
{
  for (unsigned int i = 0; i<kernel.size(); i++)
    fFIR[i] = kernel[i];
}

template <class REALTYPE>
waveform::FirFilter<REALTYPE>::FirFilter(size_t n,const float* kernel) : fFIR(n)
{
  if (kernel!=0)
    for (size_t i = 0; i < n; i++)
      fFIR[i] = kernel[i];
}

template <class REALTYPE>
waveform::FirFilter<REALTYPE>::FirFilter(const std::vector<float>& kernel) : fFIR(kernel.size())
{
  for (unsigned int i = 0; i<kernel.size(); i++)
    fFIR[i] = kernel[i];
}

template <class REALTYPE>
void
waveform::FirFilter<REALTYPE>::setFirCoeffs(size_t n, const float* k)
{
  fFIR.resize(n);
  for (size_t i = 0; i<n; i++)
    fFIR[i] = k[i];
}
template <class REALTYPE>
void
waveform::FirFilter<REALTYPE>::setFirCoeffs(size_t n, const double* k)
{
  fFIR.resize(n);
  for (size_t i = 0; i<n; i++)
    fFIR[i] = k[i];
}

template <class REALTYPE>
void
waveform::FirFilter<REALTYPE>::normalize()
{
  double norm = 0;
  for (int i = 0 ; i<(int)fFIR.size(); i++)
    norm+=fFIR[i];
  for (int i = 0 ; i<(int)fFIR.size(); i++)
    fFIR[i]/=norm;
}

template <class REALTYPE>
void
waveform::FirFilter<REALTYPE>::transferFunction(double zr,double zi,double& ztr, double& zti) const
{
  ztr = 0;
  zti = 0;
  if (zr==0&&zi==0) return;
  double norm = sqrt(zr*zr+zi*zi);
  double phi = atan2(zi,zr);
  double power = 1;
  for (int i = 0 ; i<(int) fFIR.size(); i++)
  {
    ztr += fFIR[fFIR.size()-1-i]/power*cos(i*phi);
    zti -= fFIR[fFIR.size()-1-i]/power*sin(i*phi);
    power/=norm;
  }

}

template <class REALTYPE>
void
waveform::FirFilter<REALTYPE>::frequencyResponse(double w, double& re,double& im) const
{

  re = 0; 
  im = 0; 
  for (int i =0; i <(int) fFIR.size(); i++)
  {
    re += fFIR[fFIR.size()-1-i] * cos(2*::PI*i*w);
    im -= fFIR[fFIR.size()-1-i] * sin(2*::PI*i*w);

  }
}

template<class REALTYPE>
double
waveform::FirFilter<REALTYPE>::powerAttenuation(double w) const
{
  double r,i;
  frequencyResponse(w,r,i);
  return r*r+i*i;
}

template<class REALTYPE>
double
waveform::FirFilter<REALTYPE>::magnitudeAttenuation(double w) const
{return sqrt(powerAttenuation(w));}

template<class REALTYPE>
double
waveform::FirFilter<REALTYPE>::phaseShift(double w) const
{
  double r,i;
  frequencyResponse(w,r,i);
  return atan2(i,r);
}

template<class REALTYPE>
double
waveform::FirFilter<REALTYPE>::phaseDelay(double w) const
{
  if (w==0) return 0;
  return -phaseShift(w)/w;
}

template<class REALTYPE>
double
waveform::FirFilter<REALTYPE>::groupDelay(double w) const
{
  if (w==0) return 0;
  double Cf=0,Cfn=0,Sf=0,Sfn=0;
  for (unsigned int i = 0; i < fFIR.size(); i++)
  {
    Cf += fFIR[fFIR.size()-1-i]*cos(2*::PI*w);
    Cfn += i*fFIR[fFIR.size()-1-i]*cos(2*::PI*w);
    Sf -= fFIR[fFIR.size()-1-i]*sin(2*::PI*w);
    Sfn -= i*fFIR[fFIR.size()-1-i]*sin(2*::PI*w);
  }
  double Rf = Sf/Cf;
  return -(Cfn/Cf+Rf*Sfn/Cf)/(1+Rf*Rf);
}


#define COPY_OUTPUT(outType) \
  template <class REALTYPE>  \
  void \
  waveform::FirFilter<REALTYPE>::copyOutput(outType *out) const \
  { \
    for (size_t i = 0; i < outputSize(); i++) \
      out[i] = fOutput[i]; \
  }

COPY_OUTPUT(float)
COPY_OUTPUT(double)

//Run and save data to the internal output vector

#define RUN_NO_OUTPUT(inputType) \
  template <class REALTYPE> \
  void \
  waveform::FirFilter<REALTYPE>::apply(size_t n, const inputType *in) \
  { \
    size_t outsize = n + 1 - fFIR.size(); \
    if (outsize<1) \
    { \
      fOutput.resize(0); \
      return; \
    }else if (outsize!=fOutput.size()) \
      fOutput.resize(outsize); \
   \
    for (size_t i = 0; i < fOutput.size(); i++) \
    { \
      fOutput[i] = 0; \
      for (size_t j = 0; j < fFIR.size(); j++) \
      { \
        fOutput[i] += fFIR[j] * in[i+j]; \
      } \
    } \
  }

RUN_NO_OUTPUT(double)
RUN_NO_OUTPUT(float)
RUN_NO_OUTPUT(char)
RUN_NO_OUTPUT(short)
RUN_NO_OUTPUT(int)
RUN_NO_OUTPUT(long)
RUN_NO_OUTPUT(unsigned char)
RUN_NO_OUTPUT(unsigned short)
RUN_NO_OUTPUT(unsigned int)
RUN_NO_OUTPUT(unsigned long)

//Run copying data to an output array

#define RUN_OUTPUT(inputType) \
  template <class REALTYPE> \
  void \
  waveform::FirFilter<REALTYPE>::apply(size_t n, const inputType *in, REALTYPE *out) const \
  { \
    size_t outsize = n + 1 - fFIR.size(); \
     \
    for (size_t i = 0; i < outsize; i++) \
    { \
      out[i] = 0; \
      for (size_t j = 0; j < fFIR.size(); j++) \
      { \
        out[i] += fFIR[j] * in[i+j]; \
      } \
    } \
  }

RUN_OUTPUT(double)
RUN_OUTPUT(float)
RUN_OUTPUT(char)
RUN_OUTPUT(short)
RUN_OUTPUT(int)
RUN_OUTPUT(long)
RUN_OUTPUT(unsigned char)
RUN_OUTPUT(unsigned short)
RUN_OUTPUT(unsigned int)
RUN_OUTPUT(unsigned long)
}
//Force compilation of double, float, and long double versions
namespace dmtpc{
  namespace waveform{
  template class FirFilter<double>;
  template class FirFilter<float>;
  template class FirFilter<long double>;
  }
}
//}//dmtpc
