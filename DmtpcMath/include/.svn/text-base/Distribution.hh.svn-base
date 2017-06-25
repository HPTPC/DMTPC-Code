/** \file Distribution.hh
\brief Header for dmtpc::math::Distribution class
\author Jeremy Lopez
*/
#ifndef DMTPC_DISTRIBUTION_HH
#define DMTPC_DISTRIBUTION_HH

namespace dmtpc{
  namespace math{

    /**\brief Abstract base class for a probability distribution.

       This class contains information about a one-dimensional probability distribution.
       Derived classes must provide a probability density function (pdf).
       If a random number is distributed according to a pdf, the probability of it being between points x<sub>1</sub> and x<sub>2</sub> is
       \f[P(x_1<x<x_2) = \int\limits_{x_1}^{x_2} {\rm pdf}(x)dx. \f]
       
       If available, the cumulative distribution function gives the total probability up to a given point,
       \f[ {\rm cdf}(x) = P(x) = \int\limits_{-\infty}^x {\rm pdf}(y)dy. \f]
       The inverse cdf gives the position as a function of probability. The inverse cdf is
       particularly useful in generating random numbers based on the distribution.

       In addition to the pdf, cdf, and inverse cdf, various statistical properties may be
       included. Flags for each of these variables are set so a user can test if they are
       available. The mean, median, and mode characterize a typical value that may be produced
       by this pdf. They are defined as
       \f{eqnarray*}{ 
       {\rm mean}(x) &=& \mu = \int\limits_{-\infty}^{\infty} {\rm pdf}(x)xdx\\
       {\rm median}(x) &=& {\rm cdf}^{-1}(0.5)\\
       {\rm mode}(x) &=& \left\{x\left| {\rm pdf}(x) = {\rm sup}\ {\rm pdf}(x)\right.\right\}
      \f}

       We can also define the expectation value of a function to be
       \f[E\left[f(x)\right] = \left\langle f(x)\right\rangle \int\limits_{-\infty}^\infty f(x){\rm pdf}(x)dx. \f]
       Using this definition, we define the variance to be
       \f[\sigma^2 = \left\langle(x - \mu)^2\right\rangle
                   = \left\langle x^2\right\rangle - \mu^2\f]
       where &sigma; is the standard deviation, the skewness to be
       \f[ \gamma_1 = \left\langle\left(\frac{x-\mu}{\sigma}\right)^3\right\rangle 
                    = \frac{\left\langle x^3\right\rangle - 3\mu\sigma^2 - \mu^3}{\sigma^3}\f]
       and the excess kurtosis to be
       \f[\gamma_2 =\left\langle\left(\frac{x-\mu}{\sigma}\right)^4 - 3\right\rangle.\f]
    */  
    class Distribution{
      public:   
        /** Constructor */
        Distribution(){}
        /** Destructor */
        virtual ~Distribution(){}
        /** See if distribution is discrete */
        bool isDiscrete() const {return fIsDiscrete;}
        /** See if the distribution mean is available */
        bool hasMean() const {return fHasMean;}
        /** See if the distribution median is available */
        bool hasMedian() const {return fHasMedian;}
        /** See if the distribution mode is available */
        bool hasMode() const {return fHasMode;}
        /** See if the distribution variance is available */
        bool hasVariance() const {return fHasVar;}
        /** See if the distribution skewness is available */
        bool hasSkewness() const {return fHasSkew;}
        /** See if the distribution excess kurtosis is available */
        bool hasKurtosis() const {return fHasKurt;}
        /** See if the cumulative distribution function is available */
        bool hasCDF() const {return fHasCDF;}
        /** See if the inverse CDF is available */
        bool hasInverseCDF() const {return fHasInvCDF;}
        /** Get the number of parameters */
        virtual int numberOfPars() const = 0;
        /** Get the parameters as an array 
            \param x the array into which the parameters should be copied
        */
        virtual void getParameters(double* x) const = 0;
        /** Set the parameters 
            \param x the input array of parameters
        */
        virtual void setParameters(const double* x) = 0;
        /** Get the mean */
        virtual double mean() const = 0;
        /** Get the median */
        virtual double median() const = 0;
        /** Get the mode */
        virtual double mode() const = 0;
        /** Get the standard deviation */
        virtual double stddev() const = 0;
        /** Get the variance */
        virtual double variance() const = 0;
        /** Get the skewness */
        virtual double skewness() const = 0;
        /** Get the excess kurtosis */
        virtual double kurtosis() const = 0;
        /** Evaluate the probability density function
            \param x the position
        */
        virtual double pdf(double x) const = 0;
        /** Evaluate the cumulative distribution function
            \param x the position
        */
        virtual double cdf(double x) const = 0;
        /** Evaluate the probability density function
            \param x the position
        */
        virtual double pdf(int x) const {return pdf(x);}
        /** Evaluate the cumulative distribution function
            \param x the position
        */
        virtual double cdf(int x) const {return cdf(x);}
        /** Evaluate the inverse cumulative distribution function
            \param x the probability, 0 to 1
        */
        virtual double inverseCDF(double x) const = 0;
        /** Evaluate the probability density function
            \param x the position
        */
        virtual double operator()(double x) const {return pdf(x);}
        /** Evaluate the probability density function
            \param x the position
        */
        virtual double operator()(int x) const {return pdf(x);}
      
      protected:
        bool fIsDiscrete;///< true if discrete
        bool fHasMean;///< mean is available
        bool fHasMedian;///< median is available
        bool fHasMode;///< mode is available
        bool fHasVar;///< variance is available
        bool fHasSkew;///< skewness is available
        bool fHasKurt;///< kurtosis is available
        bool fHasCDF;///< CDF is available
        bool fHasInvCDF;///< inverse CDF is available
    };

  }
}

#endif
