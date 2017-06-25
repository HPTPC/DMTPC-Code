#include "DAQConfigXml.hh"
#include <assert.h>
#include <string>
using namespace std;
using namespace tinyxml2;

#include "M3GlobalData.hh"  

dmtpc::daq::DAQConfigXml::DAQConfigXml(tinyxml2::XMLDocument *xmlDocXX){
  xmlDoc=xmlDocXX;
  pRoot = xmlDoc->FirstChildElement("DMTPC_M3_Config");
  assert(pRoot);
  printf("found valid XML head-tag\n");
}


//====================
dmtpc::core::CameraConfig 
dmtpc::daq::DAQConfigXml::xmlUnpackOneCcdCamera(tinyxml2::XMLElement * pRoot){
  if(gM3Data.verbose>1) printf("in xmlUnpackOneCcdCamera ()\n");
  assert(pRoot);
  dmtpc::core::CameraConfig info;
  XMLError  eResult; 
  XMLElement * pElement;
  
  info.cameraType=pRoot->Attribute("type");
  if( info.cameraType.Sizeof()==0) xmlValdateResult(pRoot,"type");
  if(gM3Data.verbose>1)
    printf(" found: %s  type=%s enabled \n", pRoot->Name(),info.cameraType.Data());
  // common
  xmlUnpack("exposure_time",&info.exposureTime,pRoot);
  xmlUnpack("set_temp",&info.ccdTempSet,pRoot);
  xmlUnpack("xbin",&info.xbin,pRoot);
  xmlUnpack("ybin",&info.ybin,pRoot);

  if (  info.cameraType=="Fake" ) { //special case , reusing variables
      xmlUnpack("n_fake_cameras",&info.nflushes,pRoot);
  } else {
    xmlUnpack("nflushes",&info.nflushes,pRoot);
  }
  if(gM3Data.verbose>1) printf("in xmlUnpackOneCcdCamera ()  end\n");
  return info;
}


//====================
dmtpc::core::ScopeChannelConfig 
dmtpc::daq::DAQConfigXml::xmlUnpackOneScopeChannel(tinyxml2::XMLElement * pRoot, int boardId){
  if(gM3Data.verbose>1) printf("in xmlUnpackOneScopeChannel()\n");
  assert(pRoot);
  dmtpc::core::ScopeChannelConfig info;
  XMLError  eResult; 
  XMLElement * pElement;

  eResult= pRoot->QueryIntAttribute("id",&info.channel_id);
  if(  eResult) xmlValdateResult(pRoot,"id");
  if(gM3Data.verbose>1)
    printf(" found: %s  id=%d enabled \n", pRoot->Name(),info.channel_id);

  info.input_type= pRoot->Attribute("type");
  if(info.input_type.Sizeof()<=0) xmlValdateResult(pRoot,"type");

  //Ints
  xmlUnpack("vmin",&info.vmin,pRoot);
  xmlUnpack("vmax",&info.vmax,pRoot);
  xmlUnpack("dc_offset",&info.dc_offset,pRoot);
  xmlUnpack("trigger_threshold",&info.trigger_threshold,pRoot);
  xmlUnpack("pulse_polarity_falling",&info.pulse_polarity_falling,pRoot);
  xmlUnpack("trigger_polarity_falling",&info.trigger_polarity_falling,pRoot);
 
  info.scope=boardId;
  info.name=Form("scope_%d_ch_%d_", boardId,info.channel_id)+info.input_type;

  if(gM3Data.verbose>1) printf("in xmlUnpackOneScopeChannel()  end\n");
  return info;
}


