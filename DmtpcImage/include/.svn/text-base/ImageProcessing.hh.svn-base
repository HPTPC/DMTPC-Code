#ifndef DMTPC_IMAGE_PROCESSING_HH
#define DMTPC_IMAGE_PROCESSING_HH

class TH2; 
class TH2S; 
class TH2C; 

#include <vector>
#include <cstring>
#include "TString.h" 

namespace dmtpc
{
  namespace image
  {
    namespace processing
    {

      /** types of bilateral filter value function **/ 
      typedef enum
      {
        BILATERAL_GAUSSIAN,
        BILATERAL_CAUCHY, 
        BILATERAL_BOX, 
        BILATERAL_TRIANGLE, 
        BILATERAL_TUKEY,  
      } BILATERAL_VALUE_FN;



      /** Applies an overall threshold to an image; sets all bins not above the threshold to zero
           @param image input image
           @param threshold threshold      
      */
      void applyThreshold(TH2* image, float threshold);
 

      /** Applies an overall ceiling to an image; sets all bins not below ceiling to zero
           @param image input image
           @param ceiling ceiling      
       */
      void applyCeiling(TH2* image, float ceiling);

      /** Set to average all pixels that are above threshold
           @param[in] image input image
           @param[in] threshold threshold above which all pixels will be set to the image mean
           @param[out] killedPixels pointer to vector of killed pixels to populate, or NULL if none is needed
           @return number of pixels killed
      */
      int killPixels(TH2* image, float threshold, std::vector<int>* killedPixels=NULL);

      /**Set to zero all pixels that are below the threshold, or have no neighbors above the threshold.
       \param image input image
       \param threshold threshold
       \param minNeighbors minimum number of neighbors a pixel must have to survive
       \return number of pixels killed
      */
      int killLonePixels(TH2* image, float threshold, int minNeighbors=2);

      int killLonePixelsMedianDifference(TH2 * tokill, double threshold); 

      /**Set to the image mean all pixels that are below the threshold, or have no neighbors above the threshold.
         \param image input image
         \param threshold threshold
         \return number of pixels killed
      */
      int killLonePixels2(TH2* image, float threshold,std::vector<int>* killedPixels=NULL);
      

       /** set a vector of pixels to zero
           \param image input image
           \param pixellist list of pixels to kill
           \return number of pixels killed
       */
        int killPixelList(TH2* image, std::vector<int> pixellist);

        /* kills pixels outside of a designated region of interest. The region is rectangular.
        \param image input image 
        \param roi vector of bins containing pixels to be saved
        \param d number of bins to preserve outside of the roi
        \param opt choose x or y to kill the bins in
        */
        int killUnusedPixels(TH2* image, std::vector<int> &roi, int d, TString opt);

        /** Sets a row in a 2d image to zero
            @param image input image
            @param irow the row to kill
        */
        int killRow(TH2* image, int irow);

        /** Sets a column in a 2d image to zero
           \param image input image
           \param icolumn the column to kill
        */
        int killColumn(TH2* image, int icolumn);

       /** subtracts a constant from the value of every bin in the image
         \param image input image
         \param pedestal constant pedestal to be subtracted
        */
        void subtractPedestal(TH2* image,float pedestal);
    

       TH2* topHatBlur(TH2* image, int blurn, double blurfrac);

       typedef enum
       {
         RENORMALIZE,
         MIRROR_EDGES, 
         EXTEND_EDGES,
         ZERO_EDGES
       }BLUR_EDGE_BEHAVIOR;


      /** 
       * Return the image blurred witha  gaussian kernel of size (2*blurn+1) X (2*blurn+1) and sigma sigma. 
       * The fourth parameter allows you to store the resulting kernel so that it needn't be calculated 
       * each time. 
       *
       * Note that the last specifed, non-saved kernel is cached, so repeated calls with the same parameters
       * should be quick. Unless you have a threaded program... 
       *
       * @param image the image to blur
       * @param the number of bins on each side used in the gaussia kernel
       * @param sigma the standard deviation, in bins, of the gaussian kernel
       * @param kernel if this parameter is NULL, the kernel is calculated and not saved. 
       * If it is a pointer to a double * that is set to NULL, the kernel is calculated and 
       * saved to that location to that memory location (it is the responsibility of the user
       * to free it using the free(). If this parameter is a pointer to a double * that is not equal to NULL, then 
       * the existing kernel is used. It is assumed that it is of size 2*blurn+1 otherwise you may experience a segfault. 
       * @param edge_action specifies what to do when the kernel hits an edge
       *  
       */ 
      TH2 * gaussianBlur(const TH2* image, int blurn, double sigma, double ** kernel = NULL, BLUR_EDGE_BEHAVIOR edge_action = RENORMALIZE) ;


      TH2 * bilateralFilter(const TH2 * image, double space_sigma, double value_sigma, double nsigma=3, BILATERAL_VALUE_FN fn = BILATERAL_GAUSSIAN); 
      TH2 * fastBilateralFilter(const TH2 * image, double space_sigma, double value_sigma, double nsigma=3, BILATERAL_VALUE_FN fn = BILATERAL_GAUSSIAN, int scale_exp = 8, bool cache_lookup = false); 

