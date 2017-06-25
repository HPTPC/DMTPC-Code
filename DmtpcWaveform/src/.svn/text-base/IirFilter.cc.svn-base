/** \file IirFilter.cc
\brief Implementation of dmtpc::waveform::IirFilter class
\author Jeremy Lopez
*/

#include "IirFilter.hh"
#include "FirFilter.hh"
#include <vector>
#include <cmath>

#ifndef M_PI
  #define M_PI 3.14159265358979323846L
#endif
namespace{
 static const double PI = M_PI;
}

using namespace std;
//Constructors



template<class REALTYPE>
dmtpc::waveform::IirFilter<REALTYPE>::IirFilter() : FirFilter<REALTYPE>::FirFilter(), fIIR(0), fInitialValues(0)
{}

template<class REALTYPE>
dmtpc::waveform::IirFilter<REALTYPE>::IirFilter(size_t nfir, const float* fir, size_t niir, const float* iir, const float* init) :
  FirFilter<REALTYPE>::FirFilter(nfir,fir), fIIR(niir), fInitialValues(niir-1)
{
  setIirCoeffs(niir,iir);
  setInitialValues(niir-1,init);
}

template<class REALTYPE>
dmtpc::waveform::IirFilter<REALTYPE>::IirFilter(size_t nfir, const double* fir, size_t niir, const double* iir, const double* init) :
  FirFilter<REALTYPE>::FirFilter(nfir,fir), fIIR(niir), fInitialValues(niir-1)
{
  setIirCoeffs(niir,iir);
  setInitialValues(niir-1,init);
}

template <class REALTYPE>
dmtpc::waveform::IirFilter<REALTYPE>::IirFilter(const vector<float>& fir, const vector<float>& iir, const vector<float>& init) :
  FirFilter<REALTYPE>::FirFilter(fir)
{
  setIirCoeffs(iir);
  setInitialValues(init);
}

template <class REALTYPE>
dmtpc::waveform::IirFilter<REALTYPE>::IirFilter(const vector<double>& fir, const vector<double>& iir, const vector<double>& init) :
  FirFilter<REALTYPE>::FirFilter(fir)
{
  setIirCoeffs(iir);
  setInitialValues(init);
}

//Set filter info

template <class REALTYPE>
void
dmtpc::waveform::IirFilter<REALTYPE>::setIirCoeffs(size_t n, const float* iir)
{
  fIIR.resize(n);
  fInitialValues.resize(n,0);
  if (!iir) return;
  for (size_t i = 0 ; i < n ; i++)
    fIIR[i] = iir[i];
}

template <class REALTYPE>
void
dmtpc::waveform::IirFilter<REALTYPE>::setIirCoeffs(size_t n, const double* iir)
{
  fIIR.resize(n);
  fInitialValues.resize(n,0);
  if (!iir) return;
  for (size_t i = 0 ; i < n ; i++)
    fIIR[i] = iir[i];
}

template <class REALTYPE>
void
dmtpc::waveform::IirFilter<REALTYPE>::setInitialValues(size_t n, const float* init)
{
  size_t size = n > fIIR.size() ? n : fIIR.size();
  fInitialValues.resize(size,0);
  if (!init) return;
  for (size_t i = 0 ; i < n ; i++)
    fInitialValues[i] = init[i];
}

template <class REALTYPE>
void
dmtpc::waveform::IirFilter<REALTYPE>::setInitialValues(size_t n, const double* init)
{
  size_t size = n > fIIR.size() ? n : fIIR.size();
  fInitialValues.resize(size,0);
  if (!init) return;
  for (size_t i = 0 ; i < n ; i++)
    fInitialValues[i] = init[i];
}

//Normalize coefficients
template <class REALTYPE>
void
dmtpc::waveform::IirFilter<REALTYPE>::normalizeFir()
{
  double total = 0;
  for (size_t i = 0 ; i < fFIR.size(); i++)
    total+=fFIR[i];
  for (size_t i = 0 ; i < fFIR.size(); i++)
    fFIR[i]/=total;
}

template <class REALTYPE>
void
dmtpc::waveform::IirFilter<REALTYPE>::normalizeIir()
{
  double value = fIIR[0];
  for (size_t i = 0; i < fFIR.size(); i++) fFIR[i]/=value;
  for (size_t i = 0; i < fIIR.size(); i++) fIIR[i]/=value;
}

