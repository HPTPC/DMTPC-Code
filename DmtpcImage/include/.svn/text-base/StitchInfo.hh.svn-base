#ifndef DMTPC_STITCH_INFO_HH
#define DMTPC_STITCH_INFO_HH

#include "TNamed.h" 
#include <vector>
#include "TH2F.h"
#include "LensCorrection.hh" 
#include "TH2D.h"



namespace dmtpc
{
  namespace image
  {
    /** This class stores the information necessary to stitch an image with the montage function in dmtpc::image::transforms */ 

    class StitchInfo : public TNamed
    {
      public:

        /** Create a StitchInfo object. 
         *
         *  @param name The name of the object
         *  @param n    The number of images to stitch
         *  @param lens The lens correction object, or null to not use any. 
         *  @param xmins array of minimum x in the images 
         *  @param ymins array of minimum y in the images 
         *  @param xmaxs array of maximum x in the images 
         *  @param ymaxs array of maximum y in the images 
         *  @param nbinsx array of number of x bins in the images
         *  @param nbinsy array of number of y bins in the images
         *  @param xorigin x coordinate of each image origin
         *  @param yorigin y coordinate of each image origin
         *  @param rotation amount of rotation for each image
         *  @param scale amount of scale for each image
         *  @param weigh amount of weight factor for each image
         *  @param binning imagine binning, or 0 to calculate from first bin
         */ 
       StitchInfo (const char * name, unsigned n, const LensCorrection * lens,  const double * xmins, const double * ymins, const double * xmaxs, const double * ymaxs,
                     const int * nbinsx, const int * nbinsy,  const double * xorigin , const double * yorigin , const double * rotation , 
                     const double * scale , const double * weight, unsigned binning = 0); 

       StitchInfo() { weight_sum = 0;}

       virtual ~StitchInfo(); 

       unsigned nimages;
       std::vector<std::vector<std::vector<double> > > corners;
       std::vector<double> xmins;
       std::vector<double> xmaxs;
       std::vector<double> ymins;
       std::vector<double> ymaxs;
       std::vector<int> nbinsx;
       std::vector<int> nbinsy;
       std::vector<double> xorigin;
       std::vector<double> yorigin;
       std::vector<double> rotation;
       std::vector<double> sintheta;
       std::vector<double> costheta;
       std::vector<double> scale; 
       std::vector<double> weight; 
       std::vector<double>xwidths;
       std::vector<double>ywidths;
       std::vector<TH2F*> frac;
       double stitched_xmax; 
       double stitched_ymax; 
       double stitched_xmin; 
       double stitched_ymin; 
       unsigned stitched_nbinsx; 
       unsigned stitched_nbinsy; 
       TH2F * weight_sum; 

       ClassDef(StitchInfo,4); 
    };
  }
}






#endif
