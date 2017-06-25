/** \file FirFilter.hh
\brief Header file for dmtpc::waveform::FirFilter class
\author Jeremy Lopez
*/
#ifndef FIRFILTER_HH
#define FIRFILTER_HH
#include <vector>
#include <cstdlib>

namespace dmtpc{

namespace waveform{

/**
  \brief A class to perform finite-impulse response (FIR) filters
  on one-dimensional data. 

  The classes FirFilter<float>, FirFilter<double>
  and FirFilter<long double> are compiled automatically. The filter 
  works on single and double precision input. These are provided rather
  than relying on templates in order to get everything to work with CINT.
  
  \section FIR Finite-Impulse Response Filters
  Finite-impulse response (or FIR) filters are a simple type of digital filter
  used to extract local properties of a discrete signal. They can be used to
  smooth data using the values of a set of nearby points and can perform derivatives,
  as well as other functions. Am FIR filter is defined as a series of coefficients
  (we will call this the kernel and denote its elements by \f$ \{b[n]\}\f$ ) and the 
  filtering action is a discrete convolution of an input signal and the FIR kernel.
  Given an input signal x, the output signal is given by
  \f[ y[n] = \sum\limits_{m=0}^{N_k-1}b[m]x[n+m] = b[N_k-1-n] \ast x[n] \f] 
  
  Note that the order of the coefficients is the reverse of the definition
  used in some other sources such as Wikipedia. This definition uses the actual
  array indices used in the calculation. The last equality is ignoring some time
  shifting since y[n] should really be y[N-1+n] for the convolution definition to
  be correct.

  \section ZT Z-Transforms and the Transfer Function  
 
  A filter is typically characterized by its transfer function, related to its z-transform.
  The z-transform of a digital signal is given by
  \f[ X(z) = Z\{x[n]\} = \sum\limits_{-\infty}^\infty x[n] z^{-n} \f]
  The z-transform is the discrete analog to the Laplace transform. 

  One useful property of FIR filters is the convolution identity
  \f[ Z(x[n]\ast y[n]) = Z(x[n])Z(y[n]) \f]
  Applying this to our definition of the filter, 
  \f[Y(z) = X(z) \sum\limits_{k=0}^{N-1} z^{-k} b[N-1-k] \f]
  We then define the transfer function to be
  \f[ T(z) = \frac{Y(z)}{X(z)} = \sum\limits_{k=0}^{N-1} z^{-k} b[N-1-k] \f]
  
  \section FR Frequency Response
  The frequency response is a special case of the z-transform that is particularly useful in 
  characterizing the filter for actual signals. It is just the transfer function
  of the filter on the unit circle, \f$ z = \exp(2\pi i w) \f$, where w is the
  frequency. This is equivalent to taking a discrete Fourier transform. We let
 \f[ F(w) = T(e^{2\pi iw}) = F_r(w)+ i F_i(w) = \sum\limits_{k=0}^{N-1} \cos (2\pi kw) b[N-1-k] -i \sum\limits_{k=0}^{N-1} \sin(2\pi kw) b[N-1-k] \f]

  The power attenuation gives the ratio of the output signal power to the input signal power and is given by
  \f[ P(w) = F_r(w)^2 + F_i(w)^2 \f]
  The magnitude attenuation is the ratio of the output signal magnitude to the input signal magnitude and is given by
  \f[ M(w) = \sqrt{P(w)} \f]

  The phase shift gives the phase change between the input and output signals. 
  \f[\phi(w) = \arg(F(w)) = \tan^{-1}\frac{F_i}{F_r}  \f]
  
  The phase and group delays are additional ways to measure time delays or phase shifts between the input and output signals.
  the phase delay is 
  \f[\tau_{\phi} = -\frac{\phi(w)}{2\pi w}\f]
  and the group delay is defined as
  \f[\tau_g = -\frac{1}{2\pi}\frac{d\phi}{d\omega}\f]
  If we let \f$F_{r,n} = \sum\limits{k=0}^{N-1} k\cos(2\pi kw) b[N-1-k]\f$ and
  \f$F_{i,n} = -\sum\limits_{k=0}^{N-1} k\sin(2\pi kw)b[N-1-k]\f$ then the group delay
  can be shown to be
  \f[\tau_g = \frac{1}{1+\left(\frac{F_i}{F_r}\right)^2}\left(\frac{F_{r,n}}{F_r}+\frac{F_{i,n}F_i}{F_r^2}\right)\f]

  \author Jeremy Lopez
  \date August 2012
 */
template <class REALTYPE>
class FirFilter
{
  public:
    /** Default Constructor */
    FirFilter(){}
    /** 
     * Constructs a filter object with kernel size n. If ker is not null,
     * its contents are copied into fFIR
     * \param n The kernel size
     * \param ker an array holding the kernel
     */
    FirFilter(size_t n,const float* ker);
    /** 
     * Constructs a filter object with kernel size n. If ker is not null,
     * its contents are copied into fFIR
     * \param n The kernel size
     * \param ker an array holding the kernel
     */
    FirFilter(size_t n,const double* ker);
    /**
     * Constructs a filter using the contents of ker as the kernel
     * \param ker the vector to be copied into fFIR
     */
    FirFilter(const std::vector<float>& ker);
    /**
     * Constructs a filter using the contents of ker as the kernel
     * \param ker the vector to be copied into fFIR
     */
    FirFilter(const std::vector<double>& ker);
    /** Destructor */
    virtual ~FirFilter(){}

