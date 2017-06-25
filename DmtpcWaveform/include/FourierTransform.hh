/** \file FourierTransform.hh
\brief Header file for dmtpc::waveform::FourierTransform class
\author Jeremy Lopez
*/
#ifndef FOURIERTRANSFORM_HH
#define FOURIERTRANSFORM_HH
#include <cmath>
#include <vector>

#ifndef M_PI 
  #define M_PI 3.1415926535897932384626433832795029L
#endif

#ifndef __CINT__
  #include "fftw3.h"
#else
//Define some FFTW stuff
  class fftw_complex;
  class fftw_plan;
//Defined in fftw3.h
  #define FFTW_MEASURE (0U)
  #define FFTW_DESTROY_INPUT (1U << 0)
  #define FFTW_UNALIGNED (1U << 1)
  #define FFTW_CONSERVE_MEMORY (1U << 2)
  #define FFTW_EXHAUSTIVE (1U << 3)
  #define FFTW_PRESERVE_INPUT (1U << 4)
  #define FFTW_PATIENT (1U << 5)
  #define FFTW_ESTIMATE (1U << 6)
#endif

namespace dmtpc{

namespace waveform{
/**
* \class FourierTransform
  \brief A wrapper class for using FFTW3 with 1-dimensional data.

  /section intro Introduction 

  This class is meant to provide the functionality of the ROOT FFT classes
  acting on one-dimensional real-valued data. It should work without any of 
  the memory management issues that need to be dealt with when using the
  ROOT classes.

  To use or compile with the CINT command line, make sure the following
 (replacing the paths with the correct ones for the system) are included
  in rootlogon.C:

  \code
    gSystem->Load("FFTW_LIB_PATH/libfftw3.so");
    gSystem->AddIncludePath("-IFFTW_INCLUDE_PATH");
  \endcode

  \section workings Internal Workings

  This class works a bit differently than the standard ROOT classes, which
  inherit from TVirtualFFT. While TVirtualFFT acts on ROOT histograms (TH1 class)
  waveform::FFT works on C++ arrays to allow for more flexibility. Separate
  functions to use histograms are to be provided in the waveform::tools namespace.

  As in the TVirtualFFT classes, waveform::FFT uses FFTW3 to calculate the transforms.
  FFTW3 only acts on double arrays, so waveform::FFT only does as well. In order to 
  utilize the built-in optimization of FFTW, this class has a significant amount of
  memory allocated using fftw_malloc(). The internal memory is used for all FFT
  calculations, so the data must be copied from the user's input to the memory in the
  fft object.

  For optimization, the object allocates memory for two real-valued arrays and
  three complex-valued arrays. These are
    1. The input data
    2. The complex output of the input FFT
    3. The complex values of an intermediate FFT step (allowing for filtering)
    3. The complex input of the inverse FFT
    4. The output of the inverse FFT

  An intermediate step is needed because for the complex to real FFT calculation,
  FFT needs to destroy the data in the input. The intermediate array allows the user
  to access both the raw FFT of the input and a transformed FFT.


  \section ops Supported Operations

    This classes supports a number of operations involving FFTs. 

  1. Fourier Transform:
    As the class name suggests, fast Fourier transforms can be calculated.
    \f[ F(\nu) = FFT(X[n]) \f]
  2. Power spectrum (spectral density):
    The power spectrum (or spectral density) at a given frequency is just
    square of the magnitude of the Fourier transform of the signal,
    \f[ P(\nu) =  F(\nu)F^\ast(\nu) \f]
  3. Autocorrelations:
    The autocorrelation of time-series data can be efficiently calculated using
    the Wiener-Khinchin theorem,
    \f[ A(\tau) = (f(t)\ast f^\ast(t))(\tau) = \int\limits_\infty^\infty f(t)f^\ast(t-\tau)dt
               = IFFT(P(\nu))(\tau) \f]

  4. Filtering in the frequency domain: Filters can be defined by a series of coefficients in
     the frequency domain. If the filter is defined by the coefficients \f$ C[i] \f$, then
     the filtered waveform is given by
     \f[ X'[n] = \mathrm{IFFT}( C[k]\mathrm{FFT}(X[j])[k] )[n]\f]

   Filtering here is an \f$\mathcal{O}(N\log N)\f$ process, so it will be faster to define a low-pass 
   filter as an FIR or IIR filter with few coefficients whenever possible, an \f$\mathcal{O}(N)\f$ process.

   Note that inverse Fourier transform, autocorrelation, and power spectrum are all real-valued, but
   only a single real-valued output array is provided. Only one of these can be saved into memory
   at a time, so calculating the autocorrelation overwrites the inverse Fourier transform, the 
   power spectrum overwrites the autocorrelation, and so on. Care must be taken to ensure that 
   when the user attempts to retrieve one of these that the correct one has been calculated and
   is saved into memory. Individual array elements of the power spectrum can be explicitly
   calculated one by one to avoid overwriting data. The autocorrelation and inverse Fourier 
   transform require that the inverse transform be calculated so overwriting data cannot be
   avoided.

  \section usage Using This Class

  This class should be fairly straightforward to use. For example, to calculate the power spectrum
  and autocorrelation:
  \code
  //Have array x of size N as input and y as output
  waveform::FFT fil(N);
  fil.setWaveform(x);
  //Calculate the FFT
  fil.fft();
  //Get the autocorrelation: this is a bit faster since the FFT is already calculated
  y = fil.autocorrelation();
  //Get the power spectrum at bin 1.
  //This does not overwrite the values in the autocorrelation array
  double z = fil.getPowerSpectrum(1);
  //This will overwrite the autocorrelation array. y will now hold the power spectrum
  fil.getPowerSpectrum();
  \endcode

 \author Jeremy Lopez


*/
class FourierTransform
{	