//Response functions

template <class REALTYPE>
void
dmtpc::waveform::IirFilter<REALTYPE>::transferFunction(double zr,double zi,double& ztr, double& zti) const
{
  ztr=0, zti=0;
  if (zr==0&&zi==0) return;
  double norm = sqrt(zr*zr+zi*zi);
  double phi = atan2(zi,zr);
  double Nr=0,Ni=0;
  double power = 1;
  for (int i = 0 ; i<(int) fFIR.size(); i++)
  {
    Nr += fFIR[fFIR.size()-1-i]/power*cos(i*phi);
    Ni -= fFIR[fFIR.size()-1-i]/power*sin(i*phi);
    power *= norm;
  }
  double Dr=0,Di=0;
  power = 1;
  for (int i = 0; i<(int) fIIR.size();i++)
  {
    Dr += fIIR[i]/power*cos(i*phi);
    Di -= fIIR[i]/power*sin(i*phi);
    power *= norm;
  }

  ztr = (Dr*Nr+Di*Ni)/sqrt(Dr*Dr+Di*Di);
  zti = (Ni*Dr-Nr*Di)/sqrt(Dr*Dr+Di*Di);

}

template <class REALTYPE>
void
dmtpc::waveform::IirFilter<REALTYPE>::frequencyResponse(double w, double& re,double& im) const
{
  double Nr=0,Ni=0, Dr=0,Di=0;
  for (int i =0; i< (int) fFIR.size(); i++)
  {
    Nr += fFIR[fFIR.size()-1-i] * cos(2*::PI*i*w);
    Ni -= fFIR[fFIR.size()-1-i] * sin(2*::PI*i*w);
  }
  Nr /= fIIR[0];
  Ni /= fIIR[0];
  for (int i = 0; i < (int) fIIR.size(); i++)
  {
    Dr += fIIR[i]*cos(2*::PI*i*w);
    Di -= fIIR[i]*sin(2*::PI*i*w);
  }
  Dr /= fIIR[0];
  Di /= fIIR[0];

  re = (Nr*Dr + Ni*Di)/sqrt(Dr*Dr+Di*Di);
  im = (Ni*Dr - Nr*Di)/sqrt(Dr*Dr+Di*Di);
}

template<class REALTYPE>
double
dmtpc::waveform::IirFilter<REALTYPE>::powerAttenuation(double w) const
{
  double re,im;
  frequencyResponse(w,re,im);
  return re*re+im*im;
}

template<class REALTYPE>
double
dmtpc::waveform::IirFilter<REALTYPE>::magnitudeAttenuation(double w) const
{return sqrt(powerAttenuation(w));}

template<class REALTYPE>
double
dmtpc::waveform::IirFilter<REALTYPE>::phaseShift(double w) const
{
  double re,im;
  frequencyResponse(w,re,im);
  return atan2(im,re);
}

template<class REALTYPE>
double
dmtpc::waveform::IirFilter<REALTYPE>::phaseDelay(double w) const
{
  if (w==0) return 0;
  return -phaseShift(w)*2*::PI/w;
}

template<class REALTYPE>
double
dmtpc::waveform::IirFilter<REALTYPE>::groupDelay(double w) const
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
  double Ci=0,Cin=0,Si=0,Sin=0;
  for (unsigned int i = 0; i < fIIR.size(); i++)
  {
    Ci += fIIR[i]*cos(2*::PI*w);
    Cin += i*fIIR[i]*cos(2*::PI*w);
    Si -= fIIR[i]*sin(2*::PI*w);
    Sin -= i*fIIR[i]*sin(2*::PI*w);
  }
  double Rf = Sf/Cf;
  double Ri = Si/Ci;

  return (Cin/Ci + Ri * Sin/Ci)/(1+Ri*Ri) - (Cfn/Cf+Rf*Sfn/Cf)/(1+Rf*Rf);
}

//Apply the filter

