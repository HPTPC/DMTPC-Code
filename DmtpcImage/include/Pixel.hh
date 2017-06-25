#ifndef DMTPC_IMAGE_PIXEL_PROCESSING_HH
#define DMTPC_IMAGE_PIXEL_PROCESSING_HH

class TH2; 
class TH2I; 
#include <set>
#include <vector>

namespace dmtpc
{
  namespace image
  {
    namespace pixel
    {

      /** Determines the neighbors of a particular pixel which are above a given threshold
          \param image input image 
          \param ibin test bin
          \param threshold threshold which neighbors must be above to be counted
          \returns a vector of the bin numbers that are above threshold
      */
      std::vector<int> findNeighbors(TH2 *image, float threshold, int ibin);

       /** takes an image, usually an averaged image, and finds arrays of pixels that are hot or cold
           \param image input image
           \param low threshold below which pixels are "cold"
           \param high threshold above which pixels are "hot"
           \param cold return variable for array of cold pixels
           \param hot return variable for array of hot pixels
           \param ncold return varible for number of cold pixels
           \param nhot return variable for number of hot pixels

      */
      void findHotCold(TH2* image, float low, float high, int* cold, int* hot, int &ncold, int &nhot);
     
      int clustWidth(std::vector<int> clusta, TH2* image);
      
      std::vector<int> halfway(std::vector<int> clusta, std::vector<int> clustb, TH2* image, double mindist);
      
      double minDist(std::vector<int> & clusta, std::vector<int> & clustb, const TH2* image);

      int countPixelsAboveThreshold(TH2* image, float threshold);

      /** Implementation in MaxCamImageTools_PixelProcessing.cc*/
      int selectPixelsAbove(const TH2 * img, std::set<int> * pixels, double thresh); 

      /** Implementation in MaxCamImageTools_PixelProcessing.cc*/
      int selectPixelsBelow(const TH2 * img, std::set<int> * pixels, double thresh); 

      /** Implementation in MaxCamImageTools_PixelProcessing.cc*/
      int selectPixelsBetween(const TH2 * img, std::set<int> * pixels, double thresh_low, double thresh_high); 

      /** 
       * Adds all pixels in the image but not inside in to out
       *
       * Returns number of pixels added to out; 
       *
       **/
      int getInversePixels(const TH2 * img, const std::set<int> * in, std::set<int> * out);  

       /** Adds all pixels that are at least partiall contained within the box to the set *
        *
        */ 
       int selectPixelsInsideBox(const TH2 * img, std::set<int> * pixels, double xmin, double ymin, double xmax, double ymax); 

       /** Adds all pixels whose centers are contained within the circle to the set
        *
        */ 
       int selectPixelsInsideCircle(const TH2 * img, std::set<int> * pixels, double x0, double y0, double r); 

       int selectPixelsAboveWithAtLeastNNeighbors(const TH2 * img, std::set<int> * pixels, double thresh, int n); 
       int getNeighborsAboveThreshold(const TH2 * img, int bin, std::set<int> * pixels, double thresh); 


       bool fullyConnected(const TH2 * img, std::set<int> * pixels); 
       

       int dilate(const TH2 *img, std::set<int> * pixels); 

       int erode(const TH2 *img, std::set<int> * pixels); 

       void fillPixels(TH2 *img, const std::set<int> * pixels, double val); 

       int innerBorder(const TH2 *img, const std::set<int> * pixels, std::set<int> * out); 

       int outerBorder(const TH2 *img, const std::set<int> * pixels, std::set<int> * out); 

       TH2I * distanceTransform(const TH2* in, const std::set<int> * pixels); 
 
       bool fullyConnected(const std::set<int> * pixels, const TH2 * img); 
    }
  }
}


#endif
