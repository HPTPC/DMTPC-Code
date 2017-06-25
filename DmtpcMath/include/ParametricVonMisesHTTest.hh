#ifndef DMTPC_MATH_PARAMETRIC_VON_MISES_HT_TEST_HH
#define DMTPC_MATH_PARAMETRIC_VON_MISES_HT_TEST_HH

#include "Minuit2/Minuit2Minimizer.h" 
#include "TRandom3.h"
#include <vector>
#include "Math/Functor.h"
class TF1; 

namespace dmtpc
{

  namespace math
  {

    class ParametricVonMisesHTTest
    {

      public: 

        struct Entry
        {
          double phi; 
          double phi_ht; 
          const double *X; 
          const double *covar; 
        }; 

 
        ParametricVonMisesHTTest(unsigned ndim, const TF1 * width_fn, const TF1 * ht_fn, const TF1 * normalization_fn, const TF1 * mean_fn = 0); 
        virtual ~ParametricVonMisesHTTest(); 

        double train(unsigned n, const Entry * train_entries ); 

        
        double f(double phi, const double * X, double phi_ht = 1,  const double * params = 0) const;


        
        double loglikelihood(int n,  const Entry  * entries, const double * pars = 0, double * fs = 0, double * d2fs = 0) const; 
        double likelihood(int n,  const Entry * entries, const double * pars, double * fs = 0, double * d2fs = 0) const; 
        

        /* sets parameters for functions, order is width, ht, normalization, mean */ 
        void setPars(const double * pars, const double * par_errors = 0); 
        void setWidthPars(const double * pars, const double * par_errors = 0); 
        void setHTPars(const double * pars, const double * par_errors = 0); 
        void setNormalizationPars(const double * pars, const double * par_errors = 0); 
        void setMeanPars(const double * pars, const double * par_errors = 0); 
        void setEpsilons(const double *eps); 
        const double * getPars() const { return params;} 
        const double * getParErrs() const  { return param_errs;} 
        const double * getWidthPars() const; 
        const double * getWidthParErrs() const; 
        const double * getHTPars() const; 
        const double * getHTParErrs() const; 
        const double * getNormalizationPars() const; 
        const double * getNormalizationParErrs() const; 
        const double * getMeanPars() const; 
        const double * getMeanParErrs() const; 
        void setMaxCalls(unsigned n) { min->SetMaxFunctionCalls(n); }
        void setMaxIter(unsigned n) { min->SetMaxIterations(n); }
        void setNTrials(unsigned n) { ntrials = n; } 
        void setMaxAttempts(unsigned n) { max_attempts = n; } 
        void setSeed(long seed) { rand.SetSeed(seed); } 


        void setXUpperLimit(unsigned dim, double val) { X_upper_limits[dim] = val; setXLowerLimitOn(dim,true); }
        void setXLowerLimit(unsigned dim, double val) { X_lower_limits[dim] = val; setXUpperLimitOn(dim,true); }  
        void setXUpperLimitOn(unsigned dim, bool on) { X_upper_limits_enable[dim] = on; }
        void setXLowerLimitOn(unsigned dim, bool on) { X_lower_limits_enable[dim] = on; }
 

      private: 
        const TF1 * width_fn; 
        const TF1 * ht_fn; 
        const TF1 * normalization_fn; 
        const TF1 * mean_fn; 
        ROOT::Math::Functor *minfn; 
        double minFn(const double *p) const; 

        unsigned ntrials; 
        unsigned max_attempts; 
        unsigned ndim; 
        int nparams; 
        double * params; 
        double * param_errs; 
        double * epsilons; 
        const char ** param_names; 

        int ntrain; 
        const Entry * train_data; 
        mutable TRandom3 rand; 
        ROOT::Minuit2::Minuit2Minimizer *min; 

        std::vector<double> X_upper_limits; 
        std::vector<double> X_lower_limits; 
        std::vector<bool> X_upper_limits_enable; 
        std::vector<bool> X_lower_limits_enable; 


    }; 
  }
}



#endif
