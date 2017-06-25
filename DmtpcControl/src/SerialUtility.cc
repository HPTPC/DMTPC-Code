#include "SerialUtility.hh"

#include <assert.h>
#include <iostream>
//#include <string>
#include <termios.h>
#include <stdlib.h>
#include <fcntl.h>

using std::cout;
using std::endl;
using std::string;
using std::hex;
using std::dec;

dmtpc::control::SerialUtility::SerialUtility() {}

int dmtpc::control::SerialUtility::Init(SerialHandle rs232, long baudRate, int dataBits, 
			char parity, int stopBits, int xonxoff, int rtscts) {

  struct termios rs232_attr;
  rs232_attr.c_iflag = 0;
  rs232_attr.c_oflag = 0;
  rs232_attr.c_lflag = 0;
  rs232_attr.c_cflag = CREAD | CLOCAL;

  if ( !SetBaudRate(&rs232_attr, baudRate) ) {
    cout << "Init:  SetBaudRate() fail" << endl;
    Abort(rs232);
  }
  if ( !SetDataBits(&rs232_attr, dataBits) ) {
    cout << "Init:  SetDataBits() fail" << endl;
    Abort(rs232);
  }
  if ( !SetParity(&rs232_attr, parity) ) {
    cout << "Init:  SetParity() fail" << endl;
    Abort(rs232);
  }
  if ( !SetStopBits(&rs232_attr, stopBits ) ) {
    cout << "Init:  SetStopBits() fail" << endl;
    Abort(rs232);
  }
  if ( !SetFlowCtl(&rs232_attr, xonxoff, rtscts) ) {
    cout << "Init:  SetFlowCtl() fail" << endl;
    Abort(rs232);
  }

  // assign the flags to the port
  if (tcsetattr(rs232, TCSANOW, &rs232_attr) == -1) {
    cout << "error setting tcsetattr" << endl;
    Abort(rs232);
  }

  return OK;
}

