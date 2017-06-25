#ifndef DMTPC_DAQ_SCOPE_HH
#define DMTPC_DAQ_SCOPE_HH

#include <deque>
#include "ScopeInfo.hh" 

class TTimeStamp; 
namespace dmtpc
{
  namespace core
  {
    class ScopeConfig;
    class Waveform; 
  }
  namespace daq
  {
    class Scope 
    {
      public : 
        Scope() {; }
        /* Initialize the scope */ 
      virtual int init(const dmtpc::core::ScopeConfig *sc ) { 
	scopeCfg = sc;
      }

        /* Start acquisition */
        virtual int start() = 0; 

        /* Stop acquisition */ 
        virtual int stop() = 0; 

        /* Get size of buffer */ 
        virtual size_t getBufferSize() const = 0; 

        const dmtpc::core::ScopeInfo * getInfo() const { return &info; } 
        
        /* Returns non-zero if data is available to readout */ 
        virtual size_t dataAvailable() = 0; 

        /* Waits for data. Default implementation is busy wait on dataAvailable but can be imporved via interrupts if available */ 
        virtual size_t waitForData(unsigned timeout_in_ms); 

        /* Reads out to buffer */ 
        virtual size_t readOut(void * buf) = 0; 

        /* Makes waveforms from the buffer, returns number made.  */ 
        virtual size_t makeWaveforms(std::deque<dmtpc::core::Waveform*> *destination, const void * buf) const = 0; 

      protected:
        const dmtpc::core::ScopeConfig * scopeCfg;
        dmtpc::core::ScopeInfo info; 
        /* apply the settings in the config */
    };
  }
}

#endif
