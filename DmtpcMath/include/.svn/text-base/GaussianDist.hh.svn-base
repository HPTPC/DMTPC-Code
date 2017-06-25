/** \file GaussianDist.hh
\brief Header for dmtpc::math::dist::Gaussian class
\author Jeremy Lopez
*/
#ifndef DMTPC_GAUSSIANDIST_HH
#define DMTPC_GAUSSIANDIST_HH

#include "Distribution.hh"

namespace dmtpc{
  namespace math{
    namespace dist{
/** \brief Implementation of a Gaussian (normal) distribution.

The Gaussian distribution is defined by the pdf
\f[ p(x) = \frac{1}{\sqrt{2\pi \sigma^2}} e^{-(x-\mu)^2/2\sigma^2}.\f]
It has two parameters, &mu; and &sigma;.

The mean, median, and mode are all &mu;.

The variance is &sigma;<sup>2</sup> and standard deviation is &sigma;.

The Gaussian distribution has no skewness or excess kurtosis.

The cumulative distribution function is
\f[ P(x) = \frac{1}{2}\left[1 + {\rm erf\left(\frac{x-\mu}{\sigma\sqrt{2}}\right)}\right] \f]
and the inverse cumulative distribution function is
\f[x(P) = \mu+\sigma\sqrt{2}{\rm erf}^{-1}\left(2P-1\right) \f]
*/
       class Gaussian : public Distribution{
        public:   
          /** Constructor */
          Gaussian();
          /** Constructor
              \param mean the distribution mean
              \param sigma the standard deviation
          */
          Gaussian(double mean, double sigma);
          /** Destructor */
          virtual ~Gaussian(){}
          /** Number of parameters is 2*/
          int numberOfPars() const {return 2;}
          /** Get the parameters. The array is [&mu;,&sigma;]
              \param x the output array
          */
          void getParameters(double* x) const {x[0] = fMean; x[1] = fSigma;}
          /** Get the mean */
          double getMean() const {return fMean;}
          /** Set the mean value 
              \param m the mean
          */
          void setMean(double m){fMean = m;}
          /** Get sigma */
          double getSigma() const {return fSigma;}
          /** Set the standard deviation
              \param s the standard deviation
          */
          void setSigma(double s){fSigma = s;}
          /** Set the parameters using an array. The array is [&mu;,&sigma;]
              \param x the parameter array
          */
          void setParameters(const double* x){fMean = x[0]; fSigma = x[1];}
          /** The mean */
          double mean() const {return fMean;}
          /** The median */
          double median() const {return fMean;}
          /** The mode */
          double mode() const {return fMean;}
          /** The standard deviation */
          double stddev() const {return fSigma;}
          /** The variance */
          double variance() const {return fSigma*fSigma;}
          /** The skewness */
          double skewness() const {return 0;}
          /** The excess kurtosis */
          double kurtosis() const {return 0;}
          /** The pdf at a given position
              \param x the position
          */
          double pdf(double x) const;
          /** The cdf at a given position
              \param x the position
          */
          double cdf(double x) const;
          /** The pdf at a given position
              \param x the position
          */
          double pdf(int x) const {return pdf( (double) x);}
          /** The cdf at a given position
              \param x the position
          */
          double cdf(int x) const {return cdf( (double) x);}
          /** The inverse cdf at a given position
              \param x the position
          */
          double inverseCDF(double x) const;
        private:
          double fMean;///< the mean
          double fSigma;///< the standard deviation
      };
    }
  }
}

#endif
