#include "Camera_FLIProline.hh"
#include "TMutex.h"
#include "DAQConfig.hh"
#include "M3GlobalData.hh"
#include <libfli.h>
#include <assert.h>

static TMutex * fli_proline_lock = 0;

dmtpc::daq::FLIProline::~FLIProline()
{
  FLIClose(fli_dev); 
}

dmtpc::daq::FLIProline::FLIProline(long handle, unsigned index, 
				   const dmtpc::core::CameraConfig * camCfg)
{
  ccd_cfg = camCfg;

  auto_shutter_on = false; 
  ccd_info.camIndex = index; 
  fli_dev = handle; 

  char serial_buf[16]; 
  FLIGetSerialString(fli_dev,serial_buf, sizeof(serial_buf));  
  ccd_info.serialNumber = TString(serial_buf); 
  
  FLIGetVisibleArea(fli_dev,&visible_xl,&visible_yl,&visible_xu,&visible_yu); 

  // overscan implementation
  capture_xl = visible_xl+os_xl;
  capture_yl = visible_yl+os_yl;
  capture_xu = visible_xu+os_xu;
  capture_yu = visible_yu+os_yu;
  FLISetImageArea(fli_dev, capture_xl, capture_yl, capture_xu, capture_yu);
    
  ccd_info.nx = uint16_t(capture_xu - capture_xl); 
  ccd_info.ny = uint16_t(capture_yu - capture_yl); 
  ccd_info.nbytes = 2; 
  // this is not quite the full story
  // you really need to save all four: os_xl, os_yl, os_xu, os_yu
  ccd_info.overscanColumns=abs(os_xl);
  ccd_info.overscanRows=abs(os_xu);
  ccd_info.digitizeOverscan = true;
  printf("nx:%d, ny:%d\n",ccd_info.nx, ccd_info.ny); 

}

int dmtpc::daq::FLIProline::setCoolerState(bool on)
{

  /* This camera is always on */ 
  (void) on; 
  return 0; 
} 


int dmtpc::daq::FLIProline::arm()
{

  //TODO implement this
  return 1; 
}

int dmtpc::daq::FLIProline::waitAcquire()
{

  long device_status = -1; 
  long secs_left;  
  FLIGetExposureStatus(fli_dev,&secs_left) ;
  while(secs_left > 0)
  {
    FLIGetExposureStatus(fli_dev,&secs_left) ;
//    printf("%u msecs left\n",secs_left); 
    usleep(1000 * secs_left / 2); 
  }


  return 0; 

}

int dmtpc::daq::FLIProline::expose()
{
  printf("EXPOSING!\n"); fflush(stdout); 
  FLIExposeFrame(fli_dev); 
}

//need to check these
int dmtpc::daq::FLIProline::readOut(void *buf)
{

  uint16_t * short_buf = (uint16_t*) buf; 

  unsigned nrows = ccd_info.ny / ccd_info.ybin;
  unsigned ncols = ccd_info.nx / ccd_info.xbin;
  printf("FLIProline::readOut: nrows = %d, ncols = %d\n",nrows,ncols);

  for (unsigned r = 0; r < nrows; r++)
  {
//    printf("dev: %d row: %d\n",fli_dev, r); fflush(stdout); 
    FLIGrabRow(fli_dev,short_buf + r * ncols,ncols); 
  }
 
  return 0; 
}

int dmtpc::daq::FLIProline::setAutoShutter(bool on) 
{
  
  printf("Setting AutoShutter to %s\n", on? "on" : "off"); fflush(stdout); 
  FLISetFrameType(fli_dev, on ? FLI_FRAME_TYPE_NORMAL : FLI_FRAME_TYPE_DARK); 
}

unsigned dmtpc::daq::FLIProline::getBufferSize()  
{

  return sizeof(uint16_t)  * buffsize; 
}


int dmtpc::daq::FLIProline::applyConfig() 
{
  FLISetExposureTime(fli_dev, ccd_cfg->exposureTime); // in ms 
  FLISetHBin(fli_dev, ccd_cfg->xbin); 
  FLISetVBin(fli_dev, ccd_cfg->ybin); 
  /* TODO: Handle croppng */ 

  buffsize = (ccd_info.lr_x - ccd_info.ul_x) / ccd_info.xbin * (ccd_info.lr_y - ccd_info.ul_y) / ccd_info.ybin; 
  printf("buffsize: %d\n", buffsize); 
  FLISetTemperature(fli_dev,ccd_cfg->ccdTempSet); 

  /*
  long visible_xl,visible_xu,visible_yl,visible_yu;
  FLIGetVisibleArea(fli_dev,&visible_xl,&visible_yl,&visible_xu,&visible_yu); 
  printf("FLIProline::applyConfig(): visible_xl=%d, visible_xu=%d, visible_yl=%d, visible_yu=%d\n",visible_xl,visible_xu,visible_yl,visible_yu);

  long array_xl,array_xu,array_yl,array_yu;
  FLIGetArrayArea(fli_dev,&array_xl,&array_xu,&array_yl,&array_yu);
  printf("FLIProline::applyConfig(): array_xl=%d, array_xu=%d, array_yl=%d, array_yu=%d\n",array_xl,array_xu,array_yl,array_yu); 
   
  long capture_xl, capture_xu, capture_yl, capture_yu;
  capture_xl = visible_xl + os_xl;
  capture_yl = visible_yl + os_yl;
  capture_xu = visible_xu + os_xu;
  capture_yu = visible_yu + os_yu;
  FLISetImageArea(fli_dev, capture_xl, capture_yl, capture_xu, capture_yu); 
  */
  printf("FLIProline::applyConfig(): capture_xl=%d, capture_xu=%d, capture_yl=%d, capture_yu=%d\n",capture_xl,capture_xu,capture_yl,capture_yu);

}

//check shutter open and close operations
int dmtpc::daq::FLIProline::closeShutter()
{
  printf("Closing Shutter!\n"); fflush(stdout); 
  return FLIControlShutter(fli_dev, FLI_SHUTTER_CLOSE);
}



int dmtpc::daq::FLIProline::openShutter()
{
  printf("Opening Shutter!\n"); fflush(stdout); 
  return FLIControlShutter(fli_dev, FLI_SHUTTER_OPEN);
}

double dmtpc::daq::FLIProline::readTemperature() 
{

  double temperature; 
  FLIReadTemperature(fli_dev,FLI_TEMPERATURE_INTERNAL, &temperature); 
  printf("Camera %s TEMPERATURE: %f\n",ccd_info.serialNumber.Data(),temperature); 
  assert(gM3Data.mySqlTest);
  gM3Data.mySqlTest->command(Form("insert into ccdTemp_%s (value, timestamp) VALUES  (%f,NOW()) ",ccd_info.serialNumber.Data(),temperature));
  fflush(stdout); 
  ccd_info.ccdTemp= temperature; 
  return  temperature; 
}

