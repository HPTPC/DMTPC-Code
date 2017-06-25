#ifndef DMTPC_IMAGE_CALCULATIONS_HH
#define DMTPC_IMAGE_CALCULATIONS_HH

class TH1; 
class TH1F; 
class TH2; 
class TCanvas; 
class TCutG; 
class TGraph; 
class TPoints; 
#include "TString.h"

namespace dmtpc
{
  namespace image
  {
    namespace calculations
    {

     /** Determines the number of neighbors of a particular pixel which are above a given threshold
         \param image input image (1D)
         \param xbin test bin
         \param threshold threshold which neighbors must be above to be counted
         \param minNeighbors number of neighbors which the bin must have to return true
         \returns true if the number of bins above threshold is greater than or equal to minNeighbors; false otherwise

     */
    bool hasNeighbor(const TH1* image, int xbin, float threshold, int minNeighbors=2);

    /** Determines the number of neighbors of a particular pixel which are above a given threshold
         \param image input image 
         \param xbin test bin
         \param threshold threshold which neighbors must be above to be counted
         \param minNeighbors number of neighbors which the bin must have to return true
         \returns true if the number of bins above threshold is greater than or equal to minNeighbors; false otherwise

     */
    bool hasNeighbor(const TH2* image, int xbin, int ybin, float threshold, int minNeighbors=2);


    double calcSkewness(TH1* strip, int minx, int maxx, double bkg=0);
    double calcSkewness2D(TH2* image, int minbin, int maxbin, double bkg=0);
    double calcPixelDistance(const TH2* image, int i, int j);
    double calcIntensity2D(const TH2 *image, int imax, int jmax, int sidebandBins, float &bg);
    void countSegmentLength(const TH1 *project, float threshold, int pixelsBelowThreshold, int &iR, int &iL);
    void countSegmentLength2D(const TH2 *project, float threshold, int &iR, int &iL);

    int findMaximum(const TH1 *project, float threshold, int minNeighbors);
    int findMaximum2D(const TH2 *project, float threshold, int minNeighbors);

    /**
     * Rotate all points in a TCutG object by an angle, with respect to a point
     *
     *  cut     The input TCutG object to be rotated (it is altered in-place)
     *  ang_rad The angle by which to rotate cut [radians]
     *  pt      The pivot point (x,y) for the rotation (in data coordinates)
     *          By default, the pivot point is the average of the extreme
     *          x and y values in the TCutG object
     *
     *
     **/
    void rotateTCutG(TCutG *cut, float ang_rad, TPoints *pt=0);

    /**
     *  Get the center of an image in data coordinates (not bins)
     **/
    TPoints* getCenter(const TH2* img);

    /**
     *  Get the center of a TCutG object in data coordinates
     **/
    TPoints* getCenter(TCutG* cut);

    /**
     * Get the min, max values in x and y for a TCutG object
     *
     *  cut     The TCutG object to characterize
     *  xrng    (xmin, xmax) of the TCutG
     *  yrng    (ymin, ymax) of the TCutG
     **/
    void getExtent(TCutG* cut, TPoints* xrng, TPoints* yrng);
    

    /**
     * Count the number of pixels in an image that fall within a TCutG region
     *
     *  @param[in] img    = An image
     *  @param[in] cut    = A TCutG in that image
     *  @param[in] cutint = Include pixels interior or exterior to the cut 
     *                      (default is interior)
     **/
    int countPix(const TH2* img, TCutG* cut=0, bool cutint=true);

    /**
     *  code to handle the creation of rectangular TCutG objects
     **/
    TCutG* makeTCutGRect(TString name, float x0, float y0, float x1, float y1);

    /**
     * Make a annulus to approximate the ring or veto regions in the 4shooter
     * Does not assume that 
     *
     * @param[in] x0   -- center of circle, x-coordinate (data units)
     * @param[in] y0   -- center of circle, y-coordinate (data units)
     * @param[in] r1   -- Inner radius
     * @param[in] r2   -- Outer radius
     * @param[in] img  -- TH2 of image (used to get boundaries of sector)
     *
     **/
    TCutG* makeAnnulusSector(TString name, Float_t x0, Float_t y0, Float_t r1, Float_t r2, TH2* img);

    double maxProjectedStep(const TH2 * img, char axis =0);

    void principalAxes(const TH2 *image, float threshold, float &Ix, float &Iy);

    double cosRecoil2D(const  TH2 *image, int imax, int jmax);
    
    TH1F* makeProfile(TGraph *graph, int nx, float minx, float maxx, float miny, float maxy, TCanvas *c=0);
    
     /**
     *  Return the mean value of the pixels in an image
     *
     *  @param[in] image  - Image to be analyzed
     *  @param[in] cut    - TCutG to be applied to image
     *  @param[in] cutint - Include pixels interior or exterior to the cut 
     *                      (default is interior)
     *
     **/
    double getMean(const TH2* image, TCutG* cut=0, bool cutint=true);
    /**
     *  Return the RMS value of the pixels in an image
     *
     *  @param[in] image  - Image to be analyzed
     *  @param[in] cut    - TCutG to be applied to image
     *  @param[in] cutint - Include pixels interior or exterior to the cut 
     *                      (default is interior)
     *
     *
     **/
    double getRMS(const TH2* image, TCutG* cut=0, bool cutint=true);

    void meanRMSNoOutliers(const TH2* image, double& mean, double& rms);

    TH1F * pixelHist(const TH2* image, int nbins=100, Float_t minVal=9999, Float_t maxVal=-9999, TCutG* cut=0, bool cutint=true);
    
    int distanceToImageEdge(const TH2 *image, int ibin);

    double rectIntersectionArea(double * xcoords1, double * ycoords1, double * xcoords2, double * ycoords2); 

    double polygonArea(int n, const double * x, const double * y, bool ordered = true); 

    double median(const TH2 * img); 

    int countUniqueValues(const TH1 * in); 



    }
  }
}

#endif
