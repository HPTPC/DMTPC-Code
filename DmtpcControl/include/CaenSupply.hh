#ifndef DMTPC_CONTROL_CAEN_SUPPLY_HH
#define DMTPC_CONTROL_CAEN_SUPPLY_HH

//#include "CaenSupplyChannel.hh"
#include "TMutex.h"
#include <vector>

namespace dmtpc
{
  namespace control
  {
    class CaenSupply
    {
    public:
      CaenSupply(const char* namePort, const unsigned board, TMutex *passedMutex);
      ~CaenSupply(void);

      int initSupply();
      int closeSupply();

      // MONITOR commands
      std::vector<double> readVoltages();
      std::vector<double> readCurrents();
      std::vector<double> readSetVoltages();
      std::vector<double> readSetCurrents();
      std::vector<double> readTripValues();
      std::vector<double> readRampUpValues();
      std::vector<double> readRampDownValues();
      std::vector<double> readPowerDownValues();
      std::vector<double> readStatusValues();
      unsigned int        readBoardAlarmValue(bool print=false);

      // SET commands
      int setVoltages(double val);
      int setVoltage(unsigned ch, double val);
      int setCurrents(double val);
      int setCurrent(unsigned ch, double val);
      int setTripValues(double val);
      int setTripValue(unsigned ch, double val);
      int setRampUpValues(double val);
      int setRampDownValues(double val);
      int setPowerDownValuesToRamp();
      int setImonRangeToHigh();
      int setChannelsOn();
      int setChannelOn(unsigned ch);
      int setChannelsOff();
      int setChannelOff(unsigned ch);
      int clearBoardAlarm();

      //std::vector<CaenSupplyChannel*> SupplyChannels;

      unsigned getNumberChannels() { return m_noChannels; }
      //CaenSupplyChannel* getSupplyChannel(unsigned channel) { return SupplyChannels.at(channel); }

    protected:
      const char* m_portName;
      const unsigned m_boardAddr; // board address
      const static unsigned m_noChannels = 4;
      TMutex *myMutex;
     
      SerialHandle m_handle;
    };
  }
}

#endif
