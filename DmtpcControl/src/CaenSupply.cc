#include "CaenUtility.hh"
#include "CaenSupply.hh"
#include <iostream>

dmtpc::control::CaenSupply::CaenSupply(const char* namePort,
				       const unsigned board,
				       TMutex *passedMutex) : 
  m_portName(namePort),
  m_boardAddr(board),
  myMutex(passedMutex)
{
  // create pointers for each channel of Caen supply
  //for (unsigned i=0; i<getNumberChannels(); i++)
  //SupplyChannels.push_back(new dmtpc::control::CaenSupplyChannel(m_portName,m_boardAddr,i,myMutex));

  m_handle = 0;
}

dmtpc::control::CaenSupply::~CaenSupply(void)
{
  //for (std::vector<int>::size_type i=0; i!=SupplyChannels.size(); i++)
  //if (SupplyChannels.at(i)) delete SupplyChannels.at(i);
}

int dmtpc::control::CaenSupply::initSupply()
{
  if (m_handle)
    {
      std::cout << "SerialHandle for CaenSupply already exists!" << std::endl;
      return -1;
    }
  else m_handle = dmtpc::control::CaenUtility::openDevice(m_portName);

  if (!m_handle) std::cout << "initSupply() returned NULL SerialHandle" << std::endl;
  return (int)m_handle;
}


int dmtpc::control::CaenSupply::closeSupply()
{
  if (m_handle)
    {
      dmtpc::control::CaenUtility::closeDevice(m_handle);
      return 0;
    }
  else if (!m_handle) std::cout << "No handle for closeSupply()!" << std::endl;
  return -1;
}


std::vector<double> dmtpc::control::CaenSupply::readVoltages()
{
  return dmtpc::control::CaenUtility::readParameters(m_handle,m_boardAddr,"VMON");
}

std::vector<double> dmtpc::control::CaenSupply::readCurrents()
{
  return dmtpc::control::CaenUtility::readParameters(m_handle,m_boardAddr,"IMON");
}

std::vector<double> dmtpc::control::CaenSupply::readSetVoltages()
{
  return dmtpc::control::CaenUtility::readParameters(m_handle,m_boardAddr,"VSET");
}

std::vector<double> dmtpc::control::CaenSupply::readSetCurrents()
{
  return dmtpc::control::CaenUtility::readParameters(m_handle,m_boardAddr,"ISET");
}

std::vector<double> dmtpc::control::CaenSupply::readTripValues()
{
  return dmtpc::control::CaenUtility::readParameters(m_handle,m_boardAddr,"TRIP");
}

std::vector<double> dmtpc::control::CaenSupply::readRampUpValues()
{
  return dmtpc::control::CaenUtility::readParameters(m_handle,m_boardAddr,"RUP");
}

std::vector<double> dmtpc::control::CaenSupply::readRampDownValues()
{
  return dmtpc::control::CaenUtility::readParameters(m_handle,m_boardAddr,"RDW");
}

std::vector<double> dmtpc::control::CaenSupply::readPowerDownValues()
{
  return dmtpc::control::CaenUtility::readParameters(m_handle,m_boardAddr,"PDWN");
}

std::vector<double> dmtpc::control::CaenSupply::readStatusValues()
{
  return dmtpc::control::CaenUtility::readParameters(m_handle,m_boardAddr,"STAT");
}

unsigned int dmtpc::control::CaenSupply::readBoardAlarmValue(bool print)
{
  unsigned int alarmValue = (unsigned int)dmtpc::control::CaenUtility::readParameters(m_handle,m_boardAddr,"BDALARM").at(0);
  if (print)
    {
      if (alarmValue &  1) std::cout << "Caen channel 0 in Alarm status!" << std::endl;
      if (alarmValue &  2) std::cout << "Caen channel 1 in Alarm status!" << std::endl;
      if (alarmValue &  4) std::cout << "Caen channel 2 in Alarm status!" << std::endl;
      if (alarmValue &  8) std::cout << "Caen channel 3 in Alarm status!" << std::endl;
      if (alarmValue & 16) std::cout << "Caen board in POWER FAIL!"       << std::endl;
      if (alarmValue & 32) std::cout << "Caen board in OVER POWER!"       << std::endl;
      if (alarmValue & 64) std::cout << "Caen internal HV clock FAIL!"   << std::endl;
    }
  return alarmValue;
}


int dmtpc::control::CaenSupply::setVoltage(unsigned ch, double val)
{
  return dmtpc::control::CaenUtility::setParameter(m_handle,m_boardAddr,ch,"VSET",val);
}

int dmtpc::control::CaenSupply::setVoltages(double val)
{
  return dmtpc::control::CaenUtility::setParameters(m_handle,m_boardAddr,"VSET",val);
}

int dmtpc::control::CaenSupply::setCurrent(unsigned ch, double val)
{
  return dmtpc::control::CaenUtility::setParameter(m_handle,m_boardAddr,ch,"ISET",val);
}

int dmtpc::control::CaenSupply::setCurrents(double val)
{
  return dmtpc::control::CaenUtility::setParameters(m_handle,m_boardAddr,"ISET",val);
}

int dmtpc::control::CaenSupply::setTripValue(unsigned ch, double val)
{
  return dmtpc::control::CaenUtility::setParameter(m_handle,m_boardAddr,ch,"TRIP",val);
}

int dmtpc::control::CaenSupply::setTripValues(double val)
{
  return dmtpc::control::CaenUtility::setParameters(m_handle,m_boardAddr,"TRIP",val);
}

int dmtpc::control::CaenSupply::setRampUpValues(double val)
{
  return dmtpc::control::CaenUtility::setParameters(m_handle,m_boardAddr,"RUP",val);
}

int dmtpc::control::CaenSupply::setRampDownValues(double val)
{
  return dmtpc::control::CaenUtility::setParameters(m_handle,m_boardAddr,"RDW",val);
}

int dmtpc::control::CaenSupply::setPowerDownValuesToRamp()
{
  return dmtpc::control::CaenUtility::setParameters(m_handle,m_boardAddr,"PDWN");
}

int dmtpc::control::CaenSupply::setImonRangeToHigh()
{
  return dmtpc::control::CaenUtility::setParameters(m_handle,m_boardAddr,"IMRANGE");
}

int dmtpc::control::CaenSupply::setChannelsOn()
{
  return dmtpc::control::CaenUtility::setParameters(m_handle,m_boardAddr,"ON");
}

int dmtpc::control::CaenSupply::setChannelOn(unsigned ch)
{
  return dmtpc::control::CaenUtility::setParameter(m_handle,m_boardAddr,ch,"ON");
}

int dmtpc::control::CaenSupply::setChannelsOff()
{
  return dmtpc::control::CaenUtility::setParameters(m_handle,m_boardAddr,"OFF");
}

int dmtpc::control::CaenSupply::setChannelOff(unsigned ch)
{
  return dmtpc::control::CaenUtility::setParameter(m_handle,m_boardAddr,ch,"OFF");
}

int dmtpc::control::CaenSupply::clearBoardAlarm()
{
  return dmtpc::control::CaenUtility::setParameters(m_handle,m_boardAddr,"BDCLR");
}
