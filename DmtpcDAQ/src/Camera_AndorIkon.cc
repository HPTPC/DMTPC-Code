#include "Camera_AndorIkon.hh" 
#include "TMutex.h" 
#include "DAQConfig.hh"
#include <atmcdLXd.h> 


static TMutex * andor_ikon_lock = 0; 



static void lock() 
{ 
  if (andor_ikon_lock) andor_ikon_lock->Lock(); 
}

static void unlock() 
{ 
  if (andor_ikon_lock) andor_ikon_lock->UnLock(); 
}

dmtpc::daq::AndorIkon::~AndorIkon()
{

  lock(); 

  SetCurrentCamera(andor_handle); 
  ShutDown(); 

  unlock(); 
}



dmtpc::daq::AndorIkon::AndorIkon(long handle, unsigned index, const DAQConfig * cfg)
{

  if (index  == 1 )
  {
    andor_ikon_lock = new TMutex(); 
  }
  ccd_info.camIndex = index; 
  andor_handle = handle; 


  lock(); 
  SetCurrentCamera(handle);

  //TODO make sure cast is ok
  unsigned long err = Initialize((char*) cfg->getString("andor_init_dir")); 

  if (err!=DRV_SUCCESS)
  {
    std::cerr << "Error initializing AndorIkon camera " << index << " with handle " << handle <<". Err: " << err << std::endl; 
    ok = false; 
    return; 
  }

  sleep(2); //apparently it takes a while to initialize? 
  SetReadMode(4); // image mode
  SetAcquisitionMode(1); //single scan 
  int serial;  
  GetCameraSerialNumber(&serial); 
  ccd_info.serialNumber = TString::Format("%d",serial); 

  GetShutterMinTimes(&closing_time, &open_time); 

  int nx, ny; 
  GetDetector(&nx, &ny); 
  ccd_info.nx = uint16_t(nx); 
  ccd_info.ny = uint16_t(ny); 
  ccd_info.nbytes = 2; 
  ccd_info.digitizeOverscan = false;  //I don't think there's an overscan option here
  SetFilterMode(0); //don't want cosmic filter, right? 
  SetBaselineClamp(1); //

  int depth; 

  unlock(); 

}


int dmtpc::daq::AndorIkon::setCoolerState(bool on)
{
  lock(); 
  SetCurrentCamera(andor_handle);
  on ? CoolerON() : CoolerOFF(); 
  unlock(); 
}

std::vector<double> dmtpc::daq::AndorIkon::getSupportedReadoutSpeeds(char dim) const
{

  int nspeeds; 
  if (dim == 0)
  {
    int nchannels; 
    GetNumberADChannels(&nchannels); 
    printf("n ad channels: %d\n",nchannels); 
  }

  dim == 0 ? GetNumberHSSpeeds(0,0,&nspeeds) : GetNumberVSSpeeds(&nspeeds); 
  std::vector<double> speeds(nspeeds); 

  for (int i = 0; i < nspeeds; i++)
  {
    float speed; 
    dim == 0 ? GetHSSpeed(0,0,i,&speed)  : GetVSSpeed (i,&speed); 
    speeds[i]= speed; 
  }

  return speeds; 

}

int dmtpc::daq::AndorIkon::arm()
{

  //TODO implement this
  return 1; 
}



int dmtpc::daq::AndorIkon::waitAcquire()
{

#ifdef _ANDOR_IKON_USE_BUSY_WAIT_
  lock(); 
  SetCurrentCamera(andor_handle); 
  int status; 
  GetStatus (&status); 
  while (status == DRV_ACQUIRING)
  {
    GetStatus(&status); 
  }
  unlock(); 
  return 0; 
  
#else
  return WaitForAcquisitionByHandle(andor_handle); 
#endif
}

int dmtpc::daq::AndorIkon::expose()
{

  lock();
  SetCurrentCamera(andor_handle); 
  int ret = StartAcquisition(); 
  unlock();
  return ret; 
}

int dmtpc::daq::AndorIkon::readOut(void *buf)
{
  lock(); 
  SetCurrentCamera(andor_handle);
  GetAcquiredData16((uint16_t*)buf, bufsize); 
  unlock(); 
}

int dmtpc::daq::AndorIkon::setAutoShutter(bool on) 
{
  lock(); 
  SetCurrentCamera(andor_handle); 
  int ret = SetShutter(1, on ? 0 : 1, open_time, closing_time);  
  unlock(); 

  return ret; 
}

unsigned dmtpc::daq::AndorIkon::getBufferSize()  
{

  return sizeof(uint16_t)  * bufsize; 
}

int dmtpc::daq::AndorIkon::applyConfig() 
{

  lock(); 
  SetCurrentCamera(andor_handle); 
  SetExposureTime(ccd_info.exposureTime/1000.);  //ms -> s
  SetHSSpeed(0,closestReadoutSpeed(0)); 
  SetVSSpeed(closestReadoutSpeed(1)); 
  int ret = SetImage(ccd_info.xbin, ccd_info.ybin, ccd_info.ul_x+1, ccd_info.lr_x, ccd_info.ul_y+1, ccd_info.lr_y); 
  bufsize = (ccd_info.lr_x - ccd_info.ul_x) / ccd_info.xbin * (ccd_info.lr_y - ccd_info.ul_y) / ccd_info.ybin; 
  SetTemperature((int) ccd_info.ccdTempSet); 
  unlock(); 
}

int dmtpc::daq::AndorIkon::closeShutter()
{
  lock(); 
  SetCurrentCamera(andor_handle); 
  int ret = SetShutter(1,  2, open_time, closing_time);  //!???
  unlock(); 

  return ret; 
}



int dmtpc::daq::AndorIkon::openShutter()
{
  lock(); 
  SetCurrentCamera(andor_handle); 
  int ret = SetShutter(1,  1, open_time, closing_time);  //!???
  unlock(); 

  return ret; 
}

double dmtpc::daq::AndorIkon::readTemperature() 
{

  lock(); 
  SetCurrentCamera(andor_handle);
  float temp; 
  GetTemperatureF(&temp); 
  unlock(); 
  ccd_info.ccdTemp= temp; 
  return (double) temp; 
}


