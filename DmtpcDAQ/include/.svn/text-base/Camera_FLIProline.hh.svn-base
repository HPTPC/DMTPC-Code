#ifndef DMTPC_DAQ_FLIPROLINE_HH
#define DMTPC_DAQ_FLIPROLINE_HH

#include "Camera.hh"
#include "Buffer.hh"
#include <vector>



namespace dmtpc
{
  namespace daq
  {
    class FLIProline : public Camera
    {
    public:
      
      FLIProline(long fli_dev, unsigned index, const dmtpc::core::CameraConfig * camCfg );
      virtual ~FLIProline();
      static const char * getDriverNameStatic() { "FLIProline"; }
      virtual const char * getDriverName() { "FLIProline"; }
      virtual int expose();
      virtual int arm();
      virtual int setCoolerState(bool on);
      virtual int setAutoShutter(bool on);
      virtual int readOut(void * buf);
      virtual unsigned getBufferSize();

      virtual int openShutter();
      virtual int closeShutter();
      virtual int waitAcquire();
      virtual double readTemperature();
      

    private:
      long fli_dev;
      virtual int applyConfig();
      unsigned buffsize;
      bool auto_shutter_on; 
      
      long visible_xl, visible_xu, visible_yl, visible_yu; 
      long capture_xl, capture_xu, capture_yl, capture_yu; // capture area, including overscan  

      // these are hard-coded overscan pixel values for the KAF-09000 sensor
      static const long os_xl = -20;
      static const long os_yl = -20;
      static const long os_xu = 9;
      static const long os_yu = 9;
    };
  }
}

#endif
