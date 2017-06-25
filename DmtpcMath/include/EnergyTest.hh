#ifndef DMTPC_MATH_ENERGY_TEST_HH
#define DMTPC_MATH_ENERGY_TEST_HH

#include "MultivariateTest.hh"

#include <vector>

/* Computes a modified energy test between two multivariate distributions
 *
 *
 **/ 

namespace dmtpc
{
  namespace math

  {
    class EnergyTest : public MultivariateTest
    {

      public:
        enum DistanceMetric
        {

          SIMPLE,  // x - y 
          ERRORS_IN_QUADTRATURE, // (x-y) / sqrt(sx^2 + sy^2) 
          AVERAGE_ERROR_DISTANCE, // (x-y) / (2sx) + (x-y) / (2sy) 

        };   


        EnergyTest(unsigned ndim, double alpha = 1, DistanceMetric metric = AVERAGE_ERROR_DISTANCE); 




        
        double testStatistic() const; 
        double getAA() const { return last_AA; } 
        double getBB() const { return last_BB; } 
        double getAB() const { return last_AB; } 
        double getBA() const { return last_BA; } 



//        double nullWidth(bool AisNull = true, int ntrials = 1000); 

      private:

        double dist(char,char) const; 
        double alpha; 
        mutable double last_AA; 
        mutable double last_AB; 
        mutable double last_BA; 
        mutable double last_BB; 
        DistanceMetric metric; 
    }; 
  }
}




#endif
