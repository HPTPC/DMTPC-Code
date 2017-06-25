/**\file WaveformAnalysis.hh
\author Jeremy Lopez
\date 19 April 2011

Header file for waveform/pulse analysis methods.
*/

#ifndef DMTPC_WAVEFORM_ANALYSIS
#define DMTPC_WAVEFORM_ANALYSIS

#include <vector>
#include "TMath.h"
class TH1;

/**Contains methods and classes for waveform analysis

*/

namespace dmtpc
{
namespace waveform
{

class CspWaveform;
class PMTWaveform;
class FastWaveform;
/**\brief Methods to extract information about pulses in an oscilloscope trace

This namespace contains functions to extract waveform analysis parameters.

*/
  namespace analysis{



/**Gets the baseline and baseline rms from the desired range of bins.
   \param hist the histogram
   \param rms the rms of the bin values (returned to user)
   \param binMin the first bin to use
   \param binMax the last bin to use
   \return the mean value of the bins
*/
    double baseline(const TH1* hist, double& rms, int binMin=1, 
                    int binMax=100);

/**Gets the minimum/maximum bin the histogram neglecting the last bin,
   the underflow, and the overflow.
   \param hist the histogram
*/
    int minbin(const TH1* hist);
    int maxbin(const TH1* hist);

/**Gets a set of rise times.  Determines the times at which the trace crosses the given list of
   heights.  The list should be in ascending order.

   fromStart determines how to deal with multiple crossings of these heights.  If true, it
   chooses the crossing closest to the start time and if false it chooses that closest to the end.
   \param hist the histogram
   \param list the list of heights
   \param values the values to be returned
   \param startTime the earliest time to look
   \param endTime the latest time to look
   \param fromStart the method to choose points

*/
    void riseTime(const TH1* hist, const std::vector<double>& list,
		  std::vector<double>& values, double startTime, double endTime,
		  bool fromStart=true);
/**Gets a set of fall times.  Determines the times at which the trace crosses the given list of
   heights.  The list should be in descending order.

   fromStart determines how to deal with multiple crossings of these heights.  If true, it
   chooses the crossing closest to the start time and if false it chooses that closest to the end.
   \param hist the histogram
   \param list the list of heights
   \param values the values to be returned
   \param startTime the earliest time to look
   \param endTime the latest time to look
   \param fromStart the method to choose points

*/
    void fallTime(const TH1* hist, const std::vector<double>& list,
                  std::vector<double>& values, double startTime, double endTime,
	          bool fromStart=false);

/**Determines whether or not a given bin is a peak (defined as being a local maximum within [bin-nbins,bin+nbins])
  \param hist the histogram
  \param bin the bin to check
  \param nbins the number of bins from the given one to look at.
  \return if bin is a peak
*/
    bool isPeak(const TH1* hist, int bin, int nbins=5);
/**Determines whether or not a given bin is a trough (defined as being a local minimum within [bin-nbins,bin+nbins])
  \param hist the histogram
  \param bin the bin to check
  \param nbins the number of bins from the given one to look at.
  \return if bin is a trough
*/
    bool isTrough(const TH1* hist, int bin, int nbins=5);

/**Takes the sum of all bins between the start and end times
  \param hist the histogram
  \param start the start time
  \param end the end time
  \return the histogram integral between these points (not taking the bin width into account)
*/
    double integral(const TH1* hist, double start, double end);

/**Determine where the pulse containing the seed bin first crosses above a threshold
   \param hist the histogram
   \param bin a seed bin
   \param threshold the value to cross
   \param startBin the number of the bin where the threshold is crossed.
   \param minBin the earliest bin allowed to check. -1 uses the first bin in the histogram
   \return the time at which the pulse first crosses above a threshold
*/
    double startTime(const TH1* hist, int bin, 
		     double threshold,int& startBin,int minBin=-1);
/**Determine where the pulse containing the seed bin first crosses below a threshold
   \param hist the histogram
   \param bin a seed bin
   \param threshold the value to cross
   \param endBin the number of the bin where the threshold is crossed.
   \param maxBin the last bin allowed to check. -1 uses the last bin in the histogram
   \return the time at which the pulse first crosses below threshold
*/
    double endTime(const TH1* hist, int bin, 
		     double threshold,int& endBin,int maxBin=-1);
    
/**Find a list of peaks between the given bin limits.  A peak here is 
   defined as the highest point in each pulse passing over the threshold.
  \param hist the histogram
  \param threshold the threshold for finding pulses
  \param minBin the first bin to use
  \param maxBin the last bin to use
  \return a list of bins containing the maximum value of each pulse
*/
    std::vector<int> peaks(const TH1* hist, double threshold,
                          int minBin=-1,int maxBin=-1);

/**Find a list of valleys between the given bin limits.  A valley here is 
   defined as the lowest point in each pulse passing below the threshold.
  \param hist the histogram
  \param threshold the threshold for finding pulses
  \param minBin the first bin to use
  \param maxBin the last bin to use
  \return a list of bins containing the minimum value of each pulse
*/
    std::vector<int> valleys(const TH1* hist, double threshold,
                            int minBin=-1,int maxBin=-1);

/**Find a list of local peaks and valleys between the two limits.  
   \param hist the histogram
   \param pkBin the list of peaks
   \param valBin the list of valleys (lowest points between two peaks. e.g. valBin[0] is the lowest bin between pkBin[0] and pkBin[1])
   \param minBin the first bin to use
   \param maxBin the last bin to use
   \param nbins the minimum half-width of peak

*/
    void peaksAndValleys(const TH1* hist,std::vector<int>& pkBin,
		         std::vector<int>& valBin, 
                         int minBin=-1,int maxBin=-1,int nbins=5);

/**Merge peaks together that are too close.  This is different than what is done in peaksAndValleys(), which requires a peak to be the highest point within a certain number of bins, while this requires that any two peaks must be some distance apart.
   \param hist the histogram
   \param pkBin the list of peaks
   \param valBin the list of valleys
   \param minDist the minimum distance in bins allowed between any two peaks

*/
    void mergePeaksByDistance(const TH1* hist,std::vector<int>& pkBin, 
			      std::vector<int>& valBin,int minDist=10);


/*
    void mergePeaksByDepth(const TH1* hist, std::vector<int>& pkBin, 
			   std::vector<int>& valBin,double minDepth=0.001);
*/

    void analyzeCSP(const TH1* h, CspWaveform& wf,Double_t gausConvSigma=20.);

    void analyzePMT(const TH1* h, PMTWaveform& wf,Double_t gausConvSigma=1.0);

    void analyzeFast(const TH1* h, FastWaveform& wf);

  }


}
}

#endif