      typedef enum
      {
        SOBEL,
        PREWITT, 
        SCHARR
      }GRADIENT_OPERATOR; 

      /** Canny edge detector. 
      *
      *  Uses nxn gaussian kernel with the gradient operator of your choice and hysteresis thresholding. 
      *  
      *  \param img An image to undergo edge detection 
      *  \param blurlevel Sigma of gaussian blur filter
      *  \param thresh_low Lower hysteresis threshold
      *  \param thresh_high Higher hysteresis threshold 
      *  \param g 3x3 gradient operator to use. 
      *  \param kernel_size The radius of the gaussian kernel to convolve with 
      *  \returns a TH2C with 0 for no edge and 1 for an edge. The histogram name is the input hist's name
      *   with a _thresh suffix. 
      *
      *  Implementation in MaxCamImageTools_ImageProcessing.cc 
      * */

      TH2C* edgeDetect(const TH2* img, double blurlevel, double thresh_low, double thresh_high, GRADIENT_OPERATOR g = SOBEL, unsigned int kernel_size = 7); 

      /** 2D Convolution
      *
      *  Creates a new image convolved with the specified kernel  
      *  
      *  \param img The image to convolve    
      *  \param kernel A collapsed array specifying the kernel. kernel[row][column] should be kernel + (width * row) + column. Note that if 
      *                the convolution is separable, the height is ignored and the kernel will be treated as one dimensional 
      *  \param width the width of the kernel 
      *  \param height the height of the kernel 
      *  \param linear treat the convolution as separable (i.e. equivalent to applying in x followed by y)
      *  \param edge_behavior what to do with the kernel by an edge
      *  \param new_name The name of the the resultant image. If NULL, the name will be the current name with _convolved appended
      *
      *  \return the convolved image
      */
      TH2 * convolve(const TH2* img, const double *kernel, int width, int height, bool separable = false , BLUR_EDGE_BEHAVIOR edge_behavior = RENORMALIZE, const char * new_name = 0); 

      /** Gradient, as used by canny edge detector
      *
      *  \param img the image to analyze
      *  \param magnitude this histogram will be filled with the magnitude of the gradient 
      *  \param orientation this histogram will be filled with the rounded axial orientation. 
      *           the orientation will be rounded to 0, 45, 90, and 135 degrees. 
      *  \param blurlevel sigma of gaussian blur filter 
      *  \param g 3x3 gradient operator to use 
      *  \param kernel_size radius of gaussian kernel to convolve with 
      *
      */ 
      void gradient(const TH2 * img, TH2 * magnitude, TH2S * orientation,  double blurlevel, GRADIENT_OPERATOR g = SOBEL, unsigned int kernel_size = 7); 
   

      typedef enum 
      {
        GAUSSIAN, 
        LORENTZIAN,
        TUKEY
      } ANISOTROPIC_DIFFUSION_FN; 


       /** Anisotropic Diffusion using the algorithm suggested by Perona and Malik for smoothing while preserving features. 
       *  See http://dx.doi.org/10.1109%2F34.56205 for paper 
       *
       *  \param img The image to perform the operation on 
       *  \param lambda Lambda parameter. Related to amount of diffusion. 0 < lambda <=1 
       *  \param K K parameter, related to strength of edge sensitivity
       *  \param fn The edge sensitivity function to use. Right now, choices are Gaussian, Lorentzian and Tukey Bilevel. 
       *  \param gradient_sigma the blur amount for computing the sigma 
       *  \param gradop The gradient operator for performing the gradient 
       *
       **/
      TH2* anisotropicDiffusion(const TH2* img, double lambda, double K, ANISOTROPIC_DIFFUSION_FN fn = TUKEY, double gradient_sigma = 1, GRADIENT_OPERATOR gradop = SOBEL, bool use_diagonals = false); 

      

      /** Suppress all values that are not local maxima 
       * 
       * \param in input histogram
       * \param n  the number of pixels in neighborhood to check
       * \param setval the value to set nonmaximal values to 
       * \param suffix the suffix appended to the name of the original
       * \return a suppressed histogram
       *
       * */
      TH2* nonMaximumSuppress(const TH2 * in, int n =1, double setval =0, const char * suffix = "_suppressed"); 

      TH2 * medianFilter(const TH2 * in, unsigned width =3, unsigned niter = 1); 
      TH2 * laplacian (const TH2 * in); 
      TH2 * laplacianOfGaussian (const TH2 * in, double sigma, int width); 
      void fillEdges(TH2 * in, unsigned width = 1, double setval = 0); 
      TH2 * neighborRatio(const TH2 * in, bool abs = false, bool median = false, bool difference = false); 

      void addGaussianNoise(TH2 * in, double sigma = 1); 
      TH2C * binaryThreshold(const TH2 * in, double thresh); 

      TH2 * WienerGaussDeconvolve(const TH2 * s, double sigma, double noise_sigma); 
    }
  }
}


#endif
