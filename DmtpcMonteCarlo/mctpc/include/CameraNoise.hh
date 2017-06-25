#ifndef DMTPC_MC_MCTPC_CAMERA_NOISE_HH
#define DMTPC_MC_MCTPC_CAMERA_NOISE_HH

#include "TFile.h" 


class TTree; 
class TH2; 
class DmtpcEvent; 

namespace dmtpc
{
  namespace core
  {
    class Event; 
  }

  namespace mc
  {
    namespace mctpc
    {

      class CameraNoise
      {
        public:
          virtual void newEvent() { ; } 
          virtual double getBias(int i, int j) = 0; 
          virtual double getNoise(int i, int j) = 0; 
          virtual ~CameraNoise() {; } 
      };

      class SimpleCameraNoise : public CameraNoise
      {

        public:

          SimpleCameraNoise(double bias, double noise) : bias(bias), noise(noise) {}
          virtual ~SimpleCameraNoise(){; }

          virtual double getBias(int i, int j) { return bias; } 
          virtual double getNoise(int i, int j) { return noise; } 


        private:

          double bias; 
          double noise; 

      }; 


      class CosMixCameraNoise : public CameraNoise
      {

        public: 
          CosMixCameraNoise(const char * serial, int run, const char * det_tag = "4sh", const char * dir= "/net/zwicky/esata04/dmtpc/data/4sh"); 
          virtual void newEvent(); 
          virtual double getBias(int i, int j) = 0; 
          virtual double getNoise(int i, int j) = 0; 

        private: 
          TFile f; 
          TH2 * bias; 
          int index; 
          bool compat; 
          TTree * tree; 
          DmtpcEvent* compat_event; 
          dmtpc::core::Event * event; 

      }; 
    }
  }
}

#endif

