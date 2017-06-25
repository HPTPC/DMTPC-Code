#ifndef DMTPC_MATH_ANGULAR_HH
#define DMTPC_MATH_ANGULAR_HH
#include <cstdlib>

class TH1; 


namespace dmtpc
{
  namespace math
  {
    namespace angle
    {

      /** Normalize angle to (center - pi, center + pi] 
       *  \param phi input angle
       *  \param center center angle
       **/
      double normalize(double phi, double center = 0); 
      double normalize_axial(double phi, double center = 0); 

      /** Compute difference in angle between phi0 and phi1 */ 
      double delta(double phi0, double phi1); 
      double delta_axial(double phi0, double phi1); 


      /** Compute the average of given angles **/ 
      double avg(size_t nangles, const double * angles, const double * weights = 0); 
      double avg(const TH1* hist); 

      double avg_axial(size_t nangles, const double * angles, const double * weights = 0); 
      double avg_axial(const TH1* hist); 
    
      /** Compute the resultant of given angles with the given weights **/ 
      double resultant(size_t nangles, const double * angles, const double * weights = 0); 
      double resultant(const TH1* hist); 

      double resultant_axial(size_t nangles, const double * angles, const double * weights = 0); 
      double resultant_axial(const TH1* hist); 


      /** Compute the rayleigh statistic of given angles with the given weights **/ 
      double rayleigh(double resultant, size_t n);
      double var(double resultant, size_t n); 

      /** Finds k for x = I1(k)/I0(k) */ 
      double a1inv(double x); 
      

      double von_mises_kappa_estimate(double resultant, size_t n); 
      /** Von mises distribution, 0 = scale, 1 = mean, 2 = dispersion */
      double von_mises_dist(double *x, double *par); 
      /** Von mises distribution w/ head tail fraction, 0 = HT, 1 = scale, 2 = mean, 3 = dispersion  */
      double von_mises_dist_ht(double *x, double *par); 


      /** Wrapped normal distribution, 0 = scale, 1 = mean, 2 = sigma **/ 
      double wrapped_normal_dist(double *x, double *par) ;
      /** Wrapped normal distribution w/ head tail fraction 0 = HT, 1 = scale, 2 = mean, 3 = sigma  **/ 
      double wrapped_normal_dist_ht(double *x, double *par) ;


      /** Ratio distribution, comes up when angle is between two gaussianly distributed vectors
       * pars: 0: normalization, 1 v1 mean, 2 v1 sigma, 3 v2 mean, 4 v2 sigma  
       *
       * */ 
      double ratio_distribution(double *x, double * par); 

      /* Implementation of Kuiper's distribution free two sample test.
       * kuipers_test returns test statistic, while kuipers_p converts test
       * statistic to p value. The histogrammed version will give slightly
       * higher p values than warranted,  worse with coarser binning. */ 

      double kuipers_test(unsigned m, const double * A, unsigned n, const double * B, bool isSorted = false); 
      double kuipers_test(const TH1 * A, const TH1 * B);
      double kuipers_p(double V, int m, int n = 0); 

      /* TODO. Complicated part is getting p value...
      double watson_test(unsigned m, const double * A, unsigned n, const double * B, bool isSorted = false); 
      double watson_test(const TH1 * A, const TH1 * B);
      double watson_p(double V, int m, int n = 0); 
      */

    }
  }
}



#endif
