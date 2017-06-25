#ifndef DMTPC_CORE_CAMERA_INFO
#define DMTPC_CORE_CAMERA_INFO

#include "TNamed.h"
#include <stdint.h>

namespace dmtpc
{
  namespace core
  {
    struct CameraInfo  : public TObject
    {
       CameraInfo() {;}  


       virtual ~CameraInfo(){}; 

       uint16_t nx; 
       uint16_t ny; 
       uint16_t camIndex; 
       uint16_t xbin; 
       uint16_t ybin; 
      
       uint16_t ul_x; 
       uint16_t ul_y; 
       uint16_t lr_x; 
       uint16_t lr_y; 

       uint16_t nflushes; 

       double ccdTemp ;
       double ccdTempSet; 

       uint32_t exposureTime; 
       float readoutSpeedVertical; 
       float readoutSpeedHorizontal; 


       bool dark; 
       bool shutterUsed; 

       TString serialNumber; 
       TString model; 

       uint8_t nbytes; 

       unsigned short overscanColumns; // number of column bins that are not exposed but digitized
       unsigned short overscanRows;    // number of row bins that are not exposed but digitized
       bool  digitizeOverscan;         // apply digitization to overscan pixels


       ClassDef(CameraInfo,1); 
    };
  }
}



#endif

