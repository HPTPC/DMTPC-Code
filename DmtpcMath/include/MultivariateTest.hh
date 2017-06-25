#ifndef DMTPC_MATH_MULTIVARIATE_TEST_HH
#define DMTPC_MATH_MULTIVARIATE_TEST_HH


#include <vector>

class TTree; 

namespace dmtpc
{
  namespace math
  {

    class MultivariateTest
    {

      public: 

        void setAValues(unsigned n, const double * const * x, const double * const * xerr_dn= 0, const double * const * xerr_up = 0, const double *  weights = 0); 
        void setBValues(unsigned n, const double * const * x, const double * const * xerr_dn= 0, const double * const * xerr_up = 0, const double *  weights = 0); 

        int setAValues(TTree * t, const char * const * xnames, const char * const * xerr_dn_names = 0, const char * const * xerr_up_names = 0, const char * weight_names = 0, const char * cut = "", bool append = false); 
        int setBValues(TTree * t, const char * const * xnames, const char * const * xerr_dn_names = 0, const char * const * xerr_up_names = 0, const char * weight_names = 0, const char * cut = "", bool append = false); 


        virtual double testStatistic() const = 0; 

        void setScale(unsigned dim, double scale = 1) { scales[dim] = scale; }  
        void treatAsAngle(int dim, bool treat = true ) { isAngle[dim ] = treat; } 
        virtual ~MultivariateTest() {;} 

      protected: 
        MultivariateTest(unsigned ndim); 
        std::vector<const double *> vals_A; 
        std::vector<const double *> vals_B; 
        std::vector<double *> tree_store_A; 
        std::vector<double *> tree_store_B; 
        const double * weights_A; 
        const double * weights_B; 
        double sum_weights_A; 
        double sum_weights_B; 
        std::vector<const double *> vals_A_err_up; 
        std::vector<const double *> vals_A_err_dn; 
        std::vector<const double *> vals_B_err_up; 
        std::vector<const double *> vals_B_err_dn; 
        std::vector<double> scales; 
        std::vector<bool> isAngle; 
        unsigned num_A;
        unsigned ndim; 
        unsigned num_B;
        mutable bool Adirty; 
        mutable bool Bdirty; 

      private: 
        int setValues(char which,TTree * t, const char * const * xnames, const char * const * xerr_dn_names = 0, const char * const * xerr_up_names = 0, const char * weight_names = 0, const char * cut = "", bool append = false); 

    };
  
  }
}

#endif

