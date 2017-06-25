#include "Camera_Fake.hh"
#include "CameraConfig.hh"

dmtpc::daq::FakeCamera::FakeCamera(unsigned index, const dmtpc::core::CameraConfig * camCfg) 
{
  ccd_cfg = camCfg;

  ccd_info.nx = 1024;
  ccd_info.ny = 1024;
  ccd_info.nbytes =2;
  ccd_info.serialNumber = TString::Format("FAKE%04u", index); 
  ccd_info.camIndex = index; 
}



std::vector<double> dmtpc::daq::FakeCamera::getSupportedReadoutSpeeds(char dim) 
{
  std::vector<double> v; 
  v.push_back(dim == 0 ? 1000000 : 1000); 
  return v; 
}

int dmtpc::daq::FakeCamera::readOut(void * buf)  
{
  memset(buf, 0,getBufferSize()); 
  return 0; 

}
int dmtpc::daq::FakeCamera::expose()
{
  watch.Start();
  printf("Exposing %s\n", ccd_info.serialNumber.Data()); 
  return 0; 
}

unsigned dmtpc::daq::FakeCamera::getBufferSize()  
{
  return sizeof(uint16_t)  * 1024 * 1024 / ccd_info.xbin / ccd_info.ybin; 
}


int dmtpc::daq::FakeCamera::waitAcquire()  
{

  while (watch.RealTime()*1000 < ccd_info.exposureTime)
  {
    double time_left =  (ccd_info.exposureTime - watch.RealTime()*1000.);
    watch.Continue(); 
//    printf("time_left: %d\n", int(time_left)); 
    usleep(time_left*500); 
  }

  return 0 ; 
}