  public:

    /**
      Constructor. Creates FFTW plans for data with n points.
      \param n the size of the input data to be used
      \param flag the FFTW flag used to generate the plans
    */
    FourierTransform(int n,unsigned flag = FFTW_MEASURE);
    /** Destructor */
    virtual ~FourierTransform();

    /** Get the size of the input data */
    int getN() const{return fN;}
 
    /** Get the size of the input data */
    int size() const {return fN;}
    
    /** Get the size of the FFT data */
    int fftSize() const {return fN/2 + 1;}

    /** Get the frequency 
       \param i frequency bin
    */
    double frequency(int i) const {return ((double)i)/fN;}

    /** Get the angular frequency 
        \param i frequency bin
    */
    double angularFrequency(int i) const {return frequency(i) * 2 * M_PI;}

    /** Get the wavelength
        \param i frequency bin
    */
    double wavelength(int i) const {return 1./frequency(i);}     

    /** Set the waveform from an array
        \param wf the data
    */
    void setWaveform(const double* wf);
    /** Set the waveform from an array
        \param wf the data
    */
    void setWaveform(const float* wf);
    /** Set the waveform from a vector
        \param x the data
    */
    void setWaveform(const std::vector<double>& x){setWaveform(&x[0]);}

    /** Directly set the values of the FFT
        \param cx the FFT
    */
    void setFFT(const fftw_complex* cx);
    /** Directly set the values of the FFT
        \param r the real part
        \param i the imaginary part
     */
    void setFFT(const double* r,const double* i);
    /** Directly set the values of the FFT using vectors
       \param r the real part
       \param i the imaginary part
    */
    void setFFT(const std::vector<double>& r, const std::vector<double>& i)
      {setFFT(&r[0],&i[0]);}

    /** Calculate the FFT of the current waveform */
    void fft();
    /** Calculate the FFT of the given waveform
        \param wf the waveform
    */
    void fft(const double* wf);
    /** Calculate the FFT of the given waveform
        \param x the waveform
    */
    void fft(const std::vector<double>& x){fft(&x[0]);}	

