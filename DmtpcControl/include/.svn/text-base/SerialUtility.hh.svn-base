#ifndef SERIAL_UTILITY_HH
#define SERIAL_UTILITY_HH

#ifndef __CINT__
#include <termios.h>
#else
struct termios;
#endif

#include <string> 
#include "TNamed.h"

using std::string;

#define SERIAL_EXIT_ERROR -1
#define OK true;
#define ERROR false;
#define OFF false;
#define ON  true;
#define MAXREADCHARS 1024
#define MAXSENDCHARS 512
#define TOGGLE(flag) flag=flag?OFF:ON

typedef int SerialHandle;

namespace dmtpc
{
  namespace control
  {
    class SerialUtility : public TNamed {

    public:

      // Ctors
      SerialUtility();

      static SerialHandle Open(string device); 
      static int Init(SerialHandle rs232, long baudRate=19200, int dataBits=8, 
		      char parity='n', int stopBits=1, int xonxoff=0, int rtscts=0);
      static void Abort(SerialHandle rs232);
      static int Send(SerialHandle rs232, char *sendstring, float pause);
      static int SerialWrite(SerialHandle rs232, char *sendstring, int nbytes);
      static string ReadPort(SerialHandle rs232, int readlength, bool trim=true);
      static char *ReadPort(SerialHandle rs232, char *readbuf, int readlength);
      static int SerialRead(SerialHandle rs232, char *readbuf, int readlength);
      static int Close(SerialHandle rs232);

      // functions to configure the serial port
      static int SetBaudRate(struct termios *rs232_attr, long baudrate);
      static int SetDataBits(struct termios *rs232_attr, int databits);
      static int SetParity  (struct termios *rs232_attr, char parity);
      static int SetStopBits(struct termios *rs232_attr, int stopbits);
      static int SetFlowCtl (struct termios *rs232_attr, int xonxoff, int rtscts);

      //void configPort();

      static void Wait(float waittime);
      //static int CalcCRC(char *cmd, int length);
      static void Flush(SerialHandle rs232);

    private:
      //static float ck_atof(char const *str);
      //static int hex2dec(char const *);

    };
  }
}

#endif

