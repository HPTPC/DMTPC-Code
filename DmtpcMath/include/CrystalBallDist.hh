/** \file CrystalBallDist.hh
\brief Header for dmtpc::math::dist::CrystalBall class
\author Jeremy Lopez
*/
#ifndef DMTPC_CRYSTALBALLDIST_HH
#define DMTPC_CRYSTALBALLDIST_HH
#include "Distribution.hh"

namespace dmtpc{
  namespace math{
    namespace dist{
      /** \brief Implementation of the Crystal Ball distribution

          The Crystal Ball function is a Gaussian distribution with
          an extended polynomial tail at low x. 
          Its pdf is
          \f[ p(x) = N\cdot \left\{\begin{array}{lr}\exp\left(-\frac{(x-\mu)^2}{2\sigma^2}\right) & x-\mu > -\alpha\sigma \\ A\left(B-\frac{x-\mu}{\sigma}\right)^{-n}& x-\mu \le -\alpha\sigma \end{array} \right.\f]
          where
          \f{eqnarray*}{ 
            A &=& \left(\frac{n}{|\alpha|}\right)^n\exp\left(-\frac{\alpha^2}{2}\right)\\
            B &=& \frac{n}{|\alpha|}-|\alpha|\\
            N &=& \frac{1}{\sigma(C+D)} \\
            C &=& \frac{n}{(n-1)|\alpha|}\exp\left(-\frac{\alpha^2}{2}\right)\\
            D &=& \sqrt{\frac{\pi}{2}}\left(1+{\rm erf}\left(\frac{|\alpha|}{\sqrt{2}}\right)\right).
          \f}

          Note that statistics and cdf information is not currently available
          for this distribution. 

       */
      class CrystalBall : public Distribution{
        public:   
          /** Constructor */
          CrystalBall();
          /** Constructor
              \param mean the mean of the Gaussian part
              \param sigma the std deviation of the Gaussian part 
              \param alpha the cutoff position of the Gaussian in sigma
              \param n the order of the polynomial part
          */
          CrystalBall(double mean, double sigma,double alpha, double n);
          /** Destructor */
          virtual ~CrystalBall(){}
          /** There are 4 parameters */
          int numberOfPars() const {return 4;}
          /** Get the parameters as an array. The array is [&mu;,&sigma;,&alpha;,n]
              \param x the output array
          */
          void getParameters(double* x) const
            {x[0] = fMean; x[1] = fSigma; x[2] = fAlpha; x[3] = fN;}
          /** Get the parameter &mu; */
          double getMean() const {return fMean;}
          /** Set the mean of the Gaussian part
              \param m the mean
          */
          void setMean(double m){fMean = m;}
          /** Get the parameter &sigma; */
          double getSigma() const {return fSigma;}
          /** Set the standard deviation of the Gaussian part
              \param s the standard deviation
          */
          void setSigma(double s){fSigma = s;}
          /** Get the parameter &alpha; */
          double getAlpha() const {return fAlpha;}
          /** Set the boundary between Gaussian and polynomial parts, 
              in standard deviations from the mean.
              \param a alpha
          */
          void setAlpha(double a){fAlpha = a;}
          /** Get the parameter n */
          double getN() const {return fN;}
          /** Set the order of the polynomial part 
              \param n the order of the polynomial
          */
          void setN(double n){fN = n;}
          /** Set the parameters from an array. The array is [&mu;,&sigma;,&alpha;,n]
             \param x the array 
          */
          void setParameters(const double* x)
            {fMean = x[0]; fSigma = x[1]; fAlpha = x[2]; fN = x[3];}
          /** Get the mean */
          double mean() const;
          /** Get the median */
          double median() const;
          /** Get the mode */
          double mode() const;
          /** Get the standard deviation */
          double stddev() const;
          /** Get the variance */
          double variance() const;
          /** Get the skewness */
          double skewness() const;
          /** Get the excess kurtosis */
          double kurtosis() const;
          /** Get the probability density function
              \param x the position
          */
          double pdf(double x) const;
          /** Get the cumulative distribution function
               \param x the  position
          */
          double cdf(double x) const;
          /** Get the probability density function
              \param x the position
          */
          double pdf(int x) const {return pdf( (double) x);}
          /** Get the cumulative distribution function
               \param x the  position
          */
          double cdf(int x) const {return cdf( (double) x);}
          /** Get the inverse cumulative distribution function
               \param x the probability
          */
          double inverseCDF(double x) const;
        private:
          double fMean;///< the mean of the Gaussian
          double fSigma;///< the standard deviation of the Gaussian
          double fAlpha;///< the boundary between the Gaussian and polynomial parts
          double fN;///< the order of the polynomial part
      };
    }
  }
}

#endif
