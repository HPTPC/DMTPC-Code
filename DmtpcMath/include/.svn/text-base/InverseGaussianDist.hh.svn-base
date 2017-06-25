/** \file InverseGaussianDist.hh
\brief Header for dmtpc::math::dist::InverseGaussian class
\author Jeremy Lopez
*/
#ifndef DMTPC_INVERSEGAUSSIANDIST_HH
#define DMTPC_INVERSEGAUSSIANDIST_HH

#include "Distribution.hh"

namespace dmtpc{
  namespace math{
    namespace dist{
      /** \brief Implementation of an inverse Gaussian distribution

          The inverse Gaussian distribution is the time distribution of
          particles reaching a given point after undergoing drift and Brownian motion.
          Its pdf is
          \f[ p(x) = \left[\frac{\lambda}{2\pi x^3}\right]^{1/2}\exp\left(-\frac{\lambda(x-\mu)^2}{2\mu^2x}\right).\f]
          Its CDF is
          \f[\Phi\left(\sqrt{\frac{\lambda}{x}}\left[\frac{x}{\mu}-1\right]\right)
             +\exp\left(\frac{2\lambda}{\mu}\right)\Phi\left(-\sqrt{\frac{\lambda}{x}}\left[\frac{x}{\mu}+1\right]\right)
          \f]
          where \f$\Phi(x)\f$ is the CDF of the Gaussian distribution.

          The distribution mean and mode are
          \f{eqnarray*}{
            {\rm mean} &=& \mu\\
            {\rm mode} &=& \mu\left[\left(1+\frac{9\mu^2}{4\lambda^2}\right)^{1/2}-\frac{3\mu}{2\lambda}\right].
          \f}
          The variance, skewness, and excess kurtosis are
          \f{eqnarray*}{
            \sigma^2 &=& \frac{\mu^3}{\lambda}\\
            \gamma_1 &=& 3\sqrt{\frac{\mu}{\lambda}} \\
            \gamma_2 &=& \frac{15\mu}{\lambda}.
          \f}
      */
      class InverseGaussian : public Distribution {
        public:   
          /** Constructor */
          InverseGaussian();
          /** Constructor
              \param mean the distribution mean
              \param lambda the distribution width
          */
          InverseGaussian(double mean, double lambda);
          /** Destructor */
          virtual ~InverseGaussian(){}
          /** There are two parameters */
          int numberOfPars() const {return 2;}
          /** Get the parameters as an array. The array is [&mu;,&lambda;].
              \param x the output array
          */
          void getParameters(double* x) const {x[0] = fMean; x[1] = fLambda;}
          /** Get the mean */
          double getMean() const {return fMean;} 
          /** Get the width */
          double getLambda() const {return fLambda;}
          /** Set the mean
              \param m the mean
          */
          void setMean(double m){fMean = m;}
          /** Set the distribution width
              \param l the width
          */
          void setLambda(double l){fLambda = l;}
          /** Set the parameters using an array. The array should be [&mu;,&lambda;].
              \param x the array
          */  
          void setParameters(const double* x){fMean = x[0]; fLambda = x[1];}
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
          /** Evaluate the pdf at a position
              \param x the position
          */
          double pdf(double x) const;
          /** Evaluate the cumulative distribution function at a position
              \param x the position
          */
          double cdf(double x) const;
          /** Evaluate the pdf at a position
              \param x the position
          */
          double pdf(int x) const {return pdf( (double) x);}
          /** Evaluate the cumulative distribution function at a position
              \param x the position
          */
          double cdf(int x) const {return cdf( (double) x);}
          /** Evaluate the inverse CDF
              \param x the probability
          */
          double inverseCDF(double x) const;
        private:
          double fMean;///< the mean
          double fLambda;///< the width
      };
    }
  }
}

#endif
