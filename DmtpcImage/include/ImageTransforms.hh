#ifndef DMTPC_IMAGE_TRANSFORMS_HH
#define DMTPC_IMAGE_TRANSFORMS_HH  

class THnSparse;
class TH3; 
class TPoints; 
class TH2; 
class TH2D; 
class TH1; 

#include "TString.h" 

namespace dmtpc
{
 namespace image
 {
   class StitchInfo; 

   namespace transform
   {
      /** Copy histogram and return a histogram rotated
       *  by a multiple of PI/2.
       *
       *  @param[in] hin the histogram to rotate
       *  @param[in] dir, the direction. May be one of: 
       *          "QUARTER_RIGHT"
       *          "QUARTER_LEFT"
       *          "HALF_TURN"
       *          "NONE"
       * */
      TH2* rotatePerfect(const TH2 *hin, TString dir);

      /** Copy and rotate histogram to the right 
       *   @param image the histogram to rotate 
       **/
      TH2* rotateRight(const TH2* image);

      /** Copy and rotate histogram to the left 
       *   @param image the histogram to rotate 
       **/
      TH2* rotateLeft(const TH2* image);

      /** Copy and rotate histogram 180 degrees 
       *   @param image the histogram to rotate 
       **/
      TH2* rotate180(const TH2* image);

      /**
       *  Rotate an image by an arbitrary amount using a subdivision algorithm.
       *  
       *  WARNING: algorithm seems to do worst when rotation is multiples 
       *           of pi/4
       *  
       *  The rotation is counter-clockwise and the pivot point is set by the
       *  input parameter "pt"
       *  
       *  Algorithm is to loop over all bins.  Sub-divide the bins into
       *  N=ndiv^2 sub-bins and then rotate each sub-bin. This spreads the
       *  "flux" in each source bin among several destination bins.  There
       *  is a tradeoff between accuracy and speed here.  Typically if ndiv
       *  = 8 or 16 the quality is fine (at least to my eye).  More
       *  detailed investigations needed to really quantify the quality of
       *  the rotation.
       *  
       *  @param[in] img  = Input image to be rotated.
       *  @param[in] ang  = Angle of counter-clockwise rotation [radians]
       *  @param[in] pt   = Point around which the rotation is applied.  
       *                    In "real" units, not bin number.
       *                    If omitted or zero, the center of the image is used.
       *  @param[in] ndiv = Each source pixel will be divided into ndiv^2 
       *                    sub-pixels.
       *  @param[in] reg  = A sub-region of the image to rotate.  If not specified
       *                    or zero, then the entire image is rotated.
       *                    [NOT YET IMPLEMENTED]
       *  @return           The rotated image.
       *
       *  Implementation in MaxCamImageTools_ImageTransforms.cc
       */
      TH2* rotateSubdivide(const TH2* img, double ang, TPoints* pt=0, Int_t ndiv=4);


      /** 
      * Rotate an image by an arbitrary amount using an interpolation algorithm. 
      *
      * The rotation is counter-clockwise and the pivot point is set by the input parameter "pt"
      *
      * Algorithm is to create the output image, loop over the bins in the output image and for each bin center, transform back 
      * into the original frame and interpolate the value at that point. 
      *
      * @param[in] img  Input image 
      * @param[in] ang  Angle ro rotate by 
      * @param[in] pt   Pivot point, or 0 to use center of image 
      * @param[out] out Pointer to output image, or 0 to create a new one. 
      * @param[in] interpolation  Interpolation algorithm to use. See dmtpc::image::interpolation::interpolate for available types
      *
      * @return  The rotated image
      **/ 
      TH2* rotateInterpolate(const TH2 *img, double ang, TPoints *pt = 0, TH2 *out = 0, const char * interpolation = "bilinear"); 

      void projectAlongLine (const TH2 * img, TH1 ** longi, TH1 ** tranv, double x0, double y0, double x1, double y1, double width, const char * method = "bilinear") ; 

      /** Resize an image axes keeping the same number of bins.
          The content is copied bin-by-bin into new coordinates.
          @param[in]  image the original image 
          @param[in]  xmin new x min
          @param[in]  xmax new x max
          @param[in]  ymin new y min
          @param[in]  ymax new y max
          @return     The resized image
       **/
      TH2* resizeImage(const TH2* image, double xmin, double xmax, double ymin, double ymax);

