#ifndef DMTPC_CONTROL_INFICON_CONTROLLER_HH
#define DMTPC_CONTROL_INFICON_CONTROLLER_HH

#include "SerialUtility.hh"
#include "Channel.hh"
#include "TMutex.h"

namespace dmtpc
{
  namespace control
  {
    class InficonController
    {
    public:

      InficonController(const char* namePort);

      // open serial device with settings appropriate for Inficon controller
      // returns handle to device if success, -1 otherwise
      SerialHandle initController();

      // close serial device
      // returns 0 if success, -1 otherwise
      int closeController();

      /**
	 Get a pressure reading via serial from an Inficon Vacuum Gauge Controller (VGC)
	 @param channel specifies which channel to read (1, 2, or 3)
	 @param baudRate   Serial comm. baud rate.  Defaults to 9600 which is the default baud rate for the VGC402.
	 @return the read value
      */
      double readChannel(unsigned channel);

      /** 
	  Reads the ACK bye from the infinicon controller (01 I believe) 
	  and then two more bytes (CR LR) in preparation for data transfer
	  @param rs232 Serial handle to device
      */
      int getACK(SerialHandle rs232);

    private:
      const char* portName; // the serial port e.g. /dev/ttyS0 
      SerialHandle m_handle;
    };
  }
}

#endif
