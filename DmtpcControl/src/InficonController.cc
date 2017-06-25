#include "InficonController.hh"
//#include "SerialUtility.hh"

//#include <assert.h>
//#include <iostream>
//#include <string>
//#include <termios.h>
//#include <stdlib.h>
//#include <fcntl.h>

#include "TRegexp.h"

using std::cout;
using std::endl;

dmtpc::control::InficonController::InficonController(const char* namePort) :
  portName(namePort) 
{ 
  m_handle = 0;
}

SerialHandle dmtpc::control::InficonController::initController()
{
  int dataBits  = 8;
  int baudRate  = 9600;
  char parity   = 'n';
  int stopBits  = 1;
  int xonxoff   = 0;
  int rtscts    = 0;

  if (portName == NULL)
    {
      cout << "ERROR in InficonController::initController(): portName not initialized" << endl;
      return -1;
    }
  if (m_handle)
    {
      cout << "ERROR in InficonController::initController(): SerialHandle already exists!" << endl;
      return -1;
    }
  else m_handle = SerialUtility::Open(portName);

  if (!m_handle) {
    std::cout << "ERROR in InficonController::initController(): could not open device"  << endl;
    return -1;
  }
  else {
    SerialUtility::Init(m_handle, baudRate, dataBits, parity, stopBits, xonxoff, rtscts);  
    SerialUtility::Flush(m_handle);
  }
  return (int)m_handle;
}


int dmtpc::control::InficonController::closeController()
{
  if (m_handle)
    {
      SerialUtility::Close(m_handle);
      return 0;
    }
  else if (!m_handle) std::cout << "No handle for closeController()!" << std::endl;
  return -1;
}


