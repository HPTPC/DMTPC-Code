#include "CameraLoader.hh"

#include "Camera_Fake.hh"

#ifdef _CAMERA_ANDOR_IKON_ 
#include "Camera_AndorIkon.hh" 
#include <atmcdLXd.h> 
#endif

#ifdef _CAMERA_ALTA_U6_ 
#include "Camera_AltaU6.hh" 
#endif

#ifdef _CAMERA_FLI_PROLINE_
#include "Camera_FLIProline.hh" 
#include "libfli.h"
#endif

#include <iostream>
#include <cstring>
#include <algorithm>

bool sort_cameras(const dmtpc::daq::Camera * a, const dmtpc::daq::Camera * b) 
{
  return strcmp(a->getInfo()->serialNumber.Data(), b->getInfo()->serialNumber.Data()) < 0; 
}

std::vector<std::string> dmtpc::daq::CameraLoader::getListOfDrivers()
{
  std::vector<std::string> ret; 
#ifdef _CAMERA_ANDOR_IKON_ 
  ret.push_back(AndorIkon::getDriverNameStatic()); 
#endif 

#ifdef _CAMERA_ALTA_U6_ 
  ret.push_back(AltaU6::getDriverName()); 
#endif 

#ifdef _CAMERA_FLI_PROLINE_
  ret.push_back(FLIProline::getDriverNameStatic()); 
#endif 
  return ret; 
}


void dmtpc::daq::CameraLoader::printListOfDrivers()
{
  std::vector<std::string> drivers = getListOfDrivers(); 

  for (unsigned i = 0; i < drivers.size() ;i++)
  {
    std::cout << drivers[i] << std::endl; 
  }
}

void dmtpc::daq::CameraLoader::loadFLIProline(const dmtpc::core::CameraConfig* CamCfg, 
					      std::vector<Camera * >& vCamera)
{
#ifdef _CAMERA_FLI_PROLINE_
    char ** fli_list; 
    FLICreateList(FLIDOMAIN_USB | FLIDEVICE_CAMERA); 

    const unsigned max_length = 255; 
    char file[max_length]; 
    char name[max_length]; 
    long domain; 
    unsigned fli_index = 0; 

    if(FLIListFirst(&domain, file, max_length, name, max_length) == 0)
    {
      do
      {
        printf("Found FLI Camera %s\n", name); 
        long handle; 
        if (!FLIOpen(&handle,file,domain))
        {
          printf("handle is %x\n", handle); 
          vCamera.push_back((Camera*) new FLIProline(handle,fli_index,CamCfg));
          fli_index++; 
        }
        else
        {
          fprintf(stderr,"WARNING: Could not open FLI Camera %s\n", name); 
        }
      }
      while((FLIListNext(&domain, file, max_length, name,max_length) == 0));
    }
#endif
  return;
}

void dmtpc::daq::CameraLoader::loadAndorIkon(const dmtpc::core::CameraConfig* CamCfg, 
					     std::vector<Camera * >& vCamera)
{
#ifdef _CAMERA_ANDOR_IKON_
  unsigned int andor_err; 
  int n_andor_cam = -1; 
  printf("Searching for andor cameras...\n"); 
  andor_err = GetAvailableCameras(&n_andor_cam); 
  
  if(andor_err != DRV_SUCCESS) 
    {
      fprintf(stderr, "Error %d from GetAvailableCameras\n", andor_err); 
    }
  
  if (n_andor_cam > 8) 
    {
      fprintf(stderr,"WARNING in CameraLoader::loadCameras: SDK claims it only supports up to 8 cameras\n"); 
    }

  for (long i = 0; i < n_andor_cam; i++)
    {
      int handle; 
      andor_err = GetCameraHandle(i, &handle); 
      if (andor_err!= DRV_SUCCESS)
	{
	  fprintf(stderr, "Error %d from GetCameraHandle\n", andor_err); 
	}
      else if (verbose)
	{
	  fprintf(stdout, "Loaded Andor iKon with handle %d\n",handle); 
	}

      vCamera.push_back((Camera*) new AndorIkon(handle,i,CamCfg)); 
    }
#endif
  return;
}

void dmtpc::daq::CameraLoader::loadFakeCamera(const dmtpc::core::CameraConfig* CamCfg, 
					      std::vector<Camera * >& vCamera)
{
  int nFakeCamera = CamCfg->nflushes;
  for (unsigned i = 0; i < nFakeCamera; i++)
  {
    vCamera.push_back((Camera*) new FakeCamera(i,CamCfg)); 
  }
  return;
}

std::vector<dmtpc::daq::Camera*> dmtpc::daq::CameraLoader::loadCameras(const std::vector<dmtpc::core::CameraConfig> * camCfgs)
{
  std::vector<Camera * > ret; 
  std::cout << "CameraLoader::loadCameras()" << std::endl;

  for (unsigned int i=0; i<camCfgs->size(); i++)
    {
      const dmtpc::core::CameraConfig* cc = &(camCfgs->at(i));
      printf("Camera type = %s\n", cc->cameraType.Data());

      if (cc->cameraType=="FLIProline") loadFLIProline(cc,ret); 
      else if (cc->cameraType=="AndorIkon") loadAndorIkon(cc,ret);
      else if (cc->cameraType=="Fake") loadFakeCamera(cc,ret);
      else {
	printf("CameraLoader::loadCameras() type =%s= unknown, Abort\n",cc->cameraType.Data());
	exit(77);
      }
    }

  //sort by serial number, alphabetical 
  std::sort(ret.begin(), ret.end(), sort_cameras); 


  if (ret.size() == 0 ) 
  {
    std::cerr << "WARNING: no cameras loaded!" << std::endl; 
  }

  return ret; 
}

