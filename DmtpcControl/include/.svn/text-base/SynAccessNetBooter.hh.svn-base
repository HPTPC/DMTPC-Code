#ifndef DMTPC_CONTROL_SYNACCESS_NETBOOTER_HH
#define DMTPC_CONTROL_SYNACCESS_NETBOOTER_HH

/** Abstract implementation of SynAccess netBooter NP-1601DT **/ 

#include "TMutex.h"
#include "TSocket.h"
#include <string>
#include <vector>

namespace dmtpc
{
  namespace control
  {
    class SynAccessNetBooter
    {
    public:
      SynAccessNetBooter(const char * address, TMutex * passedMutex);
      ~SynAccessNetBooter(void) { if (mySocket) delete mySocket; }

      int connect(const char * username, const char * password);

      int disconnect();

      int checkSocket();

      int getTempFromStatusXML(const char * username, const char * password); 

      int updateValues();

      int getPortStatus(unsigned port);

      bool switchGroupOn(unsigned group);

      bool switchGroupOff(unsigned group);

      int switchPort(unsigned port, bool onoff);

      int switchPortOn(unsigned port);

      int switchPortOff(unsigned port);

      int getHandshake(const char* myResult);

      // inline methods

      double getCurrentA() { return m_currentA; }

      double getCurrentB() { return m_currentB; }

      int getTemp() { return m_temp; }

      int getNumberOfChannels() { return m_NCHANNELS; }

      std::vector<bool> getStatusAllPorts() { return m_portStatus; }

    private:
      const char * m_IPaddress;
      TMutex * myMutex;
      TSocket * mySocket;

      static const unsigned m_NCHANNELS=16;

      double m_currentA;
      double m_currentB;
      int m_temp;
      std::vector<bool> m_portStatus;

    };
  }
}

#endif