//====================
dmtpc::core::ScopeConfig 
dmtpc::daq::DAQConfigXml::xmlUnpackOneScopeBoard(tinyxml2::XMLElement * pRoot){
  if(gM3Data.verbose>1) printf("in xmlUnpackOneScopeBoard()\n");
  assert(pRoot);
  dmtpc::core::ScopeConfig info;
  XMLError  eResult; 
  XMLElement * pElement;

  eResult= pRoot->QueryIntAttribute("id",&info.scope_id);
  if(  eResult) xmlValdateResult(pRoot,"id");
  if(gM3Data.verbose>1)
    printf(" found: %s  id=%d enabled \n", pRoot->Name(),info.scope_id);
  
  //Ints
  xmlUnpack("record_length",&info.record_length,pRoot);
  xmlUnpack("post_trigger",&info.post_trigger,pRoot);
  xmlUnpack("num_events_BLT",&info.num_events_BLT,pRoot);
  xmlUnpack("link_num",&info.link_num,pRoot);
  xmlUnpack("conet_node",&info.conet_node,pRoot);
  // TString
  info.connection_type=xmlUnpack("connection_type",pRoot);

  //.... unpacking list of elements using iterator
  const char *keyName="channel";
  pElement = pRoot->FirstChildElement(keyName);
  if (!pElement)  xmlValdateResult(pRoot,"missing any channel for CAEN_Board ");
  while (pElement) {
    bool enabled;
    eResult= pElement->QueryBoolAttribute("enabled",&enabled);
    if(  eResult) xmlValdateResult(pElement,"enabled");
    if(enabled) {
      dmtpc::core::ScopeChannelConfig infoChan=xmlUnpackOneScopeChannel(pElement,info.scope_id);
      if(gM3Data.verbose)infoChan.print(gM3Data.verbose);
      info.scopeChannelConfigs.push_back(infoChan);
    }
    // move to next element
    pElement = pElement->NextSiblingElement(keyName);
  }

  if(gM3Data.verbose>1)  info.print(gM3Data.verbose+1);
  
  if(gM3Data.verbose>1) printf("in xmlUnpackOneScopeBoard() end\n");
  return info;
}  



//====================
void dmtpc::daq::DAQConfigXml::fillRunConfig( dmtpc::core::RunConfig * runInfo ){
  XMLError  eResult; 
  XMLElement * pElement;
  
  eResult= pRoot->QueryIntAttribute("verbose",&gM3Data.verbose);
  if(  eResult) xmlValdateResult(pRoot,"verbose");
  printf("runInfo verbose=%d\n", gM3Data.verbose);

  // TString
  runInfo->ccd_light_shutter_mode= xmlUnpack("ccd_light_shutter_mode",pRoot);
  //obsolete  runInfo->out_dir=xmlUnpack("out_dir",pRoot);
  //obsolete runInfo->run_tag=xmlUnpack("run_tag",pRoot);

  printf("out_dir=%s\n",runInfo->out_dir.Data());
  // int,float,bool
  xmlUnpack("ccd_buffer_size",&runInfo->ccd_buffer_size,pRoot);
  xmlUnpack("ccd_threaded_readout",&runInfo->ccd_threaded_readout,pRoot);
  xmlUnpack("scope_buffer_size",&runInfo->scope_buffer_size,pRoot);
  xmlUnpack("ccd_threaded_readout",&runInfo->ccd_threaded_readout,pRoot);
  xmlUnpack("ccd_external_trigger",&runInfo->ccd_external_trigger,pRoot);
  //old  xmlUnpack("ccd_shutter_always_open",&runInfo->ccd_shutter_always_open,pRoot);

  xmlUnpack("ccd_dark_shutter_open",&runInfo->ccd_dark_shutter_open,pRoot);

  if(gM3Data.verbose>1) printf(" fillRunInfo() done\n");
}
 

 
//====================
void dmtpc::daq::DAQConfigXml::fillScopes( std::vector<dmtpc::core::ScopeConfig > *vScopeConfig){

  if(gM3Data.verbose>1) printf("In fillScopess() start, vecLen=%d\n",vScopeConfig->size()); 
  XMLError  eResult; 
  XMLElement * pElement;

  //.... unpacking list of elements using iterator
  const char *keyName="CAEN_Board";
  pElement = pRoot->FirstChildElement(keyName);
  if (!pElement)  xmlValdateResult(pRoot,"missing any CAEN_Board");
  while (pElement) {
    bool enabled;
    eResult= pElement->QueryBoolAttribute("enabled",&enabled);
    if(  eResult) xmlValdateResult(pElement,"enabled");
    if(enabled) {
      dmtpc::core::ScopeConfig info=xmlUnpackOneScopeBoard(pElement);
      if(gM3Data.verbose)info.print(gM3Data.verbose);
      vScopeConfig->push_back(info);
    }
    // move to next element
    pElement = pElement->NextSiblingElement(keyName);
  }

  if(gM3Data.verbose>1) printf("In fillScopes() end, vecLen=%d\n",vScopeConfig->size());

}
 