double dmtpc::control::InficonController::readChannel(unsigned channel)
{
/** 
   Get a pressure reading via serial from an Inficon Vacuum 
   Gauge Controller (VGC).

   Works for models VGC402 or VGC403 (and maybe VGC401, too).
  
   <p>Communication with the VGC40X goes like this:
   <pre>
   Commands are a 3 character ASCII mnemonic, optionally followed by
   associated parameters, then followed by a carriage return or a 
   carriage return and line feed
     e.g. 

      <MNE>[params]<CR>[<LF>]
       where <MNE> stands for mnemonic.
  
       The controller will respond with an acknowledgement:
       e.g.
          <ACK><CR><LF>  or <NAK><CR><LF>
    
     Sending (computer to controller)
      S: <MNE>[params]<CR>[<LF>]
      R: <ACK><CR><LF>
       
    
     Receiving 
       First, send a command to the controller.
       Then look for <ACK> or <NAK>
       If <ACK>, the controller has placed information for retrieval
       in its output buffer, ready for you to retrieve.
       Then ask to retrieve data by sending <ENQ> ("enquire")
       Finally controller will send contents of output buffer to computer.

       e.g.
       S: PR1<CR>
       R: <ACK><CR><LF>
       S: <ENQ>
       R: 0, 7.1000E+02

     <ENQ> = 0x05
     <ACK> = 0x06
     <NAK> = 0x15

     For pressure readings (PR1\r, PR2\r, PR3\r), the return value 
     will look like this:
     A,<PM>B.BBBBE<PM>BB<CR><LF>
     where <PM> = plus or minus
           <CR> = carriage return (0x0D)
           <LF> = line feed (0x0A)
     and
                          A = 0  -- Measurement data ok
                              1  -- Underrange
                              2  -- Overrange
                              3  -- Sensor error
                              4  -- Sensor switched off
                              5  -- No sensor
                              6  -- Identification error
                              7  -- BPG/HPG error
    
     and
        <PM>B.BBBBE<PM>BB  = the pressure reading in the current 
                             unit of measurement

   </pre>

 */

  string cmdStr;
  char ENQ = 0x05; 
  switch(channel)
    {
    case 1: cmdStr = "PR1\r"; break;
    case 2: cmdStr = "PR2\r"; break;
    case 3: cmdStr = "PR3\r"; break;
    default: cout << "ERROR in InficonController::readChannel: Channel " << channel << " not supported!" << endl; return -1;
    }

  char cmd[1024];
  strcpy(cmd, cmdStr.c_str());
  int bytes_sent;
  bytes_sent = SerialUtility::SerialWrite(m_handle, cmd, 4);
  //cout << "bytes_sent = " << bytes_sent << endl;
  //tcdrain(m_handle);
  usleep(100000); // this is very necessary!

  getACK(m_handle);

  // send ENQ
  bytes_sent = SerialUtility::SerialWrite(m_handle, &ENQ, 1);

  // Read response
  char c[1024];
  int ncount = 0;
  string sensorString;
  int totaltries=0;
  sensorString.clear();
  while(1) {
    int nread = SerialUtility::SerialRead(m_handle, &c[ncount], 1);
    ++totaltries;
    if(totaltries>1e6){
      break; // give up if it fails 100x or more
    }
    if (nread<1) continue;
    sensorString += c[ncount];
    if (++ncount==13) break;  // the response from the VGC402 is 13 chars long
  }
  //  cout << "sensorString.size(), val = [" << sensorString.size() << "], [" 
  //       << sensorString << "]" << endl;

  // should probably verify that A=0
  string statusStr = sensorString.substr(0, sensorString.find_first_of(","));
  //  cout << "sensorString.find_first_of(',') = " << sensorString.find_first_of(",") << endl;
  //  cout << "statusStr = " << statusStr << endl;
  int status = atoi(statusStr.c_str());
  enum { MSRMT_OK, UNDERRANGE, OVERRANGE, 
   SENSOR_ERROR, SENSOR_OFF, NO_SENSOR, 
   ID_ERROR, BPG_HPG_ERROR };
  if (status == UNDERRANGE)
    return -10*UNDERRANGE;
  else if (status == OVERRANGE)
    return -10*OVERRANGE;
  else if (status == SENSOR_ERROR)
    return -10*SENSOR_ERROR;
  else if (status == SENSOR_OFF)
    return -10*SENSOR_OFF;
  else if (status == NO_SENSOR)
    return -10*NO_SENSOR;
  else if (status == ID_ERROR)
    return -10*ID_ERROR;
  else if (status == BPG_HPG_ERROR)
    return -10*BPG_HPG_ERROR;

  // then extract the entire substring following the comma
  TString valueStr = sensorString.substr(sensorString.find_first_of(",")+1);
  //  cout << "valueStr = " << valueStr << endl;

  // need to see if the value that we read matches the pattern of what
  // we expect to read; if it doesn't, it's a bad read, and we need to throw
  // it away.  Use regular expressions.
  TRegexp expected_inficon_value_regexp_pattern("[0-9]\\.[0-9][0-9][0-9][0-9]E[+-][0-9][0-9]");

  // and return the value (<PM>B.BBBBE<PM>BB) converted to a double
  //  cout << "valueStr.Data() = " << valueStr.Data() << endl;
  //  cout << "valueStr.Contains(expected_inficon_value_regexp_pattern)="
  //       << valueStr.Contains(expected_inficon_value_regexp_pattern)
  //       << endl;
  //  cout << "valueStr(expected_inficon_value_regexp_pattern)="
  //       << valueStr(expected_inficon_value_regexp_pattern)
  //       << endl;
  if(totaltries<1e6 && valueStr.Contains(expected_inficon_value_regexp_pattern))
    return atof(valueStr(expected_inficon_value_regexp_pattern).Data());
  else return -1;
    // don't change the current value, so that the value won't change in 
    // the database; for now this is probably the best option ...
}



int dmtpc::control::InficonController::getACK(SerialHandle rs232)
{
  // reads the <ACK> byte from the inficon controller
  // and then reads two more bytes (<CR><LF>)
  // in preparation for data transfer

  char ACK = 0x06;  // "acknowledge" sent back by the controller
  //char NAK = 0x15;  // "negative acknowledge" sent by controller

  char readbuf[256];
  int  readlength = 1;
  int  nloop = 0;
  int  bytes_read = 0;
  while (1) {
    nloop++;
    //printf("readport\n");
    //SerialUtility::ReadPort(rs232, readbuf, readlength);
    bytes_read = SerialUtility::SerialRead(rs232, readbuf, readlength);
    //if (bytes_read > 0) {
    //  printf("bytes_read [%d] ", bytes_read);
    //  printf("%03d, [0x%x]\n", nloop, (unsigned int)readbuf[0]);
    //}
    if ( (bytes_read && readbuf[0] == ACK) || nloop>100) {
      //printf("readbuf done\n");
      // read the <CR> and <LF> and return
      SerialUtility::ReadPort(rs232, readbuf, 2);
      break;
    }
  }

  return 0;
}

