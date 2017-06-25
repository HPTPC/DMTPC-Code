#ifndef DMTPC_CONTROL_CHANNEL_HH
#define DMTPC_CONTROL_CHANNEL_HH

/** Opaque interface to channel classes **/ 

#include "ChannelConfig.hh"
#include <string>

namespace dmtpc
{
  namespace control
  {
    class Store; 
    class ChannelConfig; 

    class Channel 
    {
      public:
        Channel(std::string myName) { name=myName; }
        int save(Store * store); // was virtual before
        double readValue(Store * store); // did not exist before
        const char* getName() const { return name.c_str(); } 
        double getValue() const { return value; } 
      // double getValueRMS() const { return value_rms; } 
      //virtual bool hasRMS() const { return false; } 
        virtual const char * description() { return "Base Channel"; } 

        double value;

      protected:
      //void setName(std::string newName) { name=newName; }
        std::string name; 
      //double value; 
      //double value_rms; 
    }; 

    class WriteChannel : virtual public Channel 
    {
      public: 
          WriteChannel(std::string myName) : Channel(myName) {}
          virtual int write(double val) = 0; 
      //virtual int write(Store * store) = 0; 
      //    virtual int limit(double val) = 0; 
    };

    class ReadChannel : virtual public Channel 
    {
      public: 
          ReadChannel(std::string myName) : Channel(myName) {}
          virtual double read() = 0; 
    };

    class HVChannel : public ReadChannel, public WriteChannel 
    {
      public: 
          virtual int write(double val) = 0; 
          virtual int write(Store * store) = 0; 
          virtual double read() = 0; 
          virtual int limit(double val) = 0; 
          /* Set the maximum rate of change in voltage */ 
          virtual int ramp(double volts_per_second) = 0; 
    }; 

    class DOChannel : public Channel
    {
      public:
        virtual unsigned nchannels() const = 0; 
        virtual int write(const bool * writeme) = 0; 
        virtual int setFireLength(long usecs) = 0; 
        virtual int fire(const bool* fireme = 0) = 0; 
    }; 

    class DIChannel : public Channel
    {

      public:
        virtual unsigned nchannels() const = 0; 
        virtual double read(bool* fillme) = 0; 
    }; 

 
    /* Factory methods to create channels from a ChannelConfig
     **/ 

    namespace factory
    {
      Channel * make(const ChannelConfig * cfg); 
      DOChannel * makeDO(const ChannelConfig * cfg);
      DIChannel * makeDI(const ChannelConfig * cfg);
      WriteChannel * makeWrite(const ChannelConfig * cfg);
      ReadChannel * makeRead(const ChannelConfig * cfg);
      HVChannel * makeHV(const ChannelConfig * cfg);
    }
  }
}

#endif
