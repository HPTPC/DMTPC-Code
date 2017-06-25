#ifndef DMTPC_LENS_CORRECTION_HH
#define DMTPC_LENS_CORRECTION_HH

#include "TNamed.h"
#include <math.h>
class TH2; 

/** Radial lens correction model.
 *
 *  The idea is that the lens distortion can be modeled as:
 *
 *    \vec{r_{corrected}}  =  F(\mathbf{r_{distorted}}) \vec{r_{distorted}} 
 *
 *    where \vec{r} = ( x - x_{center} , y - y_{center}). 
 *
 *  
 *   Moreover, we assume that F(r) can be modelled by relatively few 
 *   terms of the the polynomial series 
 *
 *   F = c0 + c1 r + c2 r^2 + c3 r^3 + ... 
 *
 *   Reference: http://www.ipol.im/pub/algo/ags_algebraic_lens_distortion_estimation/
 *
 *
 *   Written by Cosmin Deaconu <cozzyd@mit.edu>
 */



namespace dmtpc
{
  namespace image
  {
      class LensCorrection : public TNamed
      {

        public:
          /** Construct a new lens correction object. 
           *
           *  @param name The name of the lens correction
           *  @param order The order of the polynomial used (default 2). 
           *  @param polyn The coefficients of the polynomial, or NULL (default) to set later. 
           *
           */

          LensCorrection(const char * name = "lenscorr", unsigned order = 2, double * polyn = 0); 

          /** Destructor */
          virtual ~LensCorrection();

          /** Set nth paramter to val */
          void setParameter(unsigned n, double val) { if (n <= _order) _coeffs[n] = val; } 

          /** Set all parameters from array */ 
          void setParameters(const double * vals); 

          /** Correct the distortion the input histogram placing the output in the out array. If the output array is null, a new one will be allocated 
           *
           *  @param in The input histogram to correct 
           *  @param out The output histogram, or use NULL to allocate a new one. 
           *  @param interpolation The interpolation method to use. See MaxCamImageTools interpolate method. 
           *  @param camera_center The coordinates of the lens center (2 double array), or NULL to use the center of the image
           *  @param distort true to do distortion instead of correction
           *  @returns the undistorted histogram or NULL if something went wrong. 
           *
           * */
          TH2 * correctDistortion(const TH2 * in, TH2 * out = 0, const char * interpolation = "bicubic", const double * camera_center = 0, bool distort = false) const;

          /** Takes undistorted coordinates to distorted coordinates. 
           * @param center the center of the image (2 double array)
           * @param in The coordinates input (2 double array)
           * @param out The coordinates output (2 double array).
           * @returns 0 if nothing went wrong
           */ 
          int distortCoords(const double * center, const double * in, double * out) const; 

           /** Takes distorted coordinates to undistorted coordinates. 
           * @param center the center of the image (2 double array)
           * @param in The coordinates input (2 double array)
           * @param out The coordinates output (2 double array).
           * @returns 0 if nothing went wrong
           */ 
          int unDistortCoords(const double * center, const double * in, double * out) const; 

        private:

          unsigned _order; 
          double * _coeffs; //[_order+1]; 

        ClassDef(LensCorrection,2); 
      };

  }
}

#endif