    /** Returns the current size of the output vector. This is equal to 
      * \f$N_{out} = N_{in} - N_{k} + 1 \f$. If no input has
      * been specified, the size will be 0.
      */
    size_t outputSize() const {return fOutput.size();}
    /** Returns the size of the kernel (number of coefficients used in the filter */
    size_t firCoeffsSize() const {return fFIR.size();}
    /** Calculates the output size for a given input size. This is given by
      * \f$N_{out} = N_{in} - N_{k} + 1 \f$/
      * \param insize the input size
      * \return the output array size
      */
    size_t calculateOutputSize(size_t insize) const {return insize + 1 - fFIR.size();}

    /** Sets a new kernel
      * \param n the size of the new kernel
      * \param ker an array holding the new coefficients
      */
    void setFirCoeffs(size_t n, const double* ker);
    /** Sets a new kernel
      * \param ker a vector holding the new coefficients
      */
    void setFirCoeffs(const std::vector<double>& ker){setFirCoeffs(ker.size(),&ker[0]);}
    /** Sets a new kernel
      * \param n the size of the new kernel
      * \param ker an array holding the new coefficients
      */
    void setFirCoeffs(size_t n, const float* ker);
    /** Sets a new kernel
      * \param ker a vector holding the new coefficients
      */
    void setFirCoeffs(const std::vector<float>& ker){setFirCoeffs(ker.size(),&ker[0]);}

    /** Get a reference to the output vector */
    std::vector<REALTYPE>& output() {return fOutput;}
    /** Get a const reference to the output vector */
    const std::vector<REALTYPE>& output() const{return fOutput;}
    /** Get a pointer to the output array */
    REALTYPE* getOutput(){return &fOutput[0];}   
    /** Get a const pointer to the output array */
    const REALTYPE* getOutput() const{return &fOutput[0];}
    /** Get the coefficients */
    std::vector<REALTYPE>& firCoeffs(){return fFIR;}
    /** Get the coefficients */
    const std::vector<REALTYPE>& firCoeffs() const{return fFIR;}


    /** Normalize the filter coefficients. This divides all coefficients by
      * \f[ B =  \sum\limits_0^{N_k-1} b_n \f]
      * so that \f$ 1 = \sum b_n\f$. Be careful to avoid calling this 
      * on filters that are not meant to preserve signal integral.
      * Many basic derivative filters may have \f$\sum b_n = 0 \f$ and this
      * function will generate floating point errors.
      */
    virtual void normalize();

