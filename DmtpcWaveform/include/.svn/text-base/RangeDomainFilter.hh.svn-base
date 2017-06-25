/** \file RangeDomainFilter.hh
\brief Header for dmtpc::waveform::RangeDomainFilter class
\author Jeremy Lopez
*/
#ifndef RANGEDOMAINFILTER_HH
#define RANGEDOMAINFILTER_HH
#include "FirFilter.hh"
#include <vector>
namespace dmtpc{
namespace waveform{

/** \brief Class for filters that include an FIR filter and a filter on the difference between nearby values.

This is meant to be a generalization of methods such as the bilateral filter. More documentation to come.

*/
template <class REALTYPE>
class
RangeDomainFilter : public FirFilter<REALTYPE>
{
  public:
    RangeDomainFilter();
    RangeDomainFilter(size_t nfir, const float* fir, size_t nrange, const float* range);
    RangeDomainFilter(size_t nfir, const double* fir, size_t nrange, const double* range);
    RangeDomainFilter(const std::vector<float>& fir, const std::vector<float>& range);
    RangeDomainFilter(const std::vector<double>& fir, const std::vector<double>& range);
    virtual ~RangeDomainFilter(){}

    size_t rangeCoeffSize() const{return fRangeCoeffs.size();}

    void setRangeCoeffs( size_t n, const double* range);
    void setRangeCoeffs( size_t n, const float* range);
    void setRangeCoeffs( const std::vector<float>& range){return setRangeCoeffs(range.size(),&range[0]);}
    void setRangeCoeffs( const std::vector<double>& range){return setRangeCoeffs(range.size(),&range[0]);}

    void apply(size_t n, const double* in,double diff);
    void apply(size_t n, const float* in,double diff);
    void apply(size_t n, const double* in){return apply(n,in,1);}
    void apply(size_t n, const float* in){return apply(n,in,1);}
    void apply(size_t n, const char* in);
    void apply(size_t n, const short* in);
    void apply(size_t n, const int* in);
    void apply(size_t n, const long* in);
    void apply(size_t n, const unsigned char* in);
    void apply(size_t n, const unsigned short* in);
    void apply(size_t n, const unsigned int* in);
    void apply(size_t n, const unsigned long* in);

    void apply(const std::vector<double>& in,double diff){return apply(in.size(),&in[0],diff);}
    void apply(const std::vector<float>& in,double diff){return apply(in.size(),&in[0],diff);}
    void apply(const std::vector<double>& in){return apply(in,1);}
    void apply(const std::vector<float>& in){return apply(in,1);}
    void apply(const std::vector<char>& in){return apply(in.size(),&in[0]);}
    void apply(const std::vector<short>& in){return apply(in.size(),&in[0]);}
    void apply(const std::vector<int>& in){return apply(in.size(),&in[0]);}
    void apply(const std::vector<long>& in){return apply(in.size(),&in[0]);}
    void apply(const std::vector<unsigned char>& in){return apply(in.size(),&in[0]);}
    void apply(const std::vector<unsigned short>& in){return apply(in.size(),&in[0]);}
    void apply(const std::vector<unsigned int>& in){return apply(in.size(),&in[0]);}
    void apply(const std::vector<unsigned long>& in){return apply(in.size(),&in[0]);}

    void apply(size_t n, const double* in,REALTYPE* out,double diff) const;
    void apply(size_t n, const float* in,REALTYPE* out,double diff) const;
    void apply(size_t n, const double* in,REALTYPE* out) const {return apply(n,in,out,1);} 
    void apply(size_t n, const float* in,REALTYPE* out) const {return apply(n,in,out,1);}
    void apply(size_t n, const char* in,REALTYPE* out) const;
    void apply(size_t n, const short* in,REALTYPE* out) const;
    void apply(size_t n, const int* in,REALTYPE* out) const;
    void apply(size_t n, const long* in,REALTYPE* out) const;
    void apply(size_t n, const unsigned char* in,REALTYPE* out) const;
    void apply(size_t n, const unsigned short* in,REALTYPE* out) const;
    void apply(size_t n, const unsigned int* in, REALTYPE* out) const;
    void apply(size_t n, const unsigned long* in, REALTYPE* out) const;

    void apply(const std::vector<double>& in,std::vector<REALTYPE>& out,double diff) const{return apply(in.size(),&in[0],&out[0],diff);}
    void apply(const std::vector<float>& in,std::vector<REALTYPE>& out,double diff) const{return apply(in.size(),&in[0],&out[0],diff);}
    void apply(const std::vector<double>& in,std::vector<REALTYPE>& out) const {return apply(in,out,1);}
    void apply(const std::vector<float>& in,std::vector<REALTYPE>& out) const {return apply(in,out,1);}
    void apply(const std::vector<char>& in,std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}
    void apply(const std::vector<short>& in,std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}
    void apply(const std::vector<int>& in,std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}
    void apply(const std::vector<long>& in,std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}
    void apply(const std::vector<unsigned char>& in,std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}
    void apply(const std::vector<unsigned short>& in,std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}
    void apply(const std::vector<unsigned int>& in,std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}
    void apply(const std::vector<unsigned long>& in,std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}


    const std::vector<REALTYPE>& rangeCoeffs() const{return fRangeCoeffs;}
    std::vector<REALTYPE>& rangeCoeffs(){return fRangeCoeffs;}

  protected:
    using FirFilter<REALTYPE>::fFIR;
    using FirFilter<REALTYPE>::fOutput;
    std::vector<REALTYPE> fRangeCoeffs;

};
}
}
#endif
