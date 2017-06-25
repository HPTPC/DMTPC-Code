/** \file PoissonDist.hh
\brief Header for dmtpc::math::dist::Poisson class
\author Jeremy Lopez
*/
#ifndef DMTPC_POISSONDIST_HH
#define DMTPC_POISSONDIST_HH

#include "Distribution.hh"

namespace dmtpc{
  namespace math{
    namespace dist{

      /** \brief Implementation of a Poisson distribution
        
          The Poisson distribution is a discrete distribution defined by the
          probability mass function (called pdf here)
          \f[p(N) = e^{-\mu}\frac{\mu^N}{N!}. \f]
          Its cumulative function is calculated here using
          \f[{\rm cdf}(N) = \sum\limits_{n=0}^{N} p(n) \f]
          but can also be expressed in terms of the incomplete gamma function,
          \f[{\rm cdf}(N) = \frac{\Gamma(N+1,\mu)}{N!}.\f]

          The mean, median and mode are given by
          \f{eqnarray*}{ 
            {\rm mean} &=& \mu\\
            {\rm median} &\approx& {\rm floor}\left(\mu +\frac{1}{3} - \frac{0.02}{\mu}\right)\\
            {\rm mode} &=& {\rm floor}(\mu).
          \f}
          The variance, skewness, and excess kurtosis are
          \f{eqnarray*}{
            \sigma^2 &=& \mu\\
            \gamma_1 &=& \frac{1}{\sqrt{\mu}}\\
            \gamma_2 &=& \frac{1}{\mu}.
          \f} 
      */
      class Poisson : public Distribution{
        public:  
          /** Constructor */ 
          Poisson();
          /** Constructor
              \param mean the distriubtion mean
          */
          Poisson(double mean);
          /** Destructor */
          virtual ~Poisson(){}
          /* There is one parameter */
          int numberOfPars() const {return 1;}
          /** Get the parameters as an array. The array is [&mu;]
              \param x the output array
          */
          void getParameters(double* x) const {x[0] = fMean;}
          /** Get the mean */
          double getMean() const {return fMean;}
          /** Set the distribution mean
              \param m the new mean
          */
          void setMean(double m){fMean = m;}
          /** Set the parameters using an array. The array is [&mu;]
              \param x the new mean
          */
          void setParameters(const double* x){fMean = x[0];}
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
          /** Get the pdf
              \param x the position
          */
          double pdf(double x) const {return pdf( (int) x);}
          /** Get the cdf. Uses the slow method of directly calculating the sum.
              \param x the position
          */
          double cdf(double x) const {return cdf( (int) x);}
          /** Get the pdf
              \param x the position
          */
          double pdf(int x) const;
          /** Get the cdf. Uses the slow method of directly calculating the sum.
              \param x the position
          */
          double cdf(int x) const;
          /** Get the inverse cdf. Not available for this distribution.
              \param x the probability
           */
          double inverseCDF(double x) const;
        private:
          double fMean;
      };
    }
  }
}

#endif
