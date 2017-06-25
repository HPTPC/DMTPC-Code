#include "Event.hh" 
//#include "Image.hh" 
#include "Waveform.hh" 
#include "DetectorConfig.hh" 
#include "Location.hh" 
#include <iostream>
#include "TriggerInfo.hh"

#include "TTimeStamp.h" 

ClassImp(dmtpc::core::Event); 

dmtpc::core::Event::Event() 
{
  location = 0; 
  trigger = 0; 
  timestamp = 0; 
  detectorConfig= 0; 
  runNumber = -1;
  eventNumber = -1; 
}

const TH2 * dmtpc::core::Event::ccdData(unsigned i)  const { return _ccdData[i]->getHist(); }
const TH2 * dmtpc::core::Event::visible(unsigned i)  const { return _ccdData[i]->getVisible(); }
const TH2 * dmtpc::core::Event::overscan(unsigned i) const { return _ccdData[i]->getOverscan(); }


dmtpc::core::Event::Event(const Event & other, bool copy_data) 
{

  _ccdData = other._ccdData;  
  //  _overscan = other._overscan;  
  scopeData = other.scopeData;  
  scopeConfigs =other.scopeConfigs;  

  *detectorConfig = *other.detectorConfig;  
  *location = *other.location;  
  *trigger = *other.trigger;  
  *timestamp = *other.timestamp;  

  runNumber =other.runNumber;  
  eventNumber =other.eventNumber;  


  if (copy_data)
  {
    for (unsigned i = 0; i < _ccdData.size(); i++)
    {
      if (_ccdData[i]) _ccdData[i] = new Image(*other._ccdData[i]); 
      //      if (_overscan[i]) _overscan[i] = new Image(*other._overscan[i]); 
    }

    for (unsigned i = 0; i < scopeData.size(); i++)
    {
      if (scopeData[i]) scopeData[i] = new Waveform(*other.scopeData[i]); 
    }

    for (unsigned i = 0; i < scopeConfigs.size(); i++)
    {
      if (scopeConfigs[i]) scopeConfigs[i] = new ScopeInfo(*other.scopeConfigs[i]); 
    }
  }

}

const dmtpc::core::CameraInfo* dmtpc::core::Event::ccdConfig(unsigned i) const
{
  return _ccdData[i]->getInfo(); 
}

dmtpc::core::Event::~Event()
{
  

  for (unsigned i = 0; i < _ccdData.size(); i++)
  {
    if (_ccdData[i]) delete _ccdData[i]; 
  }

  /*
  for (unsigned i = 0; i < _overscan.size(); i++)
  {
    if (_overscan[i]) delete _overscan[i]; 
  }
  */

  for (unsigned i = 0; i < scopeData.size(); i++)
  {
    if (scopeData[i]) delete scopeData[i]; 
  }

  for (unsigned i = 0; i < scopeConfigs.size(); i++)
  {
    if (scopeConfigs[i]) delete scopeConfigs[i]; 
  }

  delete detectorConfig; 
  delete trigger; 
  delete timestamp; 

}

