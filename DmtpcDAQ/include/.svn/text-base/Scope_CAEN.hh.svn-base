#ifndef DMTPC_DAQ_CAEN_SCOPE_HH
#define DMTPC_DAQ_CAEN_SCOPE_HH

#include <deque>
#include "Scope.hh"
#include "ScopeInfo.hh"
#include "TTimeStamp.h"
#include "CAENDigitizer.h"

namespace dmtpc
{
  namespace core
  {
    class ScopeConfig;
  }
  namespace daq
  {
    class CAENDigitizer : public Scope
    {
      public : 
        CAENDigitizer(int handle, int board); 
        virtual int init(const dmtpc::core::ScopeConfig *scopeCfg); 
        virtual int start(); 
        virtual int stop(); 
        virtual size_t getBufferSize() const { return buf_size; } 
        virtual size_t readOut(void * buf); 

        virtual size_t makeWaveforms(std::deque<dmtpc::core::Waveform*> *destination, const void * buf) const; 
        virtual size_t dataAvailable(); 

//        virtual size_t waitForData(); 
      virtual size_t waitForData(unsigned timeout_in_ms); // use this if you want Scope_CAEN waitForData() to run
 
        static const char * getDriverNameStatic() { "CAENDigitizer" ;} 

        
      protected:
        size_t buf_size; 
        void * Evt;
        int handle; 
        int board; 
        TTimeStamp startTimeStamp;
        TTimeStamp endTimeStamp;

        virtual int enableExtendedTimeStamp(int h);
        virtual int boardSetup(int h);
        virtual uint32_t getChannelMask (const std::vector<unsigned> * ch);
        virtual std::string getChannelName(const std::vector<unsigned> * ch, int n, std::string r);
        virtual uint32_t setWindowSize(int h, unsigned int us);
        virtual uint32_t setPreTriggerSize(int h, unsigned int percent, unsigned int us);
        virtual int setChannelTrigger(int h, unsigned ch, double vpp, double vmin, double trglvl);
        virtual TTimeStamp getTraceTime(char * eventPointer, CAEN_DGTZ_EventInfo_t evInfo)const;
        virtual uint32_t getDC_dac(double dc, double vpp, double vmax);
       
    };
  }
}

#endif
