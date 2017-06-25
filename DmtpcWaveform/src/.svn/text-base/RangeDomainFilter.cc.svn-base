/** \file RangeDomainFilter.cc
\brief Implementation of dmtpc::waveform::RangeDomainFilter class
\author Jeremy Lopez
*/

#include "RangeDomainFilter.hh"
#include "FirFilter.hh"
#include <vector>
#include <cmath>
#include <cstdlib>

using namespace std;

template <class REALTYPE>
dmtpc::waveform::RangeDomainFilter<REALTYPE>::RangeDomainFilter() : 
  FirFilter<REALTYPE>::FirFilter(),  fRangeCoeffs(0) {}

template <class REALTYPE>
dmtpc::waveform::RangeDomainFilter<REALTYPE>::RangeDomainFilter(
  size_t nfir, const float* fir,size_t nrange, const float* range) :
  FirFilter<REALTYPE>::FirFilter(nfir,fir), fRangeCoeffs(nrange)
{
  for (size_t i = 0; i< nrange; i++)
    fRangeCoeffs[i] = range[i];
}

template <class REALTYPE>
dmtpc::waveform::RangeDomainFilter<REALTYPE>::RangeDomainFilter(
  size_t nfir, const double* fir,size_t nrange, const double* range) :
  FirFilter<REALTYPE>::FirFilter(nfir,fir), fRangeCoeffs(nrange)
{
  for (size_t i = 0; i< nrange; i++)
    fRangeCoeffs[i] = range[i];
}

template <class REALTYPE>
dmtpc::waveform::RangeDomainFilter<REALTYPE>::RangeDomainFilter(
  const std::vector<float>& fir, const std::vector<float>& range) :
  FirFilter<REALTYPE>::FirFilter(fir), fRangeCoeffs(range.size())
{
  for (size_t i = 0; i < range.size(); i++)
    fRangeCoeffs[i] = range[i];
}

template <class REALTYPE>
dmtpc::waveform::RangeDomainFilter<REALTYPE>::RangeDomainFilter(
  const std::vector<double>& fir, const std::vector<double>& range) :
  FirFilter<REALTYPE>::FirFilter(fir), fRangeCoeffs(range.size())
{
  for (size_t i = 0; i < range.size(); i++)
    fRangeCoeffs[i] = range[i];
}

template <class REALTYPE>
void
dmtpc::waveform::RangeDomainFilter<REALTYPE>::setRangeCoeffs(size_t n, const double* range)
{
  fRangeCoeffs.resize(n);
  for (size_t i = 0; i < n; i++)
    fRangeCoeffs[i] = range[i];
}
template <class REALTYPE>
void
dmtpc::waveform::RangeDomainFilter<REALTYPE>::setRangeCoeffs(size_t n, const float* range)
{
  fRangeCoeffs.resize(n);
  for (size_t i = 0; i < n; i++)
    fRangeCoeffs[i] = range[i];
}

#define RUN_NO_OUTPUT_FLT(inputType) \
  template <class REALTYPE> \
  void \
  dmtpc::waveform::RangeDomainFilter<REALTYPE>::apply(size_t n, const inputType *in,double diff) \
  { \
    int outsize = n + 1 - (int)fFIR.size(); \
    if (outsize<1) \
    { \
      fOutput.resize(0); \
      return; \
    }else if (outsize!=(int)fOutput.size()) \
      fOutput.resize(outsize); \
   \
    double norm,center; \
    long delta; \
    for (size_t i = 0; i < fOutput.size(); i++) \
    { \
      norm = 0; \
      center = in[i+ fFIR.size()/2]; \
      fOutput[i] = 0; \
      for (size_t j = 0; j < fFIR.size(); j++) \
      { \
        delta = lrint( fabs(in[i+j]-center)/diff ); \
        fOutput[i] += fFIR[j] * fRangeCoeffs[delta] * in[i+j]; \
        norm += fFIR[j] * fRangeCoeffs[delta]; \
      } \
      fOutput[i] /= norm; \
    } \
  } 

RUN_NO_OUTPUT_FLT(float)
RUN_NO_OUTPUT_FLT(double)

