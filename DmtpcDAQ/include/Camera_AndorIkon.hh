#ifndef DMTPC_DAQ_ANDOR_IKON_HH
#define DMTPC_DAQ_ANDOR_IKON_HH

#include "Camera.hh"
#include "Buffer.hh" 
#include <vector>



namespace dmtpc
{
  namespace daq
  {
    class AndorIkon : public Camera
    {
      public: 

        AndorIkon(long andorHandle, unsigned index, const DAQConfig * cfg); 
        virtual ~AndorIkon(); 
        static const char * getDriverNameStatic() { "AndorIkon"; } 
        virtual const char * getDriverName() { "AndorIkon"; } 
        virtual int expose(); 
        virtual int arm(); 
        virtual int setCoolerState(bool on); 
        virtual int setAutoShutter(bool on); 
        virtual int readOut(void* buf); 
        virtual unsigned getBufferSize(); 

        virtual int openShutter(); 
        virtual int closeShutter(); 
        virtual int waitAcquire(); 
        virtual double readTemperature();  


        virtual std::vector<double> getSupportedReadoutSpeeds(char dim) const; 

      private: 
        long andor_handle; 
        virtual int applyConfig(); 
        unsigned bufsize; 
        int open_time; 
        int closing_time; 

    };

  }
}

#endif
