#ifndef DMTPC_CORE_EVENT_HH
#define DMTPC_CORE_EVENT_HH

#include "TObject.h"
#include "Image.hh"
#include <vector>

class TTimeStamp; 
class TH2; 


namespace dmtpc
{
  namespace core
  {

    class Waveform;
    //class Image; 
    class DetectorConfig; 
    class CameraInfo; 
    class ScopeInfo; 
    class Location; 
    class TriggerInfo; 

    class Event : public TObject
    {
      public: 

         Event(); 
         Event(const Event &other, bool copy_data = true); 
         virtual ~Event(); 

         const CameraInfo * ccdConfig(unsigned i) const; 
         const Image *getImage(unsigned i) const { return _ccdData[i]; } 
         const TH2 *getVisible(unsigned i) const { return _ccdData[i]->getVisible(); }
         const TH2 *getOverscan(unsigned i) const { return _ccdData[i]->getOverscan(); } 
         unsigned nccd() const { return _ccdData.size(); }
         const TH2 * ccdData(unsigned i) const;
         const TH2 * visible(unsigned i) const;
         const TH2 * overscan(unsigned i) const;
         const Waveform * getTrace(unsigned i) const { return scopeData[i]; }
         const Waveform * getTrace(int channel, int trigger, int nch) const { return scopeData[nch*trigger+channel]; }

         unsigned nscopes() const { return scopeConfigs.size() ; } 
         const ScopeInfo * getScopeConfig(unsigned scope)const { return scopeConfigs[scope]; }
         unsigned nScopeData() const { return scopeData.size(); }
         int run() const { return runNumber; }
         int ev() const { return eventNumber; }
         const TTimeStamp * getTimestamp() const { return timestamp; }

      protected: 

         std::vector<Image*> _ccdData; 
         //std::vector<Image*> _overscan; 
         std::vector<Waveform*> scopeData; 
         std::vector<ScopeInfo*> scopeConfigs;
         DetectorConfig * detectorConfig; 

         Location * location; 
         TriggerInfo * trigger; 
         TTimeStamp * timestamp; 

         int runNumber; 
         int eventNumber; 

         ClassDef(Event,2); 
    }; 
  }
}

#endif