#define RUN_NO_OUTPUT_INT(inputType) \
  template <class REALTYPE> \
  void \
  dmtpc::waveform::RangeDomainFilter<REALTYPE>::apply(size_t n, const inputType *in) \
  { \
    int outsize = n + 1 - (int)fFIR.size(); \
    if (outsize<1) \
    { \
      fOutput.resize(0); \
      return; \
    }else if (outsize!=(int)fOutput.size()) \
      fOutput.resize(outsize); \
   \
    double norm; \
    long delta; \
    inputType center; \
    for (size_t i = 0; i < fOutput.size(); i++) \
    { \
      norm = 0; \
      center = in[i+ fFIR.size()/2]; \
      fOutput[i] = 0; \
      for (size_t j = 0; j < fFIR.size(); j++) \
      { \
        delta = labs((long) (in[i+j]-center) ); \
        fOutput[i] += fFIR[j] * fRangeCoeffs[delta] * in[i+j]; \
        norm += fFIR[j] * fRangeCoeffs[delta]; \
      } \
      fOutput[i] /= norm; \
    } \
  } 

RUN_NO_OUTPUT_INT(char)
RUN_NO_OUTPUT_INT(short)
RUN_NO_OUTPUT_INT(int)
RUN_NO_OUTPUT_INT(long)
RUN_NO_OUTPUT_INT(unsigned char)
RUN_NO_OUTPUT_INT(unsigned short)
RUN_NO_OUTPUT_INT(unsigned int)
RUN_NO_OUTPUT_INT(unsigned long)


#define RUN_OUTPUT_FLT(inputType) \
  template <class REALTYPE>   \
  void \
  dmtpc::waveform::RangeDomainFilter<REALTYPE>::apply(size_t n, const inputType *in,REALTYPE* out,double diff) const \
  { \
    size_t outsize = n + 1 - fFIR.size(); \
    double norm; \
    inputType center; \
    long delta; \
    for (size_t i = 0; i < outsize; i++) \
    { \
      norm = 0; \
      center = in[i+ fFIR.size()/2]; \
      out[i] = 0; \
      for (size_t j = 0; j < fFIR.size(); j++) \
      { \
        delta = lrint( fabs(in[i+j]-center)/diff ); \
        out[i] += fFIR[j] * fRangeCoeffs[delta] * in[i+j]; \
        norm += fFIR[j] * fRangeCoeffs[delta]; \
      } \
      out[i] /= norm; \
    } \
  } 

RUN_OUTPUT_FLT(float)
RUN_OUTPUT_FLT(double)

#define RUN_OUTPUT_INT(inputType) \
  template <class REALTYPE>   \
  void \
  dmtpc::waveform::RangeDomainFilter<REALTYPE>::apply(size_t n, const inputType *in,REALTYPE* out) const \
  { \
    size_t outsize = n + 1 - fFIR.size(); \
    double norm; \
    long center; \
    long diff; \
    for (size_t i = 0; i < outsize; i++) \
    { \
      norm = 0; \
      center = in[i+ fFIR.size()/2]; \
      out[i] = 0; \
      for (size_t j = 0; j < fFIR.size(); j++) \
      { \
        diff = labs(center - in[i+j]); \
        out[i] += fFIR[j] * fRangeCoeffs[diff] * in[i+j]; \
        norm += fFIR[j] * fRangeCoeffs[diff]; \
      } \
      out[i] /= norm; \
    } \
  }

RUN_OUTPUT_INT(char) 
RUN_OUTPUT_INT(short) 
RUN_OUTPUT_INT(int) 
RUN_OUTPUT_INT(long) 
RUN_OUTPUT_INT(unsigned char) 
RUN_OUTPUT_INT(unsigned short) 
RUN_OUTPUT_INT(unsigned int) 
RUN_OUTPUT_INT(unsigned long) 

namespace dmtpc{
  namespace waveform{

    template class RangeDomainFilter<float>;
    template class RangeDomainFilter<double>;
    template class RangeDomainFilter<long double>;
  }
}
