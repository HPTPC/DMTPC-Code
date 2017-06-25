#include "Camera.hh" 
#include "TH2.h" 
#include <cmath>
#include "DAQConfig.hh"
#include "CameraConfig.hh"
#include <cmath>


/* Generic implementation assuming unsigned int image **/ 

dmtpc::core::Image* dmtpc::daq::Camera::makeImage(const void * buf) const
{
  return new dmtpc::core::Image(ccd_info.serialNumber, ccd_info.serialNumber, buf, &ccd_info); 
}



int dmtpc::daq::Camera::setConfig() 
{
   ccd_info.xbin = ccd_cfg->xbin; 
   ccd_info.ybin = ccd_cfg->ybin; 

   /*
   if (cfg->getBool("ccd_crop"))
   {
      ccd_info.ul_x = (uint16_t) cfg->getDouble("ccd_ul_x_frac") * ccd_info.nx; 
      ccd_info.ul_y = (uint16_t) cfg->getDouble("ccd_ul_y_frac") * ccd_info.ny; 
      ccd_info.lr_x = (uint16_t) cfg->getDouble("ccd_lr_x_frac") * ccd_info.nx; 
      ccd_info.lr_y = (uint16_t) cfg->getDouble("ccd_lr_y_frac") * ccd_info.ny; 
   }
   
   else
   {
   */
      ccd_info.ul_x = 0;
      ccd_info.ul_y = 0;
      ccd_info.lr_x = ccd_info.nx; 
      ccd_info.lr_y = ccd_info.ny;
      //}

   ccd_info.nflushes = ccd_cfg->nflushes; 
   ccd_info.ccdTempSet = ccd_cfg->ccdTempSet; 
   ccd_info.exposureTime = ccd_cfg->exposureTime; 
   //ccd_info.readoutSpeedHorizontal = cfg->ccd_readout_speed_h); 
   //ccd_info.readoutSpeedVertical = cfg->ccd_readout_speed_v); 

   return applyConfig(); 
}


/** When none is available use this */ 
std::vector<double> dmtpc::daq::Camera::getSupportedReadoutSpeeds(char dim) const
{
  (void) dim; 
  std::vector<double> v (1); 
  return v; 
}



uint32_t dmtpc::daq::Camera::closestReadoutSpeed(char dim) const
{
  //pick closest readout speed 
  double min_delta = -1; 
  int speed_index = 0; 
  double wanted_speed = dim == 0 ? ccd_info.readoutSpeedHorizontal : ccd_info.readoutSpeedVertical; 
  vector<double> speeds = getSupportedReadoutSpeeds(dim); 

  for (unsigned i = 0; i < speeds.size(); i++)
  {
     printf("%s speed %d is %f\n",dim==0 ? "horizontal" : "vertical", i, speeds[i]); 
     double delta = fabs(speeds[i] - wanted_speed); 
     if (delta == 0)
     {
       speed_index = i;
       break; 
     }
     else if (min_delta < 0 || delta < min_delta)
     {
       min_delta = delta; 
       speed_index = i; 
     }
  }

  printf("Closest %s readout speed to %f is %f\n", dim == 0 ? "horizontal" : "vertical", wanted_speed, speeds[speed_index]); 

  return speed_index; 
}

