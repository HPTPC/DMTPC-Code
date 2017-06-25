/**\file WaveformTools.hh
\author Jeremy Lopez
\date 19 April 2011

\brief Header file for signal processing methods

*/

#ifndef WAVEFORMPROCESSING_HH
#define WAVEFORMPROCESSING_HH

class TH2;
class TH1; 
#include "FirFilter.hh"
#include "IirFilter.hh"


namespace dmtpc{

namespace waveform{

  /**Perform a median filter on the given histogram
    \param hist the histogram
    \param size the size of the filter window
    \param name the name of the new histogram
  */
  TH1* medianFilter(const TH1* hist, int size, 
		     const char* name="medianFilter");



  /**Numerically integrate the histogram using linear interpolation
    \param hist the histogram
    \param useXwidth if true, include the width of each bin in determining the integral
    \param name the new histogram name
  */
  TH1* integral(const TH1* hist, bool useXwidth = false,
	      const char* name ="Integral");

  /** Compute an IIR filter of the given histogram. acoeffs and bcoeffs should be arrays of size order+1
   *  \param hist the histogram to compute the filter of 
   *  \param order the order of the filter
   *  \param acoeffs array of a coefficients 
   *  \param bcoeffs array of b coefficients 
   *  \return the filtered histogram
   * */
  TH1 *IIRFilter(const TH1* hist, unsigned int order, const double * acoeffs, const double * bcoeffs, TH1 * out = 0);

  /** Compute an FIR filter of the given histogram. bcoeffs should be array of size order+1
   *  \param hist the histogram to compute the filter of 
   *  \param order the order of the filter
   *  \param bcoeffs array of b coefficients 
   *  \return the filtered histogram
   * */
  TH1 *FIRFilter(const TH1* hist, unsigned int order, const double * bcoeffs, TH1 *  out);

 
  /** 
   * Compute the IIR coefficients for a (2*n) order band stop butterworth filter. 
   * \param w0 the center of the stopband in normalized frequency (w/wnyquist)
   * \param delta width of the stopband in normalized frequency (w/wnyquist)
   * \param the order of the filter (actually a band stop is two filters, so it comes out to 2*n)
   * \param acoeffs pointer to double *, memory will be allocated for this array
   * \param bcoeffs pointer to double *, memory will be allocated for this array
   * \return the gain of the filter
   */
  double butterworthBandStop(double w0, double delta, unsigned int n, double ** acoeffs, double ** bcoeffs);
  double butterworthBandPass(double w0, double delta, unsigned int n, double ** acoeffs, double ** bcoeffs);

  //FIR Filter Tools
  /** Create an output histogram for an FIR filter given the input histogram and a kernel size.
   *  The type is either double for TH1D or float for all others
   *  \param in The input histogram
   *  \param nkernel The size of the convolution kernel
   *  \return An output histogram with the correct number of bins, axis titles, and bin widths/positions
   * */
  TH1* createFirOutputHist(const TH1* in, int nkernel);
  /** Create an output histogram for an FIR filter given the number of input bins, a kernel size, and a
   *  histogram type. The type should be "C" for char, "S" for short, "I" for int, "F" for float and "D"
   *  for double. In practice only float or double should be used.
   * \param nin the size of the input histogram
   * \param nkernel The size of the convolution kernel
   * \param type the type of output to be used
   * \return An output histogram with the correct number of bins. 
   * */
  TH1* createFirOutputHist(int nin, int nkernel,char type);
  /** Use an FIR filter on a ROOT histogram. If out points to NULL, a new 
      histogram will be created.
      \param in The input histogram
      \param out The output histogram
      \param fir The filter object
      \return The output (either out or a new histogram if out is null)
   * */
  TH1* runFirFilter(const TH1* in, TH1* out, FirFilter<double>* fir);

  //IIR Filter Tools
  /** Create an output histogram for an IIR filter given the input histogram and a kernel size.
   *  The type is either double for TH1D or float for all others
   *  \param in The input histogram
   *  \param nkernel The number of FIR coefficients in the filter
   *  \return An output histogram with the correct number of bins, axis titles, and bin widths/positions
   * */
  TH1* createIirOutputHist(const TH1* in, int nkernel);
  /** Create an output histogram for an IIR filter given the number of input bins, a kernel size, and a
   *  histogram type. The type should be "C" for char, "S" for short, "I" for int, "F" for float and "D"
   *  for double. In practice only float or double should be used.
   * \param nin the size of the input histogram
   * \param nkernel The number of FIR coefficients in the filter
   * \param type the type of output to be used
   * \return An output histogram with the correct number of bins. 
   * */
  TH1* createIirOutputHist(int nin,int nkernel, char type);
  /** Use an IIR filter on a ROOT histogram. If out points to NULL, a new 
      histogram will be created.
      \param in The input histogram
      \param out The output histogram
      \param iir The filter object
      \return The output (either out or a new histogram if the output is null
   * */
  TH1* runIirFilter(const TH1* in, TH1* out, IirFilter<double>* iir);

  /** Remove bins below some threshold
      \param in the input histogram. Will be modified
      \param zero_level the baseline
      \param noise_ceiling the threshold below which bins will be suppressed
      \param min the minimum number of bins in the window above threshold to keep a bin
      \param window_size the (half-)size of the window used to test if a bin is to be suppressed
      \param mean the mean of the suppressed bins
      \param rms the rms of the suppressed bins
  */
  unsigned int zeroSupress(TH1* in, double zero_level,double noise_ceiling,
                        unsigned int min, unsigned int window_size, 
                        double* mean=0,double* rms=0);
  
  /** Removes zero-valued bins at the beginning and end of a histogram and saves the output to a given pointer.
      \param in the input histogram
      \param out the pointer where the output histogram should be allocated. Set to 0 to use default new function.
  */
  TH1* cropZeros(const TH1* in, TH1* out = 0);

  TH1 * gausConv(const TH1* in, double sigma, bool rmEdge = true, double cut = 0.01, const char * name = "gausConv"); 

  TH1* convolution(const TH1* hist, int N, const double* arr, 
		    bool removeEdges = true, const char* name = "convolution");

}//waveform
}//dmtpc
#endif
