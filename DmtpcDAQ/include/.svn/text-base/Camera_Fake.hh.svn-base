
#ifndef DMTPC_DAQ_FAKE_CAMERA_HH
#define DMTPC_DAQ_FAKE_CAMERA_HH

#include "TStopwatch.h"
#include "Camera.hh"

namespace dmtpc
{
  namespace core
  {
    class CameraConfig;
  }
  namespace daq
  {
    class FakeCamera : public Camera
    {
      public: 

        FakeCamera(unsigned index, const dmtpc::core::CameraConfig * camCfg); 

        virtual int expose() ;
        virtual int arm() { ; } 

        virtual int setTargetTemperature(float temp) { ccd_info.ccdTempSet = temp; } 
        virtual int setCoolerState(bool on) {(void)on;return 0 ; }

        virtual int setAutoShutter(bool on)  { return 0; } 
        virtual int readOut(void* buf); 
        virtual unsigned getBufferSize();


        virtual int openShutter()  {return 0;} 
        virtual int closeShutter() {return 0;} 
        virtual int waitAcquire();

        virtual const dmtpc::core::CameraInfo * getInfo() const { return &ccd_info; } 

        virtual const char * getDriverName() {return "FakeCamera";} 
        virtual size_t getXResolution()  {return 1024;}
        virtual size_t getYResolution()  {return 1024;} 
        virtual std::vector<double> getSupportedReadoutSpeeds(char dim); 
        virtual double readTemperature() { return ccd_info.ccdTempSet; } 
      protected:

        virtual int applyConfig(){ ;}  
      private:
        TStopwatch watch; 

    }; 
  }
}
#endif
