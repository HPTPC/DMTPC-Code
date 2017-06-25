#ifndef DMTPC_CORE_CAMERA_CONFIG
#define DMTPC_CORE_CAMERA_CONFIG

#include "TNamed.h"
#include <stdint.h>

namespace dmtpc
{
  namespace core
  {
    struct CameraConfig  : public TObject
    {
       CameraConfig() {
	 clear();}  

       virtual ~CameraConfig(){};

       void clear() {
	memset(this,0,sizeof( CameraConfig));
      }

      void print(int flag=0) { // depth of printing
	printf("CameraConfig dump: type=%s\n",cameraType.Data());
	printf("    xbin=%d ybin=%d  exposureTime=%d \n",xbin,ybin,exposureTime);
	if (flag) {;}
      }
      Int_t xbin;
      Int_t ybin;
      Int_t exposureTime;
      Int_t nflushes;
      
      Float_t ccdTempSet;

      TString cameraType;

      ClassDef(CameraConfig,1); 
    };
  }
}



#endif

