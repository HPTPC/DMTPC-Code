#ifndef DMTPC_DAQ_CAMERA_HANDLER_HH
#define DMTPC_DAQ_CAMERA_HANDLER_HH
#include <vector>
#include "Camera.hh"

#include "TTimeStamp.h" 
#include "TThread.h" 
#include "DAQConfig.hh"
#include "CameraConfig.hh"
#include "RunConfig.hh"
#include "Buffer.hh" 


namespace dmtpc
{
  namespace control
  {
    class DOChannel; 
  }

  namespace daq
  {
    class CameraHandler
    {
      public:
        enum SHUTTER_MODE
        {
          ALWAYS_OPEN,
          ALWAYS_CLOSED,
          AUTO
        }; 

        CameraHandler(const dmtpc::core::RunConfig * runCfg, 
		      const std::vector<dmtpc::core::CameraConfig> * camCfgs); 
        int coolerOn(); 
        int waitForCool(double eps = 0.5); 
        int exposeAll(unsigned n); 
        int cancelAll() { cancel =true; expose_thread->Join(); done_yet= true; } 
        int waitUntilDone() { return expose_thread->Join(); done_yet= true; } 
        int setShutterMode(SHUTTER_MODE mode); 
        bool done() const; 
        bool empty() const; 
        Buffer* getBuffer(int i) { return buffers[i]; } 
        unsigned getNCameras() const { return ncam; } 
        const Camera * getCamera(unsigned i)const  { return cameras[i]; } 
        const TTimeStamp * getTimeStamp(unsigned i) const { return &timestamps[i]; } 
        const TTimeStamp * getNextTimeStamp() { return &timestamps[lastTimeStamp++]; } 
        const TTimeStamp * peekNextTimeStamp() const { return &timestamps[lastTimeStamp]; } 
        bool hasNextTimeStamp() { return timestamps.size() >= lastTimeStamp + 1; } 

      private: 
        std::vector<Camera*> cameras; 
        std::vector<Buffer*> buffers;
        std::vector<TThread*> wait_threads; 
        std::vector<TTimeStamp> timestamps; 
        volatile unsigned lastTimeStamp; 
        volatile bool done_yet; 
        volatile bool cancel; 
        unsigned ncam; 
        unsigned image_count; // this is for database progress bar 

        TThread * expose_thread; 
        const DAQConfig * cfg; 
        dmtpc::control::DOChannel * trigger_channel; 
        bool external_trigger; 
        bool threaded_readout;
        mutable TMutex critical; 
    }; 

  }
}

#endif