      /** Copy bin content from one histogram to another. Ignores overflow and underflow bins. 
       *   
       *   @param[in] in  the input image
       *   @param[out] out the output image. Should have the same binning (or perhaps more bins) but the axes may be different.
       *   @return    Pointer to out
       * */ 
      TH1*  copyContent(const TH1* in, TH1* out);
 
      /** Performs a hough transform on the input image looking for a line 
       * 
       *  The output format is a 2-dimensional histogram of line votes where
       *  the X axis is the angle of the line and the Y axis is the distance from 0,0 of the line (y = -cot(#theta) + r / sin(#theta))
       *
       *  You may pass an output histogram (assumed to be filled with zeros) with the range and binning you want or one may 
       *  be generated if you pass 0 with nangbins (ranging from 0 to 2 pi) and nrbins (ranging from 0 to the diagonal distance). 
       *  A TH2I or a TH2D will be generated depending on if the input is float or integer type. 
       *
       *   @param[in] in the histogram to transform
       *   @param[out] out either a histogram to use for the output (that defines the parameter ranges) or 0 to allocate a new one. 
       *   @param[in] min The minimum threshold in the input image to add a curve in hough space
       *   @param[in] nangbins if we allocate a new histogram, this is the number of angle bins, otherwise this is ignored
       *   @param[in] nrbins if we allocate a new histogram, this is the number of r bins, otherwise this is ignored
       *   @return the hough transform of the image, with angle on the x axis and distance from smallest corner on the y axis
       *
       */ 
      TH2* houghTransformLine(const TH2* in, TH2* out = 0, double min = 1,  unsigned nangbins = 512, unsigned nrbins=512); 

      /**Parameter order for houghTransformEllipse. 
       * order: xcenter, ycenter, minor, major, rotation_angle (phi)
       **/ 
      typedef enum 
      {
        ELL_X0,ELL_Y0,ELL_A,ELL_B,ELL_PHI
      } ELLIPSE_PARAMETERS; 

      /** Naive implementation of ellipse hough transform. Use may be impractical due to storage
       *  and computation requirements. The accumulator is implemented as a 5-dimensional sparse 
       *  histogram with B (one of the axes) as the "independent" coordinate (the others will always be filled
       *  if there exists a valid value of b)
       *
       *  @warning This hasn't been tested yet...
       *
       *  The output array must be prepared ahead of time with the number of bins and limits for each parameters. 
       *  The parameter order is defined the ELLIPSE_PARAMETERS enum. For example, you may do:
       *
       *  double nbins[5], min[5], max[5]; 
       *  nbins[ELL_X0] = ...  
       *  min[ELL_X0] = ...  
       *  max[ELL_X0] = ...  
       *  ... 
       *  THnSparse * out = new THnSparse{F,D,I,S,C}("hist","hist",5,nbins,min,max); 
       *  houghTransformEllipse(in,out); 
       *
       *  @param[in] in The histogram to find ellipses in
       *  @param[out] out a prepared output 5d sparse histogram that includes the number of bins and limits you want
       *  @param[in] min The minimum input histogram value to fill out a manifold in hough space
       *  @return pointer to out (the accumulator)
       *
       */
      THnSparse * houghTransformEllipse(TH2* in, THnSparse * out, double min = 1); 

      /** Circle hough transform. Accumulator is implemented as a 3-dimensional histogram with
       *  r as the "independent" coordinate. 
       *
       * The output histogram must be prepared ahead of time with the number of bins and limits for each paramter.
       * The parameter order (x,y,z) is x0, y0, r. 
       *
       *  ... 
       *  TH3 * out = new TH3IF,D,I,S,C}("hist","hist",nbinsx,xmin,xmax,nbinsy,ymin,ymax,nbinsr,rmin,rmax);  
       *  houghTransformCircle(in,out); 
       *
       *  @param[in] in the histogram to find cirlces in 
       *  @param[out] out the prepared output 3d 
       *  @param[in] min The minimum input histogram value to fill out a manifold in hough space
       *  @return pointer to out (the accumulator) 
       *
       *   Implementation in MaxCamImageTools_ImageTransforms.cc 
       */
      TH3 * houghTransformCircle(TH2* in, TH3 * out, double min = 1, void (* progressFn)(int row) = 0); 


