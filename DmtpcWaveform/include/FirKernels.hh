/** \file FirKernels.hh
\brief Header file for dmtpc::waveform::fir namespace containing methods to generate FIR filter coefficients
\author Jeremy Lopez
*/
#ifndef FIRKERNELS_HH
#define FIRKERNELS_HH
#include <vector>
namespace dmtpc{
namespace waveform
{
  /** \brief Namespace for different one-dimensional kernels useful for filters and other operations

    Currently, this namespace contains only the convolution kernels used by various FIR filters. It may
    be expanded later to include more. The kernels are returned as std::vector<double> in order to have 
    the best precision possible while still being portable. If a long double type is ever standardized,
    it may be useful to change the values here to that.

    \author Jeremy Lopez
    \date August 2012

    
  */
  namespace fir{
  //Kernels: Define using vector<double> for better precision. 
  //Long double has no ISO/ANSI standard so double is most precise that should be platform independent

  /** The raised cosine window is given by
   * \f[b[n+N] = \sum_{k=0}^{4} a_k \cos\frac{k\pi n}{N}\f]
   * where N is the half-width of the filter.
   * \param halfwidth The half-width of the kernel. The total width is 2*halfwidth+1
   * \param a0 first parameter
   * \param a1 second parameter
   * \param a2 third parameter
   * \param a3 fourth parameter
   * \param a4 fifth parameter
   * \return Raised cosine window with given properties
   */
  std::vector<double> raisedCosine(int halfwidth, double a0=1, double a1=0, double a2=0, double a3=0, double a4=0);
  /** Same as the other raised_cosine() function but takes an array argument instead
   * \param halfwidth The half-width of the kernel
   * \param a The parameters
   * \return a raised cosine window
   */
  std::vector<double> raisedCosine(int halfwidth, double a[]);

  //Many of these are variants of the raised cosine window, but CINT hates inline definitions in namespaces.
  /** The Blackman window is a raised cosine window with 
      \f[ \{a_k\} = \frac{1}{2}\{1-\alpha,1,\alpha,0,0 \}\f]
      \param halfwidth the half-width of the filter
      \param alpha the window parameter
      \return a Blackman window
  */
  std::vector<double> blackman(int halfwidth, double alpha=0.16);
  /** The Blackman-Harris window is a raised cosine window with
      \f[\{a_k\} = \{0.35875,0.48829,0.14128,0.01168,0 \}\f]
      \param halfwidth the half-width of the filter
      \return a Blackman-Harris window
  */ 
  std::vector<double> blackmanHarris(int halfwidth);
  /** The Blackman-Nutall window is a raised cosine window with
      \f[\{a_k\} = \{0.3635819,0.4891775,0.1365995,0.0106511,0\} \f]
      \param halfwidth the half-width of the filter
      \return a Blackman-Nutall window
  */
  std::vector<double> blackmanNutall(int halfwidth);
  /** The Bohman window is a window defined by
    \f[ b[n+N] = \left(1 - \frac{|n|}{N}\right)\cos\frac{n\pi}{N} + \frac{1}{\pi}\sin\frac{n\pi}{N}\f]
    where N is the half-width.
    \param halfwidth the half-width of the filter
    \return a Bohman window
  */
  std::vector<double> bohman(int halfwidth);
  /** The Breit-Wigner window is the kernel defined by a Breit-Wigner (or Cauchy) distribution,
      the Fourier transform of an exponential:
      \f[ b[n+N] = \frac{1}{1+\left(\frac{n}{\Gamma}\right)^2}\f]
      \param halfwidth the half-width of the filter
      \param gamma the width of the distribution
      \return a Breit-Wigner window
  */
  std::vector<double> breitWigner(int halfwidth,double gamma);
  /** The cosine window is the raised cosine window with
      \f[ \{a_k\} = \{0,1,0,0,0\} \f]
      \param halfwidth the half-width of the filter
      \return a Cosine window
  */
  std::vector<double> cosine(int halfwidth);
  /** The exponential window is given by an exponential function:
      \f[ b[n+N] = e^{-|n|/x_0}\f]
      \param halfwidth the half-width of the filter
      \param x0 the decay constant
      \return an exponential window
  */
  std::vector<double> exponential(int halfwidth, double x0);
  /** The Fermi-Dirac window is given by a Fermi-Dirac distribution,
      \f[ b[n+N] = \left[ \exp\frac{|n|-\mu}{T}+1 \right]^{-1}\f]
      \param halfwidth the half-width of the filter 
      \param T the scale parameter (temperature)
      \param mu the offset parameter (chemical potential)
      \return a Fermi-Dirac window
  */
  std::vector<double> fermiDirac(int halfwidth, double T, double mu);
  /** The flat-top window is the raised cosine window with
      \f[ \{a_k\} = \{1,1.92,1.29,0.388,0.032\} \f]
      \param halfwidth the half-width of the filter
      \return a flat-top window
  */
  std::vector<double> flattop(int halfwidth);
  /** The Gaussian window is given by a Gaussian distribution,
      \f[ b[n+N] = \exp-\frac{n^2}{2\sigma^2} \f]
      \param halfwidth the half-width of the filter
      \param sigma the standard deviation (width)
      \param a Gaussian window
  */
  std::vector<double> gaussian(int halfwidth, double sigma);
  /** The Hamming window is the raised cosine window with
      \f[ \{a_k\} = \{0.54,0.46,0,0,0\} \f]
      \param halfwidth the half-width of the filter
      \return a Hamming window
  */
  std::vector<double> hamming(int halfwidth);
  /** The Hann window is the raised cosine window with
      \f[ \{a_k\} = \{0.5,0.5,0,0,0\} \f]
      \param halfwidth the half-width of the filter
      \return a Hann window
  */
  std::vector<double> hann(int halfwidth);
  /** The Kaiser window is built from the 0th modified Bessel function of the first kind
    \f[ b[n+N] = I_0\left(\alpha\pi\sqrt{1 - \left(\frac{n}{N}\right)^2}\right) \f]
    \param halfwidth the half-width of the filter
    \param a the width parameter
    \return a Kaiser window
  */
  std::vector<double> kaiser(int halfwidth,double a=3);
  /** The Lanczos window is a sinc window,
      \f[ b[n+N] = \frac{ \sin\frac{n\pi}{N}}{n\pi/N} \f]
      \param halfwidth the half-width of the filter
      \return a Lanczos window
  */
  std::vector<double> lanczos(int halfwidth);
  /** The Nutall window is the raised cosine window with
      \f[ \{a_k\} = \{0.355768,0.487396,0.144232,0.012604,0\} \f]
      \param halfwidth the half-width of the filter
      \return a Nutall window
  */
  std::vector<double> nutall(int halfwidth);
  /** The rectangle window is just a constant
     \f[ b[n+N] = \frac{1}{2N+1} \f]
     \param halfwidth the half-width of the filter
     \return a rectangle window
  */
  std::vector<double> rectangle(int halfwidth);
  /** The Savitzky-Golay class of filters perform a least squares polynomial fit on the data near each 
      point. For a filter of half-width N and order \f$N_O\f$, we can define such a fit by finding the coefficients D[n]
      such that we can approximate the data in a region around each point M by
     \f[ x[n+M] = \sum\limits_{m=0}^{N_0} n^m D[m]\f]
     If we let \f$ A[n][m] = (n-N)^m\f$ and we take \f$0^0 = 1\f$, this can be written 
     \f[ x[n+M-N] = A[n][m] D[m] \f]

     Solving this by the usual least-squares method, we get
     \f[ D = \left(A^T A\right)^{-1} A^T x \f]
     So, letting \f$B = (A^T A)^{-1} A^T\f$, we get estimates for the values of D[n] at point M,
     \f[ D[n] = B[n][m] x[m +M-N] \f]
     If we interpret these as derivatives of the signal, we get
     \f[ x^{(n)}[M] \approx n! D[n] = n! B[n][m] x[m+M-N] \f]

     So, we see that for the \f$N_D(\le N_O)\f$ derivative the correct Savitzky-Golay kernel to use is
     \f[ b[n] = N_D! B[N_D][n] \f]

    \param halfwidth the half-width of the filter 
    \param order the order of the filter. 
    \param nder output the given derivative (0 is a smoothed function, 1 is first derivative, etc)
  */
  std::vector<double> savitzkyGolay(int halfwidth,int order, int nder=0);
  /** The sinc window is the same as the Lanczos window (lanczos())
     \param halfwidth the half-width of the filter
     \return a sinc window
  */
  std::vector<double> sinc(int halfwidth);
  /** The sinc squared window is given by
      \f[ b[n+N] = \left[\frac{ \sin\frac{n\pi}{N}}{n\pi/N}\right]^2 \f]
      \param halfwidth the half-width of the filter
      \return a sinc-squared window
  */
  std::vector<double> sincSquared(int halfwidth);
  /** The triangle (or Bartlett) window is given by 
      \f[ b[n+N] = 1 - \frac{|n|}{N} \f]
      \param halfwidth the half-width of the filter
      \return a triangle window
  */
  std::vector<double> triangle(int halfwidth);