//====================
void dmtpc::daq::DAQConfigXml::fillCameras( std::vector<dmtpc::core::CameraConfig > *vCamaraConfig){

  if(gM3Data.verbose>1) printf("In fillCameras() start, vecLen=%d\n",vCamaraConfig->size());
 
  XMLError  eResult; 
  XMLElement * pElement;
  
  //.... unpacking list of elements using iterator
  const char *ccdKeyName="CCD_Camera";
  pElement = pRoot->FirstChildElement(ccdKeyName);
  if (!pElement)  xmlValdateResult(pRoot,"missing any CCD_Camera");
  while (pElement) {
    bool enabled;
    eResult= pElement->QueryBoolAttribute("enabled",&enabled);
    if(  eResult) xmlValdateResult(pElement,"enabled");
    if(enabled) {
      dmtpc::core::CameraConfig info=xmlUnpackOneCcdCamera(pElement);
      if(gM3Data.verbose)info.print(gM3Data.verbose);
      vCamaraConfig->push_back(info);
    }
    // move to next element
    pElement = pElement->NextSiblingElement(ccdKeyName);
  }

  if(gM3Data.verbose>1) printf("In fillCameras() end, vecLen=%d\n",vCamaraConfig->size());

}

//====================
void dmtpc::daq::DAQConfigXml::xmlValdateResult(XMLElement * pRoot,string message ){
  printf("FATAL content of XML , node name=%s, message=%s\n",pRoot->Name(), message.data());
  exit(99);
}


//====================
void dmtpc::daq::DAQConfigXml::xmlUnpack(const char *name, Int_t *target,tinyxml2::XMLElement * pRoot){
  XMLError  eResult; 
  XMLElement * pElement;
  
  pElement = pRoot->FirstChildElement(name);
  if (!pElement)  xmlValdateResult(pRoot,"missing key:"+string(name));      
  eResult = pElement->QueryIntText(target);
  if(eResult)  xmlValdateResult(pRoot,"wrong int-value for key:"+string(name));
  
}


//====================
void dmtpc::daq::DAQConfigXml::xmlUnpack(const char *name, Float_t *target,tinyxml2::XMLElement * pRoot){
  XMLError  eResult; 
  XMLElement * pElement;
  
  pElement = pRoot->FirstChildElement(name);
  if (!pElement)  xmlValdateResult(pRoot,"missing key:"+string(name));      
  eResult = pElement->QueryFloatText(target);
  if(eResult)  xmlValdateResult(pRoot,"wrong float-value for key:"+string(name));
  
}


//====================
void dmtpc::daq::DAQConfigXml::xmlUnpack(const char *name, Bool_t *target,tinyxml2::XMLElement * pRoot){
  XMLError  eResult; 
  XMLElement * pElement;
  
  pElement = pRoot->FirstChildElement(name);
  if (!pElement)  xmlValdateResult(pRoot,"missing key:"+string(name));      
  eResult = pElement->QueryBoolText(target);
  if(eResult)  xmlValdateResult(pRoot,"wrong bool-value for key:"+string(name));
  
}



//====================
TString  dmtpc::daq::DAQConfigXml::xmlUnpack(const char *name,tinyxml2::XMLElement * pRoot){
  XMLError  eResult; 
  XMLElement * pElement;
  
  pElement = pRoot->FirstChildElement(name);
  if (!pElement)  xmlValdateResult(pRoot,"missing key:"+string(name));      
  TString target=pElement->GetText();
  if(target.Sizeof()<=0)  xmlValdateResult(pRoot,"empty string-value for key:"+string(name));
  return target;
}