    /** Calculate the inverse Fourier transform of the current FFT */
    void fftInverse();
    /** Calculate the inverse Fourier transform of the given FFT
        \param cx the FFT
    */
    void fftInverse(const fftw_complex* cx);
    /** Calculate the inverse fourier transform of the given FFT
        \param r the real part of the FFt
        \param im the imaginary part of the FFT
    */
    void fftInverse(const double* r,const double* im);
    /** Calculate the inverse fourier transform of the given FFT
        \param x the real part of the FFt
        \param y the imaginary part of the FFT
    */
    void fftInverse(const std::vector<double>& x,const std::vector<double>& y){fftInverse(&x[0],&y[0]);}

    /** Calculate the autocorrelation of the current waveform
        \return the autocorrelation
    */
    const double* autocorrelation();
    /** Calculate the autocorrelation of the given waveform
        \param wf the waveform
        \return the autocorrelation
    */
    const double* autocorrelation(const double* wf); 
    /** Calculate the autocorrelation of the given waveform
        \param x the waveform
        \return the autocorrelation
    */
    const double* autocorrelation(const std::vector<double>& x){return autocorrelation(&x[0]);}

    /** Calculate the spectral density of the given waveform
        \return the spectral density  
    */
    const double* spectralDensity();
    /** Calculate the spectral density of the given waveform
        \param wf the waveform
        \return the spectral density  
    */
    const double* spectralDensity(const double* wf); 
    /** Calculate the spectral density of the given waveform
        \param x the waveform
        \return the spectral density  
    */
    const double* spectralDensity(const std::vector<double>& x){return spectralDensity(&x[0]);}

    /** Set a filter in the frequency domain. The size of the filter should be the size
        of the Fourier transform of the signal. This function just scales the FFT by 
        a real value.
        \param x the filter coefficients
    */
    void setFilter(const double* x);
    /** Set a complex-valued filter in the frequency domain. The size of the filter should
        be the size of the Fourier transform of the signal. This multiplies each FFT FFT[n]
        by the complex value x[n]+i*y[n].
        \param x the real part of the filter coefficients
        \param y the imaginary part of the filter coefficients
    */
    void setFilter(const double* x,const double* y);
    /** Set a complex-valued filter in the frequency domain. The size of the filter should
        be the size of the Fourier transform of the signal. This multiplies each FFT FFT[n]
        by the complex value r[n]* (cos(phi[n])+i*sin(phi[n])).
        \param r the magnitudes of the filter coefficients
        \param phi the phase shifts the filter coefficients
    */
    void setFilterPolar(const double* r, const double* phi);
    /** Set a complex valued filter in the frequency domain. The size of the filter should
        be the size of the Fourier transform of the signal.
        \param x the filter coefficients 
    */
    void setFilter(const fftw_complex* x);
    /** Multiply the FFT values by a set of real coefficients x[n]. This is equivalent
        to setting the filter and then applying an FFT and inverse FFT. Successive calls
        will act on the filtered FFT values, allowing the user to apply multiple filters
        in succession.
        \param x the coefficients 
    */
    void multiply(const double* x);
    /** Multiply the FFT values by a set of complex coefficients x[n]+i*y[n]. This is equivalent
        to setting the filter and then applying an FFT and inverse FFT. Successive calls
        will act on the filtered FFT values, allowing the user to apply multiple filters
        in succession.
        \param x the real part of the coefficients 
        \param y the imaginary part of the coefficients
    */
    void multiply(const double* x, const double* y);
    /** Multiply the FFT values by a set of complex coefficients x[i]. This is equivalent
        to setting the filter and then applying an FFT and inverse FFT. Successive calls
        will act on the filtered FFT values, allowing the user to apply multiple filters
        in succession.
        \param x the coefficients 
    */
    void multiply(const fftw_complex* x);

