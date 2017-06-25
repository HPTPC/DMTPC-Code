/** \file LogNormalDist.hh
\brief Header for dmtpc::math::dist::LogNormal class
\author Jeremy Lopez
*/
#ifndef DMTPC_LOGNORMALDIST_HH
#define DMTPC_LOGNORMALDIST_HH

#include "Distribution.hh"

namespace dmtpc{
  namespace math{
    
    namespace dist{
      /** \brief Implementation of a log normal distribution.

          The log normal distribution is a distribution where log(x) is normally distributed.
          Its pdf is
          \f[p(x) = \frac{1}{x\sqrt{2\pi \sigma^2}} e^{-(\log x - \mu )^2 / 2\sigma^2} \f]
          and its CDF is 
          \f[{\rm cdf}(x) = \frac{1}{2}\left[1 + {\rm erf}\left(\frac{\log x - \mu}{\sigma\sqrt{2}}\right)\right].\f]
          Its mean, median, and mode are
          \f{eqnarray*}{
            {\rm mean} &=& e^{\mu+\sigma^2/2}\\
            {\rm median} &=& e^\mu \\
            {\rm mode} &=& e^{\mu-\sigma^2}.
          \f}
          Its variance, skewness, and excess kurtosis are
          \f{eqnarray*}{
            {\sigma^2} &=& \left(e^{\sigma^2}-1\right)e^{2\mu+\sigma^2}\\
            {\gamma_1} &=& \left(e^{\sigma^2}+2\right)\sqrt{\exp\left(\sigma^2\right)-1}\\
            {\gamma_2} &=& e^{4\sigma^2}+2e^{3\sigma^2}+3e^{2\sigma^2}-6\\
          \f}

      */
      class LogNormal : public Distribution{
        public:   
          /** Constructor */
          LogNormal();
          /** Constructor 
              \param mean the mean log(x)
              \param sigma the std deviation of log(x)
          */
          LogNormal(double mean, double sigma);
          /** Destructor */
          virtual ~LogNormal(){}
          /** There are two parameters */
          int numberOfPars() const {return 2;}
          /** Get the parameters as an array. The array is [&mu;,&sigma;]
              \param x the output array
          */
          void getParameters(double* x) const{x[0] = fMean; x[2] = fSigma;}
          /** Get the parameter &mu; */
          double getMean() const {return fMean;}
          /** Set the mean of log(x)
              \param m the mean
          */
          void setMean(double m){fMean = m;}
          /** Get the parameter &sigma; */
          double getSigma() const {return fSigma;}
          /** Set the standard deviation of log(x)
             \param s the standard deviation
          */
          void setSigma(double s){fSigma =s;}
          /** Set the parameters using an array. The array is [&mu;,&sigma;]
              \param x the parameter array
          */
          void setParameters(const double* x){fMean = x[0]; fSigma = x[1];}
          /** get the mean */
          double mean() const;
          /** get the median */
          double median() const;
          /** get the mode */
          double mode() const;
          /** get the standard deviation */
          double stddev() const;
          /** get the variance */
          double variance() const;
          /** get the skewness */
          double skewness() const;
          /** get the excess kurtosis */
          double kurtosis() const;
          /** The probability density function
              \param x the position
          */
          double pdf(double x) const;
          /** The cumulative distribution function
              \param x the position
          */
          double cdf(double x) const;
          /** The probability density function
              \param x the position
          */
          double pdf(int x) const{return pdf((double) x);}
          /** The cumulative distribution function
              \param x the position
          */
          double cdf(int x) const {return cdf((double) x);}
          /** The inverse CDF
              \param x the probability
          */
          double inverseCDF(double x) const;
        private:
          double fMean;///< the mean log(x)
          double fSigma;///< the std deviation of log(x)
      };
    }
  }
}

#endif
