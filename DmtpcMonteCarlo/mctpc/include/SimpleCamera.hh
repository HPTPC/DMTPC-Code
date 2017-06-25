#ifndef DMTPC_MC_MCTPC_SIMPLE_CAMERA_HH
#define DMTPC_MC_MCTPC_SIMPLE_CAMERA_HH

#include "Process.hh" 
#include "TRandom3.h" 
#include "TH2.h" 

namespace dmtpc
{
  namespace mc
  {
    namespace mctpc
    {
        
      class SimpleCamera : public Process
      {
        public: 
          SimpleCamera(int nx, int ny, int nbin, const char * name = 0 , const TH2 * gainmap = 0); 
          void setScale(double mm_per_pix) { scale = mm_per_pix; } 
          void setAcceptance(double acceptance) { acc = acceptance; } 
          void setNoise(double noise_in_adu) { noise = noise_in_adu; } 
          void setBias(double bias_in_adu) { bias = bias_in_adu; } 
          void setLocation(double x, double y) {x0 =x; y0=y; } 
          void setGain(double e_per_adu) { gain = e_per_adu; } 
          void setGainMap(const TH2 * map) { gainmap = map; } 
          void setQE(double quantum_efficiency) { qe = quantum_efficiency; }
          void setRotation(double rotation){ rot = rotation; }
          void setBlur(double sig) { blur = sig; } 
	  virtual void prepareSignals(dmtpc::analysis::MCTruth * truth); 
          virtual void clearSignals(); 
             

        protected:
          virtual int doProcess(unsigned size, const  Particle * in); 
          virtual int setSeed(long seed) { rng.SetSeed(seed); return 0; } 

        private: 
          TH2I hist; 
          TH2I biasframe; 
          const TH2 * gainmap;
          double acc; 
          double noise; 
          double blur; 
          double rot; 
          double gain; 
          int nx, ny, nbin; 
          double qe; 
          double bias; 
          double scale; 
          double x0; 
          double y0; 
          TRandom3 rng; 

      }; 


    }
  }
}

#endif
