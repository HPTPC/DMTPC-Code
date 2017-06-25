
#include "Stats.hh" 
#include "DAQConfig.hh"
#include "RunConfig.hh" 
#include "CameraHandler.hh" 

dmtpc::daq::Stats::Stats(const dmtpc::core::RunConfig * runCfg, const CameraHandler * ccd_handler)
  : ccd_handler(ccd_handler) 
{
  store = 0; 

}
