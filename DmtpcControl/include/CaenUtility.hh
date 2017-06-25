#ifndef DMTPC_CONTROL_CAEN_UTILITY_HH
#define DMTPC_CONTROL_CAEN_UTILITY_HH

/** Utility class for CAEN power supplies **/ 

//#include "TMutex.h"

#include "SerialUtility.hh"
#include <string>
#include <vector>

//typedef int SerialHandle;

namespace dmtpc
{
  namespace control
  {
    namespace CaenUtility
    {
      static const unsigned NCHANNELS   = 4;
      static const double   ERROR_VALUE = -9999.99;

      SerialHandle openDevice(const char * port_name);
      int closeDevice(SerialHandle handle);

      int rawCAENcmd(SerialHandle handle, const char * cmd, char ** result);

      std::vector<double> readParameters(SerialHandle handle, unsigned brd,
					 std::string param);

      // this is a special case of readParameters()
      double readParameter(SerialHandle handle, unsigned brd, unsigned ch, 
			   std::string param);
      
      int setParameter(SerialHandle handle, unsigned brd, unsigned ch, 
		       std::string param, double val=-999.9);

      // this is a special case of setParameter()
      int setParameters(SerialHandle handle, unsigned brd, 
			std::string param, double val=-999.9);

    }
  }
}

#endif

