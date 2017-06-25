#ifndef DMTPC_DAQ_CAMERA_LOADER
#define DMTPC_DAQ_CAMERA_LOADER

#include <vector>
#include "Camera.hh" 
#include "CameraConfig.hh" 

namespace dmtpc
{
  namespace daq
  {
    namespace CameraLoader
    {
      std::vector<std::string> getListOfDrivers();  
      void printListOfDrivers();  
      void loadFLIProline(const dmtpc::core::CameraConfig* CamCfg, 
			  std::vector<Camera * >& vCamera);
      void loadAndorIkon (const dmtpc::core::CameraConfig* CamCfg, 
			  std::vector<Camera * >& vCamera);
      void loadFakeCamera(const dmtpc::core::CameraConfig* CamCfg, 
			  std::vector<Camera * >& vCamera);
      std::vector<Camera*> loadCameras(const std::vector<dmtpc::core::CameraConfig> * camCfgs); 
    }
  }
}

#endif
