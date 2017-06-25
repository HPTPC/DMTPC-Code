#ifndef DMTPC_ANALYSIS_STITCHER_HH
#define DMTPC_ANALYSIS_STITCHER_HH

#include <vector>
#include <iostream>
#include <utility>
#include <math.h>
#include <TNamed.h>
#include <TMath.h>
#include "TObjString.h"
#include "StitchInfo.hh"
#include "LensCorrection.hh"
#include <TH2D.h>
#include <TH2F.h>
#include <TH2C.h>
#include <TH1.h>

namespace dmtpc
{
namespace analysis
{

class Stitcher : public TNamed
{

  public:
    /** Constructor. A name may be provided */
    Stitcher(const char * name = 0); 


    /**Destructor*/
    virtual ~Stitcher();

    /** Returns true if the stitcher has been trained */
    bool isInit() const { return _isInit; } 
    /* Train the stitcher.
     * @param images a vector of TH2 to train the stitcher. These must be images where the features (ring and spacers) are
     *               clearly visible. 
     * @param turns  an array of approximate image rotations (NONE, QUARTER_LEFT,QUARTER_RIGHT or HALF_TURN)
     * @param print_progress  print out progress related outout
     * @returns true if successful 
     * */
    bool train(const std::vector <const TH2*> * images, const TString * turns, const TString * serial_numbers, bool print_progress=false); 

    bool setupMCViper(int n, char ** serial_numbers, double * xcenters, double * ycenters, double * scales, double * rotations, double * innerRadius, double * outerRadius);

    /** Stitch the given images. They must be in the same order and in the same number 
     * as the training set */ 
    TH2* stitch(const std::vector <const TH2*> * images, const char * interpolation = "bicubic") const; 

    /** Returns the xcenter of the ring in the local coordinates of the ith image */
    double xCenter(int i) const { return _xCenters[i];} 

    /** Returns the ycenter of the ring in the local coordinates of the ith image */
    double yCenter(int i) const { return _yCenters[i];} 

    /** Returns the estimated inner radius (in pixels) of the ring structure (this is the veto region) */
    double innerRadius(int i) const { return _innerRadius[i];} 

    /** Returns the estimated outer radius (in pixels) of the ring structure (this is the veto region) */
    double outerRadius(int i) const { return _outerRadius[i];} 
    
    /** Returns the number of spacers detected in the ith image */ 
    unsigned getNSpacers(int i) const { return _spacers_r[i].size();}

    /** Returns the angle from vertical of the sth spacer in the ith image */ 
    double getSpacerTheta(int i, int s) const { return _spacers_theta[i][s];}

    /** Returns the distance of closest approach from the local coordinate origin of the sth spacer in the ith image */ 
    double getSpacerR(int i, int s) const { return _spacers_r[i][s];}

    /** Returns the slope of the sth spacer in the ith image */
    double getSpacerSlope(int i, int s) const { return -1./tan(_spacers_theta[i][s]);}

    /** Returns the y intercept of the sth spacer in the ith image */
    double getSpacerIntercept(int i, int s) const { return _spacers_r[i][s]/TMath::Sin(_spacers_theta[i][s]);}

    /** Set the weights for stitching. These should correspond to the (possibly only relative) gains of the images. 
     * Overlap regions are weighted according to these wights */
    void setWeights(const double * weights); 

    /** Set the lens correction for the images. This class will become the owner of it. */ 
    void setLensCorrection( dmtpc::image::LensCorrection * lens) { _lens = lens; }

    /** Sets the blur level for the edge detection (default 0.8) */ 
    void setBlurLevel(double b) { _edge_blur_level = b; }

    /** Sets the lower threshold level (in units of gradient RMS over mean) for the edge detection  (default 0) */ 
    void setEdgeLowThreshold(double t) { _edge_low_thresh = t; }

    /** Sets the upper threshold level (in units of gradient RMS over mean) for the edge detection  (default 3) */ 
    void setEdgeHighThreshold(double t) { _edge_high_thresh = t; }

    void setMinEdgeNeighbors(int n) { _min_edge_neighbors = n; } 

    /** Set the number of r bins for the linear hough transform (default 1024 */ 
    void setLinearHoughRbins(int nrbins) { _linear_hough_r_bins = nrbins; } 

    /** Set the number of theta bins for the linear hough transform (default 1024 */ 
    void setLinearHoughThetabins(int nthetabins) { _linear_hough_theta_bins = nthetabins; } 

    /** Set the minimum number of hough votes for a spacer (default 350). Roughly corresponds to minimum length of a line. */ 
    void setLinearHoughMinVotes(int nvotes) { _linear_hough_min_votes = nvotes; } 

    /** Set the threshold for merging lines in parameter space to form spacers  (default, r=20, theta=0.3)*/ 
    void setSpacerJoinThresholds(double rthresh, double theta_thresh){ _spacer_join_theta_thresh = theta_thresh; _spacer_join_r_thresh = rthresh; }

    /** Set the number of bins for each parameter for the first pass hough transform  (default 20,20,400)*/ 
    void setCircularHoughFirstPassNbins(int nxbins, int nybins, int nrbins) { _nbins_first_pass[0] = nxbins; _nbins_first_pass[1] = nybins; _nbins_first_pass[2] = nrbins; }

    /** Set the number of bins for each parameter for the second pass hough transform (default 80,80,1000)*/ 
    void setCircularHoughSecondPassNbins(int nxbins, int nybins, int nrbins) { _nbins_second_pass[0] = nxbins; _nbins_second_pass[1] = nybins; _nbins_second_pass[2] = nrbins; }

