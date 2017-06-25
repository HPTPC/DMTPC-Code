#ifndef DMTPC_DAQ_CONFIG_XML_HH
#define DMTPC_DAQ_CONFIG_XML_HH

#include "Config.hh"
#include "Location.hh"
#include "tinyxml2.h"
#include <string>
#include "RunConfig.hh"
#include "CameraConfig.hh"
#include "ScopeConfig.hh"
 
namespace dmtpc
{
  namespace daq
  {
    class DAQConfigXml : public dmtpc::core::Config
    {
    public:
      DAQConfigXml(tinyxml2::XMLDocument *xmlDoc=0);  
      virtual ~DAQConfigXml() {;}
      // const dmtpc::core::Location * getLocation() const {return &location; }
      void fillRunConfig( dmtpc::core::RunConfig  *runConfig);
      void fillCameras( std::vector< dmtpc::core::CameraConfig > *vCameraConfig );
      void fillScopes( std::vector< dmtpc::core::ScopeConfig > *vScopeConfig );
      
    private:
      //     dmtpc::core::Location location; 
      tinyxml2::XMLDocument *xmlDoc;
      tinyxml2::XMLElement *  pRoot;
      dmtpc::core::CameraConfig xmlUnpackOneCcdCamera(tinyxml2::XMLElement * pRoot);
      dmtpc::core::ScopeConfig xmlUnpackOneScopeBoard(tinyxml2::XMLElement * pRoot);
      dmtpc::core::ScopeChannelConfig xmlUnpackOneScopeChannel(tinyxml2::XMLElement * pRoot, int boardId);
      void xmlValdateResult(tinyxml2::XMLElement * pRoot,std::string messages );
      void xmlUnpack(const char *name, Int_t *target,tinyxml2::XMLElement * pRoot);
      void xmlUnpack(const char *name, Float_t *target,tinyxml2::XMLElement * pRoot);
      void xmlUnpack(const char *name, Bool_t *target,tinyxml2::XMLElement * pRoot);
      TString  xmlUnpack(const char *name,tinyxml2::XMLElement * pRoot);
    };

#if 0    
    enum SHUTTER_MODE
      {
	ALWAYS_CLOSED, 
	ALWAYS_OPEN, 
	AUTO
      }; 
    
    enum EXPOSE_MODE
      {
	SIMPLE, 
	CONCURRENT
      }; 
#endif
  }
}


#endif
