#ifndef DMTPC_DAQ_CAMERA_HH
#define DMTPC_DAQ_CAMERA_HH

#include "CameraInfo.hh"
#include "CameraConfig.hh"
#include "Buffer.hh" 
#include "Image.hh"
#include <vector>

class TH2; 

namespace dmtpc
{
  namespace daq
  {
    class DAQConfig; 

    class Camera 
    {
      public: 

        virtual int expose() = 0; 
        virtual int arm() = 0; 

        int setTargetTemperature(double temp) {ccd_info.ccdTempSet = temp; applyConfig(); }
        virtual int setCoolerState(bool on) = 0; 

        virtual int setAutoShutter(bool on) = 0; 
        virtual int readOut(void* buf) = 0; 
        virtual unsigned getBufferSize() = 0; 

        virtual dmtpc::core::Image * makeImage(const void * buf) const; 
        virtual dmtpc::core::Image * makeOverscan(const void * buf) const  { return 0; } 

        virtual int openShutter() = 0; 
        virtual int closeShutter() = 0; 
        virtual int waitAcquire() = 0; 
        virtual double readTemperature() = 0;  
        double getTargetTemperature() const { return ccd_info.ccdTempSet; } 

        virtual int setConfig();
        const dmtpc::core::CameraInfo * getInfo() const { return &ccd_info; } 

        virtual const char * getDriverName() = 0; 
        virtual std::vector<double> getSupportedReadoutSpeeds(char dim) const; 



      protected: 
        dmtpc::core::CameraInfo ccd_info; 
        const dmtpc::core::CameraConfig * ccd_cfg;
        virtual int applyConfig() = 0; 

        // 0 = horizontal, 1 = vertical 
        virtual uint32_t closestReadoutSpeed(char dim) const; 
        uint32_t index; 
        bool ok; 

    }; 
  }
}
#endif