    /** Performs an FFT on the input data, applies a real-valued filter and inverse transforms back to the time domain.
        \param x the coefficients
    */
    void convolution(const double* x);
    /** Performs an FFT on the input data, applies a complex-valued filter and inverse transforms back to the time domain.
        \param x the real part of the coefficients
        \param y the imaginary part of the coefficients
    */
    void convolution(const double* x,const double* y);
    /** Performs an FFT on the input data, applies a complex-valued filter and inverse transforms back to the time domain.
        \param x the coefficients
    */
    void convolution(const fftw_complex* x);
    /** Apply the current filter to the FFT. Does nothing if no filter is set. This is automatically called with the 
        fft() function.
    */
    void applyFilter();
    /** Clear the current filtered data to return to the FFT of the original input data */
    void clearFilter(); 
    /** Use or ignore the current figure.
        \param use true to use the filter, false to ignore
    */
    void useFilter(bool use=true){fUseFilter = use;}
    /** See if the filter is to be used. */
    bool isFilterUsed() const {return fUseFilter;}
    /** Get the input waveform */
    double* getWaveform(){return fData;}
    /** Get the input waveform (const version) */
    const double* getWaveform() const {return fData;}
    /** Get the unfiltered FFT values */
    fftw_complex* getFFT() {return fCxOut;}
    /** Get the unfiltered FFT values (const version) */
    const fftw_complex* getFFT() const {return fCxOut;}
    /** Get the filtered FFT values */
    fftw_complex* getFilteredFFT() {return fCxFil;}
    /** Get the filtered FFT values (const version) */
    const fftw_complex* getFilteredFFT() const {return fCxFil;}
    /** Get the output of operations returning real values */
    double* getRealOutput() {return fIfft;}
    /** Get the output of operations return real values (const version) */
    const double* getRealOutput() const {return fIfft;}
    /** Get the inverse FFT if it has been calculated */
    double* getInverseFFT() {return fIfft;}
    /** Get the inverse FFT if it has been calculated */
    const double* getInverseFFT() const {return fIfft;}
    /** Get the autocorrelation if it has been calculated */
    double* getAutocorrelation() {return fIfft;}
    /** Get the autocorrelation if it has been calculated */
    const double* getAutocorrelation() const {return fIfft;}
    /** Get the spectral density (power spectrum) if it has been calculated*/
    double* getSpectralDensity() {return fIfft;}
    /** Get the spectral density (power spectrum) if it has been calculated*/
    const double* getSpectralDensity() const {return fIfft;}
    /** Get the spectral density (power spectrum) if it has been calculated*/
    double* getPowerSpectrum() {return fIfft;}
    /** Get the spectral density (power spectrum) if it has been calculated*/
    const double* getPowerSpectrum() const {return fIfft;}
    /** Set a bin in the original waveform
        \param n the bin number
        \param x the new value 
    */
    void setWaveform(int n, double x);
    /** Directly set an FFT bin.
        \param n the bin number
        \param x the real part
        \param y the imaginary part
    */
    void setFFT(int n,double x,double y);
    /** Directly set an FFT bin
        \param n the bin number
        \param x the value
    */
    void setFFT(int n,fftw_complex x);
    /** Directly set a bin in the filtered FFT
        \param n the bin number
        \param x the real part
        \param y the imaginary part
    */
    void setFilteredFFT(int n,double x,double y);
    /** Directly set a bin in the filtered FFT
        \param n the bin number
        \param x the value
    */
    void setFilteredFFT(int n,fftw_complex x);
    /** Get a value from the original waveform
        \param n the bin number
    */
    double getWaveform(int n) const{return fData[n];}
    /** Get a value from a real valued output function
        \param n the bin number
    */
    double getRealOutput(int n)const{return fIfft[n];}
    /** Get a value from the inverse FFT
        \param n the bin number
    */
    double getInverseFFT(int n) const {return fIfft[n];}
    /** Get a value from the autocorrelation
        \param n the bin number
    */
    double getAutocorrelation(int n) const {return fIfft[n];}
    /** Get a value from the spectral density (power spectrum)
        \param n the bin number
    */
    double getSpectralDensity(int n) const {return  sqrt(fCxFil[n][0]*fCxFil[n][0]+fCxFil[n][1]*fCxFil[n][1]);}
    /** Get a value from the power spectrum (spectral density)
        \param n the bin number
    */
    double getPowerSpectrum(int n) const {return getSpectralDensity(n);}
    /** Get the real part of a value in the FFT
        \param n the bin number
    */
    double getFFTRealPart(int n)const{return fCxOut[n][0];}
    /** Get the imaginary part of a value in the FFT
        \param n the bin number
    */
    double getFFTImPart(int n)const{return fCxOut[n][1];}
    /** Get the magnitude squared of a value in the FFT
        \param n the bin number
    */
    double getFFTMag2(int n)const{return fCxOut[n][0]*fCxOut[n][0] + fCxOut[n][1]*fCxOut[n][1];}
    /** Get the magnitude of a value in the FFT
        \param n the bin number
    */
    double getFFTMag(int n)const{return sqrt(getFFTMag2(n));}
    /** Get the phase of a value in the FFT
        \param n the bin number
    */
    double getFFTPhase(int n)const{return atan2(fCxOut[n][1],fCxOut[n][0]);}
    /** Get the real part of a value in the filtered FFT
        \param n the bin number
    */
    double getFilteredFFTRealPart(int n)const{return fCxFil[n][0];}
    /** Get the imaginary part of a value in the filtered FFT
        \param n the bin number
    */
    double getFilteredFFTImPart(int n)const{return fCxFil[n][1];}
    /** Get the magnitude squared of a value in the filtered FFT
        \param n the bin number
    */
    double getFilteredFFTMag2(int n)const{return fCxFil[n][0]*fCxFil[n][0] + fCxFil[n][1]*fCxFil[n][1];}
    /** Get the magnitude of a value in the filtered FFT
        \param n the bin number
    */
    double getFilteredFFTMag(int n)const{return sqrt(getFilteredFFTMag2(n));}
    /** Get the phase of a value in the filtered FFT
        \param n the bin number
    */
    double getFilteredFFTPhase(int n)const{return atan2(fCxFil[n][1],fCxFil[n][0]);}
    /** Get the type of output held in the real valued output array. This is 0 for 
        the inverse FFT, 1 for autocorrelation and 2 for spectral density/power spectrum.
    */
    int getOutputType() const {return fOutputType;}
    /** See if the real output is an inverse FFT */
    bool outputIsIFFT()const{return fOutputType==0;}
    /** See if the real output is an autocorrelation */
    bool outputIsAutocorr()const{return fOutputType==1;}
    /** See if the real output is the spectral density/power spectrum */
    bool outputIsSpecDensity() const {return fOutputType==2;}

  private:
    int fN;///< the number of bins in the input histogram
    unsigned int fFlag;///< the FFTW3 flag for optimizing the FFT algorithm
    bool fFFTisDone;///< set to true if the FFT has been performed and the input has not changed
    bool fUseFilter;///< set to true if a filter is to be used
    double* fData;///< the input data
    double* fIfft;///< the real-valued output (inverse FFT, autocorrelation or power spectrum)
    fftw_complex* fCxOut;///< the output of the FFT
    fftw_complex* fCxFil;///< the filtered FFT output
    fftw_complex* fCxIn;///< the input of the inverse FFT
    fftw_plan fPlan;///< the FFT plan
    fftw_plan fIPlan;///< the inverse FFT plan

    int fOutputType;///< the type of output held by the real-valued output array
    std::vector<double> fFilterReal;///< the real part of the filter coefficients
    std::vector<double> fFilterIm;///< the imaginary part of the filter coefficients

  };
}
}
#endif

