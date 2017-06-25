#ifndef DMTPC_MC_MCTPC_SCALAR_DISTRIBUTION_HH
#define DMTPC_MC_MCTPC_SCALAR_DISTRIBUTION_HH

#include "TRandom3.h" 
#include "TF1.h" 
#include "TH1.h" 

namespace dmtpc
{
  namespace mc
  {
    namespace mctpc
    {

      class ScalarDistribution
      {
       
        public: 
          virtual double value() = 0; 
          virtual unsigned getNParameters() const { return 0; }
          virtual void setParameters(const double * parameters) {;}
          virtual void setParameter(int i, double parameter) {;}
          virtual ~ScalarDistribution() {;}
      }; 


      class UniformScalarDistribution : public ScalarDistribution
      {

        public: 
        UniformScalarDistribution(double min, double max, long seed = 0) 
          : min(min), max(max), rand(seed) { ; } 

        virtual double value() { return rand.Uniform(min,max); } 
        virtual unsigned getNParameters() const { return 2; } 
        virtual void setParameters(const double * p) { min = p[0]; max = p[1]; }
        virtual void setParameter(int i, double p ) { i == 0 ? min = p :  max = p; } 

        private: 
          double min, max; 
          TRandom3 rand;
      }; 

      class GaussianScalarDistribution : public ScalarDistribution
      {
        public:

          GaussianScalarDistribution(double mean, double sigma, long seed)
            : mean(mean), sigma(sigma), rand(seed) {;}

          virtual double value() { return rand.Gaus(mean,sigma); }
          virtual unsigned getNParameters() const {return 2; }
          virtual void setParameters(const double * p) { mean = p[0]; sigma = p[1]; }
          virtual void setParameter(int i, double p) { i == 0 ? mean = p : sigma = p; }
        private:
          double mean,sigma; 
          TRandom3 rand;
      };

      class ConstantScalarDistribution : public ScalarDistribution
      {

        public:
          ConstantScalarDistribution(double val) : val(val) { ; } 
          virtual unsigned getNParameters() const { return 1; } 
          virtual void setParameters(const double * p) { val = p[0]; } 
          virtual void setParameter(int i, double  p) { val = p; } 
          virtual double value() { return val; }

        private:
          double val; 

      }; 
      


      class AmBeEnergyDistribution : public ScalarDistribution
      {
        public: 
          /* Create ambe energy distribution, with optional reduction distribution to reduce the generated energy by that much  (in keV!).
           *
           * If EParameterNumber >= 0, then that parameter of the reduction distribution is set to E */ 

          AmBeEnergyDistribution(ScalarDistribution * reduction = 0, int EParameterNumber = -1); 
          virtual double value(); 

        private:
          TH1* iso_ambe_spectrum; 
          ScalarDistribution * reduction; 
          int redpar; 
          
      };

      class HistogramScalarDistribution : public ScalarDistribution
      {
        public:
          HistogramScalarDistribution(const TH1 * hist)
            :hist(hist) {; }

          virtual double value() { return hist->GetRandom(); } 

        private:
          const TH1 * hist; 

      }; 


      class FunctionScalarDistribution : public ScalarDistribution
      {

        public:
          FunctionScalarDistribution (TF1 * f) : f(f){;} 
          virtual double value() { return f->GetRandom(); } 
          virtual unsigned getNParameters() const { return f->GetNpar(); } 
          virtual void setParameters(const double * p) { f->SetParameters(p); } 
          virtual void setParameter(int i, double p) { f->SetParameter(i,p); } 

        private:
          TF1 * f; 
      }; 


    }
  }
}
    





#endif