#define RUN_NO_OUTPUT(inputType) \
  template <class REALTYPE> \
  void \
  dmtpc::waveform::IirFilter<REALTYPE>::apply(size_t n, const inputType *in,bool reverse) \
  { \
    size_t outsize = n + 1 - fFIR.size(); \
    if (outsize<1) \
    {  \
      fOutput.resize(0); \
      return; \
    }else if (outsize!=fOutput.size()) \
      fOutput.resize(outsize); \
    if (!reverse){ \
      for (size_t i = 0; i < outsize; i++) \
      { \
        fOutput[i] = 0; \
        /*FIR part */\
        for (size_t j = 0; j < fFIR.size(); j++) \
          fOutput[i] += fFIR[j] * in[i+j]; \
        /*IIR part*/ \
        for (size_t j = 1; j <fIIR.size(); j++) \
        { \
          double x = fIIR[j]; \
          x*= j>i? fInitialValues[fIIR.size() - (j-i)]: fOutput[i-j]; \
          fOutput[i] -= x; \
        } \
        fOutput[i] /= fIIR[0]; \
      } \
    } else {/*run in reverse direction*/ \
      int outmax = outsize-1, inmax = n-1; \
      for (size_t i = 0; i < outsize; i++) \
      { \
        fOutput[outmax-i] = 0;\
        /*FIR part*/\
        for (size_t j = 0; j < fFIR.size(); j++)\
          fOutput[outmax-i] += fFIR[j] * in[inmax-i-j];\
        /*IIR part*/\
        for (size_t j = 1; j <fIIR.size(); j++)\
        {\
          double x = fIIR[j];\
          x*= j>i? fInitialValues[fIIR.size() - (j-i)]: fOutput[outmax-i+j];\
          fOutput[outmax-i] -= x;\
        }\
        fOutput[outmax-i] /= fIIR[0];\
      }\
      \
    }/*reverse direction*/ \
  }

RUN_NO_OUTPUT(float)
RUN_NO_OUTPUT(double)
RUN_NO_OUTPUT(char)
RUN_NO_OUTPUT(short)
RUN_NO_OUTPUT(int)
RUN_NO_OUTPUT(long)
RUN_NO_OUTPUT(unsigned char)
RUN_NO_OUTPUT(unsigned short)
RUN_NO_OUTPUT(unsigned int)
RUN_NO_OUTPUT(unsigned long)

#define RUN_WITH_OUTPUT(inputType) \
  template <class REALTYPE> \
  void \
  dmtpc::waveform::IirFilter<REALTYPE>::apply(size_t n, const inputType *in,REALTYPE* out, bool reverse) const \
  { \
    size_t outsize = n + 1 - fFIR.size(); \
    if (!reverse){ \
      for (size_t i = 0; i < outsize; i++) \
      { \
        out[i] = 0; \
        /*FIR part */\
        for (size_t j = 0; j < fFIR.size(); j++) \
          out[i] += fFIR[j] * in[i+j]; \
        /*IIR part*/ \
        for (size_t j = 1; j <fIIR.size(); j++) \
        { \
          double x = fIIR[j]; \
          x*= j>i? fInitialValues[fIIR.size() - (j-i)]: out[i-j]; \
          out[i] -= x; \
        } \
        out[i] /= fIIR[0]; \
      } \
    } else {/*run in reverse direction*/ \
      int outmax = outsize-1, inmax = n-1; \
      for (size_t i = 0; i < outsize; i++) \
      { \
        out[outmax-i] = 0;\
        /*FIR part*/\
        for (size_t j = 0; j < fFIR.size(); j++)\
          out[outmax-i] += fFIR[j] * in[inmax-i-j];\
        /*IIR part*/\
        for (size_t j = 1; j <fIIR.size(); j++)\
        {\
          double x = fIIR[j];\
          x*= j>i? fInitialValues[fIIR.size() - (j-i)]: out[outmax-i+j];\
          out[outmax-i] -= x;\
        }\
        out[outmax-i] /= fIIR[0];\
      }\
      \
    }/*reverse direction*/ \
  }

RUN_WITH_OUTPUT(float)
RUN_WITH_OUTPUT(double)
RUN_WITH_OUTPUT(char)
RUN_WITH_OUTPUT(short)
RUN_WITH_OUTPUT(int)
RUN_WITH_OUTPUT(long)
RUN_WITH_OUTPUT(unsigned char)
RUN_WITH_OUTPUT(unsigned short)
RUN_WITH_OUTPUT(unsigned int)
RUN_WITH_OUTPUT(unsigned long)


//Force compilation of double, float, and long double versions
namespace dmtpc{
  namespace waveform{
    template class IirFilter<double>;
    template class IirFilter<float>;
    template class IirFilter<long double>;
  }
}