      /** Convert an image into polar coordinate by back-interpolation.
       *  
       *  The output histogram is generated and for each bin center in the output, the reverse transform and an interpolation is performed
       *  to compute the value of the point in the original image.  
       *
       *  @param[in] in The image to convert
       *  @param[in] nrbins the number of bins in r in the output image
       *  @param[in] nrbins the number of bins in theta in the output image 
       *  @param[in] center The origin that r and theta will be defined about. Pass a null pointer to use the center of the image. 
       *  @param[in] type   A char specifying the type of the output (e.g. 'D' for TH2D). 
       *  @param[in] interpolation The interpolation algorithm to use (see dmtpc::image::interpolation::interpolate() for possible types). 
       *  @param[in] name  The name of the output histogram, or NULL to use the input with "_polar" appended
       *  @return    The transformed image 
       *
       * */
      TH2 * toPolarCoordinates(const TH2 * in, unsigned nrbins = 256, unsigned nthetabins = 256, 
                              TPoints * center = 0, char type = 'D', const char * interpolation = "bilinear", const char * name = 0);


      /** Build a composite image (TH2) from multiple images (TH2's). 
      *  
      *  @param in array of pointers to histograms to use
      *  @param info StitchInfo with stitching parameters 
      *  @param interpolation the interpolation method. Currently only bilinear,bicubic and nearest neighbor are supported. 
      *  @param histType the output histogram type. 'C',S','I','F' and 'D' are supported. 
      *
      */
      TH2* montage(const TH2 * const* in, StitchInfo * info, const char * name, 
                 const char * interpolation = "bilinear", char histType ='F');


      /** Perform a radon transform of the image 
       *  @param img the image to perform the radon transform of  
       *  @param nbinstheta number of angle bins 
       *  @param nprojbins number of bins in each projection, or leave negative to automatically set to (max(xwidth,ywidth) * sqrt(2) / binwidth)
       *  @return the transformed image 
       */ 
      TH2D * radonTransform (const TH2 * img, int nbinstheta = 360, int nprojbins = -1); 


      /** Crop the image to the given bin bounds */ 
      TH2 * crop(const TH2 * img, int xbinmin, int xbinmax, int ybinmin, int ybinmax); 

      /** Crop the image to non-zero part*/ 
      TH2 * cropZero(const TH2 * img); 

     /**Zero pad the input histogram to the size newx, newy; 
      
      The newxpos and newypos define the position of the original within the new image
      where -1 = left align, 1 = right align, 0 = centered
     **/
     TH2* zeroPad(TH2 * in, int newx, int newy, float newxpos = 0, float newypos = 0, const char * name = "_zeropad"); 

     /**Zero pad the input histogram to be a square (with the size given by the larger axis); 
      
      The newpos defines the position of the original smaller axiswithin the new image
      where -1 = left align, 1 = right align, 0 = centered
     **/
   
     TH2* zeroPadSquare(TH2 * in, float newpos = 0, const char * name = "_zeropad"); 

     /** apply the function fn(value) to histogram (destructive)*/ 
     TH2* histApply (TH2 * in, double (*fn) (double)); 

     /** apply the function fn(value,arg) to histogram (destructive)*/ 
     TH2* histApply (TH2 * in, double (*fn) (double,double), double arg); 

     /** apply the square root function to histogram (destructive)*/ 
     TH2* histSqrt (TH2 * in); 

     /** apply the abs function to histogram (destructive)*/ 
     TH2* histAbs (TH2* in);

     /** raise each value in the histogram to the b power (destructive)*/ 
     TH2* histPow (TH2* in, double b);

     /** Shifts the quadrants of a 2D FFT so that the 0 frequency bin is in the center
      **/
     TH2 * fftshift (TH2 * in, double scalex = 1, double scaley = 1, bool normalize = false); 

     TH2* padImage(const TH2* image, int padfactor); 
    }
  }
} 


#endif 
