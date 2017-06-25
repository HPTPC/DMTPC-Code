/** \file IirFilter.hh
\brief Header file for dmtpc::waveform::IirFilter class
\author Jeremy Lopez
*/
#ifndef IIRFILTER_HH
#define IIRFILTER_HH

#include "FirFilter.hh"
#include <vector>

namespace dmtpc{

namespace waveform
{

/** \brief A class to perform infinite impulse response (IIR) filtering on digital signals.

The IIR filter is an extension of the FIR filter where the output depends not only on the input values, but also
on the previously-calculated output values. The name is because the response of the filter
to a unit impulse input lasts for an infinite amount of time for a non-trivial filter.
IIR filters can be designed to have much sharper frequency cutoffs than FIR filters of the same order and can
also implement functions such as integration. A tradeoff however, is that care must be taken when designing a filter
or else it can display unwanted behavior such as increasing rapidly toward infinity even with a reasonable input
array.

The IIR filter is defined by two sets of coefficients a and b so that given an input x[n],
the output y[n] is given by
\f[y[n] = \frac{1}{a_0} \left(\sum\limits_{i=0}^P b_i x[n-i] - \sum\limits_{j=1}^Q a_j y[n-j]\right).\f]
The b coefficients are just an FIR filter, while the a coefficients are the infinite impulse part.

The transfer function of an IIR filter is
\f[T(z) = \frac{\sum\limits_{i=0}^{P}b_k z^{-i}}{\sum\limits_{j=0}^Q a_j z^{-j}}.\f]

From this, we can see that the (complex-valued) frequency response is
\f[F(\omega) = T(e^{2\pi i \omega}) = \frac{\sum\limits_{n=0}^{P}b_k z^{-2\pi n i\omega}}{\sum\limits_{m=0}^Q a_j e^{-2\pi m i\omega}}. \f]

The magnitude of the frequency response is the amplitude attenuation, and its square is the power attenuation.
The phase of the frequency response is the phase shift of the filter.


*/
template <class REALTYPE>
class IirFilter : public FirFilter<REALTYPE>
{
  public:
    /** Default constructor */
    IirFilter();
    /** Constructor
        \param nfir the number of FIR coefficients
        \param fir the FIR coefficients
        \param niir the number of IIR coefficients
        \param iir the IIR coefficients
        \param init the initial step values
    */
    IirFilter(size_t nfir, const float* fir, size_t niir, const float* iir , const float* init);
    /** Constructor
        \param nfir the number of FIR coefficients
        \param fir the FIR coefficients
        \param niir the number of IIR coefficients
        \param iir the IIR coefficients
        \param init the initial step values
    */
    IirFilter(size_t nfir, const double* fir, size_t niir, const double* iir , const double* init);
    /** Constructor
        \param fir the FIR coefficients
        \param iir the IIR coefficients
        \param init the initial step values
    */
    IirFilter(const std::vector<float>& fir,const std::vector<float>& iir,const std::vector<float>& init);
    /** Constructor
        \param fir the FIR coefficients
        \param iir the IIR coefficients
        \param init the initial step values
    */
    IirFilter(const std::vector<double>& fir,const std::vector<double>& iir,const std::vector<double>& init);
    /** Destructor */
    virtual ~IirFilter(){}
    /** Set the IIR coefficients, ordered so that elements go backwards in time with increasing index 
        \param n the number of coefficients
        \param iir the coefficients
    */
    void setIirCoeffs(size_t n, const float* iir);
    /** Set the IIR coefficients, ordered so that elements go backwards in time with increasing index 
        \param n the number of coefficients
        \param iir the coefficients
    */
    void setIirCoeffs(size_t n, const double* iir);
    /** Set the IIR coefficients, ordered so that elements go backwards in time with increasing index 
        \param iir the coefficients
    */
    void setIirCoeffs(const std::vector<float>& iir){return setIirCoeffs(iir.size(),&iir[0]);}
    /** Set the IIR coefficients, ordered so that elements go backwards in time with increasing index 
        \param iir the coefficients
    */
    void setIirCoeffs(const std::vector<double>& iir){return setIirCoeffs(iir.size(),&iir[0]);}
    /** Set values to use in the initial IIR step. 
        \param n the number of values. 
        \param init the values
    */
    void setInitialValues(size_t n, const float* init);
    /** Set values to use in the initial IIR step. 
        \param n the number of values. 
        \param init the values
    */
    void setInitialValues(size_t n, const double* init);
    /** Set values to use in the initial IIR step. 
        \param init the values
    */
    void setInitialValues(const std::vector<float>& init){return setInitialValues(init.size(),&init[0]);}
    /** Set values to use in the initial IIR step. 
        \param init the values
    */
    void setInitialValues(const std::vector<double>& init){return setInitialValues(init.size(),&init[0]);}
    /** Normalize the FIR coefficients so that their sum is 1 */
    virtual void normalizeFir();
    /** Normalize the IIR coefficients by scaling so that the first coefficient is 1 */
    virtual void normalizeIir();
    /** Normalize both FIR and IIR coeffiecients */
    virtual void normalize(){normalizeIir(); normalizeFir();}
    /** Run the filter on a double array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      * \param reverse run in reverse direction (last to first)
      */
        virtual void apply(size_t n,const float *in,bool reverse=false);
    /** Run the filter on a double array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const double *in,bool reverse=false);
    /** Run the filter on a char array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const char *in,bool reverse=false);
    /** Run the filter on a short array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const short *in,bool reverese=false);
    /** Run the filter on an int array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const int *in,bool reverse=false);
    /** Run the filter on a long array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const long *in,bool reverse=false);
    /** Run the filter on an unsigned char array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const unsigned char *in,bool reverse=false);
    /** Run the filter on an unsigned short array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const unsigned short *in,bool reverse=false);
    /** Run the filter on an unsigned int array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const unsigned int *in,bool reverse=false);
    /** Run the filter on a unsigned long array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const unsigned long *in,bool reverse=false);
    /** Run the filter on a float vector and save to the output vector fOutput.
      * \param in the input vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<float>& in,bool reverse=false) {return apply(in.size(),&in[0]);}
    /** Run the filter on a double vector and save to the output vector fOutput.
      * \param in the input vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<double>& in,bool reverse=false) {return apply(in.size(),&in[0]);}
    /** Run the filter on a char vector and save to the output vector fOutput.
      * \param in the input vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<char>& in,bool reverse=false) {return apply(in.size(),&in[0]);}
    /** Run the filter on a short vector and save to the output vector fOutput.
      * \param in the input vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<short>& in,bool reverse=false) {return apply(in.size(),&in[0]);}
    /** Run the filter on a int vector and save to the output vector fOutput.
      * \param in the input vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<int>& in,bool reverse=false) {return apply(in.size(),&in[0]);}
    /** Run the filter on a long vector and save to the output vector fOutput.
      * \param in the input vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<long>& in,bool reverse=false) {return apply(in.size(),&in[0]);}
    /** Run the filter on an unsigned char vector and save to the output vector fOutput.
      * \param in the input vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<unsigned char>& in,bool reverse=false) {return apply(in.size(),&in[0]);}
    /** Run the filter on an unsigned short vector and save to the output vector fOutput.
      * \param in the input vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<unsigned short>& in,bool reverse=false) {return apply(in.size(),&in[0]);}
    /** Run the filter on an unsigned int vector and save to the output vector fOutput.
      * \param in the input vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<unsigned int>& in,bool reverse=false) {return apply(in.size(),&in[0]);}
    /** Run the filter on an unsigned long vector and save to the output vector fOutput.
      * \param in the input vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<unsigned long>& in,bool reverse=false) {return apply(in.size(),&in[0]);}
    /** Run the filter on a float array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const float *in,REALTYPE *out,bool reverse=false) const;
    /** Run the filter on a double array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const double *in,REALTYPE *out,bool reverse=false) const;
    /** Run the filter on a char array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const char *in,REALTYPE *out,bool reverse=false) const;
    /** Run the filter on a short array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const short *in,REALTYPE *out,bool reverse=false) const;
    /** Run the filter on an int array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const int *in,REALTYPE *out,bool reverse=false) const;
    /** Run the filter on a long float array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const long *in,REALTYPE *out,bool reverse=false) const;
    /** Run the filter on an unsigned char float array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const unsigned char *in,REALTYPE *out,bool reverse=false) const;
    /** Run the filter on an unsigned short array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const unsigned short *in,REALTYPE *out,bool reverse=false) const;
    /** Run the filter on an unsigned int array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const unsigned int *in,REALTYPE *out,bool reverse=false) const;
    /** Run the filter on an unsigned long array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(size_t n,const unsigned long *in,REALTYPE *out,bool reverse=false) const;
    /** Run the filter on a float vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<float>& in, std::vector<REALTYPE>& out,bool reverse= false) const {return apply(in.size(),&in[0],&out[0],reverse);}
    /** Run the filter on a float vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<double>& in, std::vector<REALTYPE>& out,bool reverse= false) const {return apply(in.size(),&in[0],&out[0],reverse);}
    /** Run the filter on a char vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<char>& in, std::vector<REALTYPE>& out,bool reverse= false) const {return apply(in.size(),&in[0],&out[0],reverse);}
    /** Run the filter on a short vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<short>& in, std::vector<REALTYPE>& out,bool reverse= false) const {return apply(in.size(),&in[0],&out[0],reverse);}
    /** Run the filter on an int vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<int>& in, std::vector<REALTYPE>& out,bool reverse= false) const {return apply(in.size(),&in[0],&out[0],reverse);}
    /** Run the filter on a long vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<long>& in, std::vector<REALTYPE>& out,bool reverse= false) const {return apply(in.size(),&in[0],&out[0],reverse);}
    /** Run the filter on an unsigned char vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<unsigned char>& in, std::vector<REALTYPE>& out,bool reverse= false) const {return apply(in.size(),&in[0],&out[0],reverse);}
    /** Run the filter on an unsigned short vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<unsigned short>& in, std::vector<REALTYPE>& out,bool reverse= false) const {return apply(in.size(),&in[0],&out[0],reverse);}
    /** Run the filter on an unsigned int vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<unsigned int>& in, std::vector<REALTYPE>& out,bool reverse= false) const {return apply(in.size(),&in[0],&out[0],reverse);}
    /** Run the filter on an unsigned long vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      * \param reverse run in reverse direction (last to first)
      */
    virtual void apply(const std::vector<unsigned long>& in, std::vector<REALTYPE>& out,bool reverse= false) const {return apply(in.size(),&in[0],&out[0],reverse);}



//Transforms
    /** Calculates the transfer function of the filter
      * \param zr the real part of the argument
      * \param zi the imaginary part of the argument
      * \param ztr the real part of the transfer function
      * \param zti the imaginary part of the transfer function
      */
    virtual void transferFunction(double zr,double zi,double& ztr,double& zti) const;
    /** Calculates the frequency response of the filter. The frequency is in
      * inverse bins so that \f$w=0.5\f$ is the Nyquist frequency, and the response
      * is the same for all integral w.
      * \param w the frequency, in inverse bins.
      * \param re the real part of the frequency response
      * \param im the imaginary part of the frequency response
      */
    virtual void frequencyResponse(double w, double& re,double& im) const;
    /** Calculate the attenuation in signal power at a given frequency. See
      * frequency_response() for an explanation of the frequency units. 
      * \param w the frequency
      * \return the attenuation in signal power
      */
    virtual double powerAttenuation(double w) const;
    /** Calculate the attenuation in signal magnitude at a given frequency. See
      * frequency_response() for an explanation of the frequency units. 
      * \param w the frequency
      * \return the attenuation in signal magnitude
      */
    virtual double magnitudeAttenuation(double w) const;
    /** Calculate the signal phase shift at a given frequency. See
      * frequency_response() for an explanation of the frequency units. 
      * \param w the frequency
      * \return the phase shift
      */
    virtual double phaseShift(double w) const;
    /** Calculate the signal phase delay at a given frequency. See
      * frequency_response() for an explanation of the frequency units. 
      * \param w the frequency
      * \return the phase delay
      */
    virtual double phaseDelay(double w) const;
    /** Calculate the signal group delay at a given frequency. See
      * frequency_response() for an explanation of the frequency units. 
      * \param w the frequency
      * \return the group delay
      */
    virtual double groupDelay(double w) const;

  protected:
    using FirFilter<REALTYPE>::fFIR;
    using FirFilter<REALTYPE>::fOutput;
    std::vector<REALTYPE> fIIR;
    std::vector<REALTYPE> fInitialValues;
};

}//waveform

}//dmtpc

#endif