SerialHandle dmtpc::control::SerialUtility::Open(string device) 
{
  SerialHandle rs232;
  rs232 = open(device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

  // I find that if you omit the O_NDELAY flag, then the serial communication
  // often hangs (but not in a repeatable way) during Read() operations.
  // See note below as to why that is.  
  // So, the moral is ASSERT O_NDELAY!!!!!
  //
  // From: Serial Programming Guide for POSIX Operating Systems
  //
  // http://www.easysw.com/~mike/index.php?serial/serial.html+printable
  // 
  // O_NDELAY
  // "The O_NDELAY flag tells UNIX that this program doesn't care what
  // state the DCD signal line is in - whether the other end of the
  // port is up and running. If you do not specify this flag, your
  // process will be put to sleep until the DCD signal line is the
  // space voltage."
  //
  // O_NOCTTY
  // The O_NOCTTY flag tells UNIX that this program doesn't want to be
  // the "controlling terminal" for that port. If you don't specify
  // this then any input (such as keyboard abort signals and so forth)
  // will affect your process. Programs like getty(1M/8) use this
  // feature when starting the login process, but normally a user
  // program does not want this behavior.

  if (rs232 == -1) {
    cout << "error opening "<< device << endl;
    cout << "rs232 = " << rs232 << endl;
    cout << "need to reimplement Abort()" << endl;
    //Abort(rs232);
  }
  tcflush(rs232,TCIOFLUSH);
  return rs232;
}

/**************************************************************
 * read data from a device via the serial port
 **************************************************************/

char* dmtpc::control::SerialUtility::ReadPort(SerialHandle rs232, char *readbuf, int readlength)
{
  if (read(rs232, readbuf, readlength) == -1) {
    //cout << "ReadPort fail" << endl;
    //Abort(rs232);
  }
  readbuf[readlength]='\0';
  return readbuf;
}

string dmtpc::control::SerialUtility::ReadPort(SerialHandle rs232, int readlength, bool trim) 
{
  // readlength  --  the number of bytes that the users asks to be read
  // trim        --  true=trim trailing whitespace from string
  //                 false=do not trim trailing whitespace

  char readbuf[MAXREADCHARS];

  Bool_t fail=false;

  int nBytesRead;  // the number of bytes actually read
  nBytesRead = read(rs232, readbuf, readlength);
  if (nBytesRead == -1) {
    cout << "ReadPort2 fail -1" << endl;
    //Abort(rs232);
    fail=true;
  }
  if (nBytesRead >=0)
    readbuf[nBytesRead]='\0';

  //for (int ii=0; ii<readlength; ii++) {
  //  if (readbuf[ii] == NULL)
  //    break;
  //  printf("%c ", readbuf[ii]);
  //}

  string readstring(readbuf);
  
  // trim trailing whitespace?
  if (trim) 
    readstring.erase(readstring.find_last_not_of(" \t\n\r")+1);

  if(fail) cout << "readstring=" << readstring << endl;

  return readstring;
}

int dmtpc::control::SerialUtility::SerialRead(SerialHandle rs232, char *readbuf, int readlength) {
  return read(rs232, readbuf, readlength);
}

int 
dmtpc::control::SerialUtility::SerialWrite(SerialHandle rs232, char *sendstring, int nbytes) {
  return write(rs232, sendstring, nbytes);
}

/**************************************************************
 * Send data from the computer to a serial device
 * sendstring  --  the ascii string to send
 * pause  --  how long to wait (seconds) after sending string
 **************************************************************/
int dmtpc::control::SerialUtility::Send(SerialHandle rs232, char *sendstring, float pause) 
{
  //cout << "SerialSend()" << endl;
  //cout << "sendstring = [" << sendstring << "]" << endl;
  int hexMode   = false;
  int writeMode = true;
  //int charcnt; 

  cout << "sending " << sendstring << endl;

  char *cp = sendstring; // character pointer
  //char *startptr;
  //char h[3];             // hex code buffer
  char c;                // character to be written to device
  //char s[MAXSENDCHARS];

  //h[2] = '\0';  // terminate the hex code buffer

  do {
    writeMode = true;
    //cout << *cp << endl;
    if (*cp == '\\') {
      switch ( *(cp+1) ) {
      case '\\': // back slash
	c = '\\'; cp++; break;
      case '-':  // minus sign (dash)
	c = '-'; cp++; break;
      case 'n':  // new line
	c = '\n'; cp++; break;
      case 'r':  // carriage return
	c = '\r'; cp++; break;
      case '0':  // null
	c = '\0'; cp++; break;
      case 'p':  // pause
	Wait(pause); cp++; writeMode = false; break;
      //case 'w':  // wait
      //	charcnt = 0;
      //	startptr = cp+2;
      //	while (*(cp+charcnt) != '\0' && *(cp+charcnt) != ' ') {
      //	  ++charcnt;
      //	}
      //	strncpy(s, startptr, charcnt-2);
      //	Wait(ck_atof(s));
      //	cp += charcnt;
      //	break;
      //case 'h':  // hex
      //	TOGGLE(hexMode); cp++; writeMode = false; break;
      default:
	c = *cp;
      }
    } else {
      if (hexMode) {
	//if (*cp == ' ')
	//  ++cp;
	//h[0] =* (cp++);
	//h[1] =* (cp);
	//c = (char)hex2dec(h);
      } else {
	c = *cp;
      }
    }
    if (writeMode == true)
      cout << "writing: [" << &c << "]" << endl;
    write(rs232,&c,1);
    /*
      if (write(rs232, &c, 1) != 1) {
	cout << "error: write" << endl;
	Abort(rs232);
      }
    */
    
  } while (*(++cp) != '\0');

  tcdrain(rs232);
  //Wait(pause);

  return OK;
}

void dmtpc::control::SerialUtility::Abort(SerialHandle rs232) 
{
  cout << "Abort():  rs232 = " << rs232 << endl;
  if (rs232 != -1) 
    {
      Close(rs232);
      exit(SERIAL_EXIT_ERROR);
    }
}

int dmtpc::control::SerialUtility::Close(SerialHandle rs232) {
  cout << "closing serial device with handle " << rs232 << endl;
  if (rs232 > 0) 
    if (close(rs232) == -1) {
      cout << "error closing serial I/O device" << endl;
      exit(SERIAL_EXIT_ERROR);
    }
  return OK;
}

int  dmtpc::control::SerialUtility::SetBaudRate(struct termios *rs232_attr, long baudrate) 
{
  switch (baudrate) { 
  case 50:
    rs232_attr->c_cflag |= B50; break;
  case 75:
    rs232_attr->c_cflag |= B75; break;
  case 110:
    rs232_attr->c_cflag |= B110; break;
  case 134:
    rs232_attr->c_cflag |= B134; break;
  case 150:
    rs232_attr->c_cflag |= B150; break;
  case 200:
    rs232_attr->c_cflag |= B200; break;
  case 300:
    rs232_attr->c_cflag |= B300; break;
  case 600:
    rs232_attr->c_cflag |= B600; break;
  case 1200:
    rs232_attr->c_cflag |= B1200; break;
  case 2400:
    rs232_attr->c_cflag |= B2400; break;
  case 4800:
    rs232_attr->c_cflag |= B4800; break;
  case 9600:
    rs232_attr->c_cflag |= B9600; break;
  case 19200:
    rs232_attr->c_cflag |= B19200; break;
  case 38400:
    rs232_attr->c_cflag |= B38400; break;
  case 57600:
    rs232_attr->c_cflag |= B57600; break;
  case 115200:
    rs232_attr->c_cflag |= B115200; break;
  case 230400:
    rs232_attr->c_cflag |= B230400; break;
  default:
    cout << "error: unsupported baud rate: " << baudrate << endl;
    return ERROR;
  }
  
  return OK;
}


int dmtpc::control::SerialUtility::SetDataBits (struct termios *rs232_attr, int databits)
{
   switch (databits) {   
    case 5:
      rs232_attr->c_cflag |= CS5; break;
    case 6:  
      rs232_attr->c_cflag |= CS6; break;
    case 7:
      rs232_attr->c_cflag |= CS7; break;
    case 8:
      rs232_attr->c_cflag |= CS8; break;
    default:
      cout << "error: unsupported character size: " << databits << endl;
      return ERROR;
   }
   return OK;
}

int dmtpc::control::SerialUtility::SetParity (struct termios *rs232_attr, char parity)
{
   switch (toupper(parity)) {   
     case 'N': 
       break;
     case 'E':  
       rs232_attr->c_cflag |= PARENB; break;
     case 'O': 
       rs232_attr->c_cflag |= PARODD; break;
     default:
       cout << "error: unsupported parity mode: " << parity << endl;
       return ERROR;
   }
   return OK;
}

/**************************************************************
 * Set the number of stop bits
 **************************************************************/
int dmtpc::control::SerialUtility::SetStopBits(struct termios *rs232_attr, int stopbits)
{
   switch (stopbits) {   
    case 1:
      break;
    case 2:  
      rs232_attr->c_cflag |= CSTOPB; break;
    default:
      cout << "error: unsupported number of stop bits: " << stopbits << endl;
      return ERROR;
   }
   return OK;
}

/**************************************************************
 * Set hardware and software flow control
 **************************************************************/
int dmtpc::control::SerialUtility::SetFlowCtl(struct termios *rs232_attr, int xonxoff, int rtscts)
{
   if (xonxoff) {
      rs232_attr->c_iflag |= IXON;
      rs232_attr->c_iflag |= IXOFF;
   }
   
   if (rtscts)
     rs232_attr->c_cflag |= CRTSCTS;
   
   return OK;
}

/**************************************************************
 * Allow for pauses in serial comm (e.g. between send and receive)
 * waittime  --   time to wait in seconds
 **************************************************************/
void dmtpc::control::SerialUtility::Wait(float waittime)
{
  usleep((unsigned int)(waittime*1E6));
}

void dmtpc::control::SerialUtility::Flush(SerialHandle rs232) 
{
  tcflush(rs232, TCIFLUSH);
}



//
//  DEPRECATED! Does not work correctly!
//  See MaxCamMFC instead
//
//
///*****************************************************************************
// * COMPUTE CYCLIC REDUNDANCY CHECK VALUE FOR THE SIERRA MASS FLOW CONTROLLER
// * inputs:
// *   *char = address of string to be converted to a float
// * outputs:
// ****************************************************************************/
//int dmtpc::control::SerialUtility::CalcCRC(char *cmd, int length) {
//  int crc = 0xffff;
//
//  //int jj = 0;
//  //while (! (cmd[jj]==0) ) {
//  for (int jj=0; jj<length; jj++) {
//    for (int ii=0; ii<8; ii++) {
//      if (crc & 0x8000)
//        crc = (crc<<1)^0x1021; // prime number                                               
//      else
//        crc = crc<<1;
//    }
//    //jj++;
//  }
//
//  if ( (crc & 0xff00) == 0x0d00) crc += 0x0100; // increment if byte is carriage return      
//  if ( (crc & 0x00ff) == 0x000d) crc += 0x0001; // increment if byte is carriage return      
//  if ( (crc & 0xff00) == 0x0000) crc += 0x0100; // no zero bytes allowed                     
//  if ( (crc & 0x00ff) == 0x0000) crc += 0x0001; // no zero bytes allowed                     
//  return crc;
//}


/*****************************************************************************
 * CONVERT STRING to FLOAT 
 * inputs:
 *   *str = address of string to be converted to a float
 * outputs:
 *   if valid number -> return float
 *   if invalid num  -> print error message and exit with error
 ****************************************************************************/
//float dmtpc::control::SerialUtility::ck_atof (char const *str)
//{
//  char const *cp;
//   
//  for (cp = str; *cp; cp++)
//    if (!isdigit (*cp) && *cp != '.' ) {
//      cout << "ck_atof(): " << str << " should be a number" << endl;
//      exit(SERIAL_EXIT_ERROR);
//    }
//
//  return atof(str);
//}

//int dmtpc::control::SerialUtility::hex2dec(char const *s) 
//{
//  int n, i;
//  int dec = 0;
//  i = strlen (s) - 1;
//  while (i >= 0) {
//    if (isdigit (*s))
//      n = *s - 48;
//    else 
//      n = toupper (*s) - 55;
//    if ((n < 0) || (n > 15))
//      return ERROR;
//    dec += (int)pow (16, i--) * n;
//    s++;
//  }
//  return dec;
//}


//// private methods
//
//void 
//dmtpc::control::SerialUtility::Flush() {
//  tcflush(fd, TCIFLUSH);
//}

