#ifndef DMTPC_INTERPOLATION_HH
#define DMTPC_INTERPOLATION_HH

class TH2; 

namespace dmtpc
{
  namespace image
  {
    namespace interpolation
    {

      /** Interpolates a value in the given histogram with the given method. 
        *
        * Current methods are :
        *   bilinear
        *   bicubic
        *   nearest 
        *
        * Use testInterpolate to check a method. 
        *
        */ 
       double interpolate(const TH2* in, double x, double y, const char * method = "bilinear"); 

       /** Checks if method is a valid interpolation method */
       bool testInterpolate(const char * method); 
    }
  }
}


#endif