    /** Copy the output into an array
      * \param out the array
      */
    void copyOutput(float* out) const;
    /** Copy the output into a vector
      * \param out the vector
      */
    void copyOutput(std::vector<float>& out) const{return copyOutput(&out[0]);}
    /** Copy the output into an array
      * \param out the array
      */
    void copyOutput(double* out) const;
    /** Copy the output into a vector
      * \param out the vector
      */
    void copyOutput(std::vector<double>& out) const{return copyOutput(&out[0]);}
    /** Run the filter on a float array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      */
    virtual void apply(size_t n,const float *in);
    /** Run the filter on a double array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      */
    virtual void apply(size_t n,const double *in);
    /** Run the filter on a char array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      */
    virtual void apply(size_t n,const char *in);
    /** Run the filter on a short array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      */
    virtual void apply(size_t n,const short *in);
    /** Run the filter on an int array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      */
    virtual void apply(size_t n,const int *in);
    /** Run the filter on a long array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      */
    virtual void apply(size_t n,const long *in);
    /** Run the filter on an unsigned char array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      */
    virtual void apply(size_t n,const unsigned char *in);
    /** Run the filter on an unsigned short array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      */
    virtual void apply(size_t n,const unsigned short *in);
    /** Run the filter on an unsigned int array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      */
    virtual void apply(size_t n,const unsigned int *in);
    /** Run the filter on a unsigned long array and save to the output vector fOutput.
      * \param n the input size
      * \param in the input array
      */
    virtual void apply(size_t n,const unsigned long *in);
    /** Run the filter on a float vector and save to the output vector fOutput.
      * \param in the input vector
      */
    virtual void apply(const std::vector<float>& in) {return apply(in.size(),&in[0]);}
    /** Run the filter on a double vector and save to the output vector fOutput.
      * \param in the input vector
      */
    virtual void apply(const std::vector<double>& in) {return apply(in.size(),&in[0]);}
    /** Run the filter on a char vector and save to the output vector fOutput.
      * \param in the input vector
      */
    virtual void apply(const std::vector<char>& in) {return apply(in.size(),&in[0]);}
    /** Run the filter on a short vector and save to the output vector fOutput.
      * \param in the input vector
      */
    virtual void apply(const std::vector<short>& in) {return apply(in.size(),&in[0]);}
    /** Run the filter on a int vector and save to the output vector fOutput.
      * \param in the input vector
      */
    virtual void apply(const std::vector<int>& in) {return apply(in.size(),&in[0]);}
    /** Run the filter on a long vector and save to the output vector fOutput.
      * \param in the input vector
      */
    virtual void apply(const std::vector<long>& in) {return apply(in.size(),&in[0]);}
    /** Run the filter on an unsigned char vector and save to the output vector fOutput.
      * \param in the input vector
      */
    virtual void apply(const std::vector<unsigned char>& in) {return apply(in.size(),&in[0]);}
    /** Run the filter on an unsigned short vector and save to the output vector fOutput.
      * \param in the input vector
      */
    virtual void apply(const std::vector<unsigned short>& in) {return apply(in.size(),&in[0]);}
    /** Run the filter on an unsigned int vector and save to the output vector fOutput.
      * \param in the input vector
      */
    virtual void apply(const std::vector<unsigned int>& in) {return apply(in.size(),&in[0]);}
    /** Run the filter on an unsigned long vector and save to the output vector fOutput.
      * \param in the input vector
      */
    virtual void apply(const std::vector<unsigned long>& in) {return apply(in.size(),&in[0]);}
    /** Run the filter on a float array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      */
    virtual void apply(size_t n,const float *in,REALTYPE *out) const;
    /** Run the filter on a double array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      */
    virtual void apply(size_t n,const double *in,REALTYPE *out) const;
    /** Run the filter on a char array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      */
    virtual void apply(size_t n,const char *in,REALTYPE *out) const;
    /** Run the filter on a short array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      */
    virtual void apply(size_t n,const short *in,REALTYPE *out) const;
    /** Run the filter on an int array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      */
    virtual void apply(size_t n,const int *in,REALTYPE *out) const;
    /** Run the filter on a long float array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      */
    virtual void apply(size_t n,const long *in,REALTYPE *out) const;
    /** Run the filter on an unsigned char float array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      */
    virtual void apply(size_t n,const unsigned char *in,REALTYPE *out) const;
    /** Run the filter on an unsigned short array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      */
    virtual void apply(size_t n,const unsigned short *in,REALTYPE *out) const;
    /** Run the filter on an unsigned int array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      */
    virtual void apply(size_t n,const unsigned int *in,REALTYPE *out) const;
    /** Run the filter on an unsigned long array and save to a given output array.
      * Note that the output vector is not filled in this function.
      * \param n the input size
      * \param in the input array
      * \param out the output array
      */
    virtual void apply(size_t n,const unsigned long *in,REALTYPE *out) const;
    /** Run the filter on a float vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      */
    virtual void apply(const std::vector<float>& in, std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}
    /** Run the filter on a float vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      */
    virtual void apply(const std::vector<double>& in, std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}
    /** Run the filter on a char vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      */
    virtual void apply(const std::vector<char>& in, std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}
    /** Run the filter on a short vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      */
    virtual void apply(const std::vector<short>& in, std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}
    /** Run the filter on an int vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      */
    virtual void apply(const std::vector<int>& in, std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}
    /** Run the filter on a long vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      */
    virtual void apply(const std::vector<long>& in, std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}
    /** Run the filter on an unsigned char vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      */
    virtual void apply(const std::vector<unsigned char>& in, std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}
    /** Run the filter on an unsigned short vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      */
    virtual void apply(const std::vector<unsigned short>& in, std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}
    /** Run the filter on an unsigned int vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      */
    virtual void apply(const std::vector<unsigned int>& in, std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}
    /** Run the filter on an unsigned long vector and save to a given output vector.
      * Note that the output vector fOutput, saved internally in the class is
      * not filled in this function.
      * \param in the input vector
      * \param out the output vector
      */
    virtual void apply(const std::vector<unsigned long>& in, std::vector<REALTYPE>& out) const {return apply(in.size(),&in[0],&out[0]);}

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
    /** The FIR coefficients */
    std::vector<REALTYPE> fFIR; 
    /** The output vector */
    std::vector<REALTYPE> fOutput;
};


}//waveform

}//dmtpc

#ifdef __CINT__
template class dmtpc::waveform::FirFilter<float>;
template class dmtpc::waveform::FirFilter<double>;
#endif
#endif

