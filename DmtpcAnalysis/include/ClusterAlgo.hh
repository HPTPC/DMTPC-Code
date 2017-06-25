#ifndef DMTPC_ANALYSIS_CLUSTER_ALGO_HH
#define DMTPC_ANALYSIS_CLUSTER_ALGO_HH

#include <list>
#include "TGraph.h"


class TH1; 
class TH2; 

namespace dmtpc
{
  namespace analysis
  {
    class Cluster; 
    class GainMap; 

    namespace clusteralgo
    {


      /** Finds the integral of the cluster, optionally with a gain map */ 
      double getIntegral(const Cluster * c, const GainMap * g = 0, bool reduced = false); 

      /* Get maximum separation within the cluster , also returning start and end points*/ 
      double getLength(const Cluster * c, double *x0 = 0, double *x1 = 0, double *y0 = 0, double *y1 = 0, bool reduced = true); 

      /* formerly getLength2 */ 
      double getLengthAlongAngle(const Cluster * c, double phi, bool reduced = true); 


      /** Finds phi from getLength, equivalent to old getPhi() */ 
      double getPhiFromLength(const Cluster * c, bool reduced = true);  

      /**
         finds a vector angle based on PCAand getAsymmetry. Formerly getPhi2
         \return phi
      */
      double getPhiPCA(const Cluster * c) ;


      /** finds an axial  angle based on the angle with maximum sigma. Formerly getPhi3. 
       \return phi
      */
     double getPhiMaxSigma(const Cluster * c ) ;

      /** finds an axial angle by fitting a line to the cluster. Formerly getPhi4 
        \return phi
      */
     double getPhiLineFit(const Cluster *c) ;

 

      /* Find centroid , returns the integral (which must be calculated anyway) */ 
      double getXY(const Cluster * c, double *x = 0, double *y=0, const GainMap * g = 0); 


      /* returns true if bin is in cluster */ 
      bool inCluster(const Cluster *c, int bin, bool reduced = false) ;


      /** calulates the major and minor axes of the best fit ellipse
      \param a destination for major axis
      \param b destination for minor axis
      */
      void getEllipseAxes(const Cluster* c, double* a, double* b);

      /** calculates the energy density
      */

      double getMean(const Cluster *c); 
      double getRMS(const Cluster *c); 
      double getMax(const Cluster *c, int *maxpix = 0); 


      /** calculates third moment along an angle 
      \param phi angle to calculate along
      \return skewness
      */
      double getSkewness(const Cluster* c, double phi);

       /** calculates two bin "skewness" to determine direction of track
        \param phi angle to calculate along
       */
      double getAsymmetry(const Cluster *c, double phi) ;
 
      void morphologicalOperation(Cluster * c, int nerode , int ndilate, bool red  = false); 

      int getNeighborsAboveThresh(const Cluster * c, double thresh, int bin); 

      /**
      finds the extremes of the cluster, plus a border
      \param i ith cluster
      \param xmin return variable for the lower x edge of the range
      \param xmax return variable for the upper x edge of the range
      \param ymin return variable for the lower y edge of the range
      \param ymax return variable for the upper y edge of the range
      \param borderpx size of the border for the region
      */


      void clusterBinBounds(const Cluster *c, int * xmin, int *xmax, int *ymin, int * ymax); 
      void clusterBounds(const Cluster *c, double * xmin, double *xmax, double *ymin, double * ymax, double border_px); 

      int makeBoundary(const Cluster *c, std::list<TGraph> * lines, bool red = false, bool draw = false, const char * drawopt="lsame", int linecolor = 0, int linewidth = 1); 


      bool edge(const Cluster *c); 

      //min and max distance from cluster to point
      double minDistance(const Cluster * c, double x, double y); 
      double maxDistance(const Cluster * c, double x, double y); 

      /* minimum aned maximum distance along cluster (i.e.  x*cos(phi) + y*sin(phi) ) */
      void minmax(const Cluster *c, double phi, double * minval = 0, double * maxval = 0, bool red = false); 

      /* Project with no interpolation. Binning either pixels per bin or total number of bins  */
      TH1 * projectLegacy(const Cluster *c, double phi, int binning = 4, bool binningIsPixPerBin = true, bool red = false); 

      /* Project with interpolation */ 
      TH1 * project(const Cluster *c, double phi, const char * interpolation_method = "bilinear", GainMap * map = 0,
                           double min_gain = 0, bool reset_xaxis = true, const char * name = 0, double * startpos = 0, double meanForErrorCalc =  0, double noiseForErrorCalc = 10);  
      
      TH2 * clusterHist(const Cluster *c, int padding = 2, bool red = false, GainMap * map = 0, double mingain = 0, bool setzero = true); 


      /* compute moment (use binning = 0 for unbinned */ 
      double moment(const Cluster *c, int nth, double phi, int binning, bool binningIsPixPerBin = true, bool red = false);

      /* returns -1 if no spacers */ 
      double nearestSpacerDistance(const Cluster *c, const GainMap * m, int & nearest_spacer) ; 

      double maxDerivativeX(const Cluster * c); 
      bool fullyConnected( const Cluster * c); 
    }
  }
}

#endif