    /** Set the lower limits of the circular hough transform search (defaults -200, 800, 800) */ 
    void setCircularHoughMins(double xmin, double ymin, double rmin) { _mins_first_pass[0] = xmin; _mins_first_pass[1] = ymin; _mins_first_pass[2] = rmin;}

    /** Set the upper limits of the circular hough transform search (defaults -200, 800, 800) */ 
    void setCircularHoughMaxs(double xmax, double ymax, double rmax) { _maxs_first_pass[0] = xmax; _maxs_first_pass[1] = ymax; _maxs_first_pass[2] = rmax;}

    /** Set the number of bin widths to zoom in around the center on for the second pass. The r coordinate still uses
     * the same limits as set for the first pass as the r dimension is not looped over (default 1) */ 
    void setNWidthsSecondPass(double w) {_nwidths_second_pass = w;}

    /** Set the number of peaks allowd in the r projection for the TSpectrum  (default 6) */ 
    void setNSpectrPeaksR(int n) { _nspectr_peaks = n; }

    void setMedianNbins(int nbins) { _median_nbins = nbins; }
    void setMedianNIter(int niter) { _median_niter = niter; }

    void setCCDWithLED(int n) { _led = n; } 
    void setLEDBorderWidth(int n) { _led_border = n; } 
    void setLEDThresh(double t) { _led_thresh = t; } 
    void setImageHighThresh(double t) { _img_high_thresh = t; } 

    void setScales(const double * scales, bool update_stitch = true); 
    void setRotations(const double * rotations, bool update_stitch = true); 
    void setCenters(const double * Xcenters, const double * Ycenters, bool update_stitch = true); 

    double getScale(int i) const { return _scales[i]; }
    double getRotation(int i) const { return _rotations[i]; } 
    int getIndex(const char * serial) const; 
    const char *  getSerial(unsigned index) { return _serials[index].String().Data(); } 

    enum SCALE_METHOD
    {
      FIXED,
      INNER,
      OUTER,
      BOTH
    };

    const dmtpc::image::StitchInfo * getStitchInfo256() const { return _stitch_info_256; } 
    const dmtpc::image::StitchInfo * getStitchInfo512() const { return _stitch_info_512; } 
    const dmtpc::image::StitchInfo * getStitchInfo1024() const { return _stitch_info_1024; } 
    const dmtpc::image::StitchInfo * getStitchInfo(unsigned nbins) const { return nbins == 256 ? _stitch_info_256 : 
                                                                         nbins == 1024 ? _stitch_info_1024 : 
                                                                         nbins == 512 ? _stitch_info_512 : 
                                                                         _stitch_info_other; } 

    /** Write an overlay file for the ith image with the spacers and the rings
     *  @param file the name of the output file 
     *  @param i the image index to use 
     *  @param ncirclesegments the number of lines to make up the circle 
     * */ 
    void writeOverlayFile(const char * file, int i, int ncirclesegments=90); 


    TH2 * polar(int i ) { return polars[i]; }
    TH1 * rproj(int i ) { return rprojs[i]; }
    TH2 * median(int i ) { return medianed[i]; }
    TH2 * original(int i ) { return orig[i]; }
    TH2 * edge(int i ) { return edges[i]; }

    void setScaleMethod(SCALE_METHOD method) { scaleMethod = method; } 
    void calculateAuxHistograms(int i, const std::vector<const TH2*> * images, bool progress); 
  private:
    void updateStitchInfo(); 

    bool _isInit; 
    unsigned _nImages;
    double _edge_low_thresh; 
    double _edge_high_thresh;
    double _edge_blur_level;
    int _min_edge_neighbors;

    double _img_high_thresh;

    int _linear_hough_r_bins;
    int _linear_hough_theta_bins;
    int _linear_hough_min_votes; 

    double _spacer_join_theta_thresh;
    double _spacer_join_r_thresh; 

    int _nbins_first_pass[3];
    int _nbins_second_pass[3]; 
    double _mins_first_pass[3]; 
    double _maxs_first_pass[3];
    double _nwidths_second_pass;
    int _median_nbins;
    int _median_niter;
    SCALE_METHOD scaleMethod; 

    int _led;
    double _led_thresh;
    double _led_border;

    double _spacer_theta_thresh; 
    int _nspectr_peaks; 

    std::vector<double> _xCenters; 
    std::vector<double> _yCenters; 
    std::vector<double> _innerRadius; 
    std::vector<double> _outerRadius; 
    std::vector<double> _weights; 
    std::vector<std::vector<double> >_spacers_r; 
    std::vector<std::vector<double> >_spacers_theta; 
    std::vector<double> _rotations; 
    std::vector<double> _scales; 

    dmtpc::image::StitchInfo * _stitch_info_1024; 
    dmtpc::image::StitchInfo * _stitch_info_512; 
    dmtpc::image::StitchInfo * _stitch_info_256; 
    mutable dmtpc::image::StitchInfo * _stitch_info_other; //!

    std::vector<TH2F * > medianed; //!
    std::vector<TH2C * > edges;  //!
    std::vector<TH2D * > polars;//!
    std::vector<TH1 * > rprojs;//!
    std::vector<TH2F * > orig;//!
    std::vector<TObjString> _serials; 
    dmtpc::image::LensCorrection * _lens; 

    void calcRadii(TH1 * rproj, double & inner_r, double & outer_r, bool progress = false); 


  ClassDef(Stitcher,1); 
};

}
}


#endif
