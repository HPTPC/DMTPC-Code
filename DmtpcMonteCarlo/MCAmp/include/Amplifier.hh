#ifndef DMTPC_MC_AMP_AMPLIFIER_HH
#define DMTPC_MC_AMP_AMPLIFIER_HH


class TH1; 

namespace dmtpc
{
  namespace mc
  {
    namespace amp
    {
      class Amplifier
      {
        public: 
          virtual TH1* result(const TH1 * target, TH1 * result = 0) const = 0;  
      }; 

      class ChargeSensitiveAmplifier : public Amplifier 
      {

        public:
          //gain in mV / pC or V / fC, tc in ns, bandwidth in Hz 
          ChargeSensitiveAmplifier(double gain=13, double tc = 50.e3, double bandwidth = 50e6)  : gain (gain), tc(tc), bandwidth(bandwidth) {;}

          virtual TH1* result(const TH1 * target, TH1 * result = 0) const; 
        private:
          double gain; 
          double tc; 
          double bandwidth; 

      };

      class FastAmplifier : public Amplifier 
      {

        public: 
          FastAmplifier(double gain = 80, double bandwidth_low = 10e6, double bandwidth_high = 250e6, int order = 2) ; 
          virtual TH1* result(const TH1 * target, TH1 * result = 0) const; 
        private:
          double gain; 
          double bandwidth_low; 
          double bandwidth_high; 
          mutable double *a,*b;
          int order; 
          mutable double last_norml; 
      };


    }

  }
} 


#endif