  //Convolutions: actually integrating the function across the bin
  /** The exponential convolution window integrates the exponential function
      across the bin rather than using just the value of the function.
      Its coefficients are given by
      \f[ b[n+N] = \exp\left(-\frac{2|n|-1}{2x_0}\right) - \exp\left(-\frac{2|n|+1}{2x_0}\right) \f]
      with
      \f[ b[N] = 2\left(1-\exp\left(-\frac{1}{2x_0}\right)\right) \f]
      The width is determined by a cut value, where the kernel is cut off when \f$\exp(-n/N)<C\f$. 
      \param x0 the decay constant
      \param cut the cutoff value for setting the width
      \return an exponential convolution window
  */
  std::vector<double> exponentialConv(double x0, double cut=0.01);
  /** The Gaussian convolution window integrates the Gaussian distribution across
      the bin.  Its coefficients are
      \f[ b[n+N] = erf\left(2\frac{|n|+1}{2\sigma\sqrt{2}}\right) - erf\left(\frac{2|n|+1}{2\sigma\sqrt{2}}\right)\f]
      The size is determined by a cut value, where  the kernel is cut off when \f$\exp(-n^2/2\sigma^2)<C \f$
      \param sigma the standard deviation (width)
      \param cut the cutoff value for setting the kernel size
      \return a Gaussian convolution window
  */
  std::vector<double> gaussianConv(double sigma, double cut=0.01);
  /** The triangle convolution window integrates the triangle distribution across each bin.
      Its coefficients are
      \f[ b[n+N] = 1 - \frac{2|n|}{2N+1} \f]
      with
      \f[ b[N] = 1 - \frac{1}{2(2N+1)}\f]
      \param halfwidth the half-width of the filter
      \return a triangle convolution window
  */
  std::vector<double> triangleConv(int halfwidth);
  }
}
}
#endif
