#ifndef DMTPC_MATH_CONVOLVED_SEGMENT_HH
#define DMTPC_MATH_CONVOLVED_SEGMENT_HH

namespace dmtpc
{
  namespace math
  {
    /**
     * One dimensional line segment convolved with gaussian distribution 
     * @param x   pointer to position to evaluate
     * @param pars m,b,x0,x1,sigma
     * @return value at x 
     */ 

    const int npar_lineSegmentConvolvedWithGaussian = 5; 
    const int npar_lineSegmentConvolvedWithCauchy = 5; 
    double lineSegmentConvolvedWithGaussian(double *x, double *pars);
    double lineSegmentConvolvedWithCauchy(double *x, double *pars);

    /** two dimensional line segment convolved with gaussian distribution
     *
     * @param x pointer to position to evaluate (x,y) 
     * @param pars m,b,x0,y0,x1,y1,sigma
     *
     */
    const int npar_lineSegmentConvolvedWithGaussian2D = 7; 
    const int npar_lineSegmentConvolvedWithCauchy2D = 7; 
    double lineSegmentConvolvedWithGaussian2D(double *x, double *pars);
    double lineSegmentConvolvedWithCauchy2D(double *x, double *pars);

    /** integral of lineSegmentConvolvedWithGaussian between y0 and y1
     */
    const int npar_integralOfLineSegmentConvolvedWithGaussian = 7; 
    double integralOfLineSegmentConvolvedWithGaussian(double y0, double y1, double *pars);
  }
}


#endif
