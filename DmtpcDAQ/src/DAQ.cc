#include "DAQ.hh"
#include "CameraHandler.hh" 
#include "Writer.hh" 
#include "EventBuilder.hh" 
#include "TriggerHandler.hh"
#include "ScopeHandler.hh" 
#include "Stats.hh" 
#include "tinyxml2.h"
#include <ctime>
#include <assert.h>

#include <iostream>
#include "DAQConfigXml.hh"

#include "M3GlobalData.hh"
M3GlobalData gM3Data; // the only instance for this global variable

#define DEBUG 1 
 

dmtpc::daq::DAQ::DAQ(void)
{
  utime_bias_first = 0;
  utime_exposure_first = 0;
  utime_exposure_last = 0;
}


int dmtpc::daq::DAQ::init(const char* xmlConfName, const char* outName, int runnum) 
{
  // new XML-based config start

  //const char *xmlConfName= "dmtpc_m3_ver2.cfg.xml";
  printf("DAQ::init() Opening %s ...\n", xmlConfName);
  tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument( true,tinyxml2::COLLAPSE_WHITESPACE);
  tinyxml2::XMLError eResult =doc->LoadFile( xmlConfName);
  printf( "XML file '%s' read-in, ErrorID=%d\n", xmlConfName,  eResult );
  if(eResult) return  success=-100;

  DAQConfigXml *cfgXml=new DAQConfigXml(doc); // warn: XML sytax errors handled by asserts
  cfgXml->fillRunConfig(&runConfig); 
  runConfig.run_num = runnum;
  runConfig.output_filename = outName;
  if(gM3Data.verbose) runConfig.print(gM3Data.verbose);
  cfgXml->fillCameras(&vCameraConfig);
  cfgXml->fillScopes(&vScopeConfig);
  if(gM3Data.verbose)printf("done XML config in DAQ::init()\n");
  // new XML-based config end

  //load cameras
  const dmtpc::core::RunConfig *cRunConfig=&runConfig;
  const std::vector<dmtpc::core::CameraConfig> *cvCameraConfig=&vCameraConfig;
  const std::vector<dmtpc::core::ScopeConfig> *cvScopeConfig=&vScopeConfig;

  ccd_handler = new CameraHandler (cRunConfig, cvCameraConfig );
  ccd_handler->coolerOn(); 

  //load scopes
  scope_handler = new ScopeHandler(cRunConfig,cvScopeConfig); 

  //wait for cameras to be at temperature  
  ccd_handler->waitForCool(); 
  
  trigger = new TriggerHandler(cRunConfig); 
  stats = new Stats(cRunConfig,ccd_handler); 
  builder = new EventBuilder(cRunConfig,ccd_handler,scope_handler,trigger,stats); 
  writer = new Writer(cRunConfig,ccd_handler,builder); 

  return 0; 
}

dmtpc::daq::DAQ::~DAQ()
{
  printf("DAQ::~DAQ()\n");
  delete writer; 
  delete builder; 
  delete stats; 
  delete trigger;
  delete scope_handler;
  delete ccd_handler; 
}

int dmtpc::daq::DAQ::finish()
{
  std::cout << "DAQ::finish()" << std::endl;
  std::cout << "#utime_bias_first " << utime_bias_first << std::endl;
  std::cout << "#utime_exposure_first " << utime_exposure_first << std::endl;
  std::cout << "#utime_exposure_last " << utime_exposure_last << std::endl;

  return 0;
}

int dmtpc::daq::DAQ::stop()
{
  ccd_handler->cancelAll(); 
  scope_handler->stop(); 
  interrupted = true; 
}

int dmtpc::daq::DAQ::dark(int ndark) 
{
  std::cout << "DAQ::dark() dark = " << ndark << std::endl;
  
#ifdef DEBUG 
  std::cout << "dark(" << ndark << ")" << std::endl; 
#endif
  ccd_handler->setShutterMode(runConfig.ccd_dark_shutter_open ? CameraHandler::ALWAYS_OPEN : CameraHandler::ALWAYS_CLOSED); 
  utime_bias_first = std::time(0);
  ccd_handler->exposeAll(ndark); 
  writer->dark(); 
  ccd_handler->waitUntilDone(); 
  writer->flushDark(); 
}


int dmtpc::daq::DAQ::run(int ndark, int nlight,  bool continuous) 
{

  writer->start(); 

  if (continuous)
  {
   fprintf(stderr,"WARNING: continuous has never been tested, this will probably crash... \n"); 
   while(!interrupted)
    {
      dark(ndark); 
      scope_handler->start(); 
      event(nlight); 
      scope_handler->stop(); 
    }
  }
  else 
  {
      dark(ndark); 
      scope_handler->start(); 
      event(nlight); 
      scope_handler->stop(); 
  }

  writer->flush(); 

}

int dmtpc::daq::DAQ::event(int n) 
{
  std::cout << "DAQ::event() n = " << n << std::endl;
  if (runConfig.ccd_light_shutter_mode=="open") ccd_handler->setShutterMode(CameraHandler::ALWAYS_OPEN);
  else if (runConfig.ccd_light_shutter_mode=="closed") ccd_handler->setShutterMode(CameraHandler::ALWAYS_CLOSED);
  else if (runConfig.ccd_light_shutter_mode=="auto") ccd_handler->setShutterMode(CameraHandler::AUTO);
  else { std::cout << "DAQ::event Invalid ccd_light_shutter_mode"; assert(5==7); }
  utime_exposure_first = std::time(0);
  ccd_handler->exposeAll(n); 
  builder->start(); 
  ccd_handler->waitUntilDone(); 
  utime_exposure_last = std::time(0);
}





