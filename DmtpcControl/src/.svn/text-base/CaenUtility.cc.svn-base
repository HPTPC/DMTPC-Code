#include "CaenUtility.hh"
//#include "SerialUtility.hh"
#include "TSystem.h"
#include <iostream>
#include <unistd.h>

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <termios.h>

using std::cout;
using std::endl;

SerialHandle dmtpc::control::CaenUtility::openDevice(const char * port_name)
{
  /* Protection against empty port names */
  if (port_name == NULL)
  {
    cout << "openDevice: No port name specified!" << endl;
    return -1;
  }  

  /* Initialize options correctly */ 
  int baudRate = 9600; 
  int dataBits = 8;
  int stopBits = 1; 
  int xonxoff=1; 
  int rtscts = 0;
  char parity = 'n';

  SerialHandle caen = dmtpc::control::SerialUtility::Open(port_name);

  if (!caen) {
    fprintf(stderr,"Could not open CAEN device at %s", port_name); 
    fflush(stderr); 
    return -1;
  }

  unsigned ret = dmtpc::control::SerialUtility::Init(caen,baudRate,dataBits,parity,
				       stopBits,xonxoff,rtscts);

  if (!ret) {
    cout << "SerialUtillity::Init() failed" << endl; 
    ret+=closeDevice(caen);
    return -1;
  }

  dmtpc::control::SerialUtility::Flush(caen); 

  return caen;

      /*
      tcdrain(caen);
      tcflush(caen,TCIOFLUSH);
      tcflow(caen, TCOOFF);
      tcflow(caen, TCOON);
      tcsendbreak(caen, 0);
      */
}


int dmtpc::control::CaenUtility::closeDevice(SerialHandle handle)
{
  return dmtpc::control::SerialUtility::Close(handle);
}

int dmtpc::control::CaenUtility::rawCAENcmd(SerialHandle handle, 
					    const char * cmd, char ** result)
{
  if ((handle < 0) || !handle)
    {
      cout << "SerialHandle passed to rawCAENcmd not valid!" << endl;
      return -1;
    }
  unsigned l = strlen(cmd); 
  char * cmdbuf = (char*)malloc(l+2); 
  char *res = (char*) calloc(80,1); 

  // add CR to the end and make non-const copy
  memcpy(cmdbuf,cmd,l); 
  cmdbuf[l] = '\r'; 
  cmdbuf[l+1] = '\n'; 

  //printf("cmd: %s\n", cmd); 
  
  unsigned ret = dmtpc::control::SerialUtility::SerialWrite(handle,cmdbuf,l+2);
  free(cmdbuf);
  if (ret!=l+2)
  {	
   fprintf(stderr,"Only %d bytes written instead of %d\n", ret, (int) l+2); 
   fflush(stderr); 
   free(res);
   //closeDevice(handle);
   return -1;
  }
  ret = 0; 
  unsigned nloop=0;
  while(nloop<400 && (!ret || !strchr(res,'\n')))
  {
    nloop++;
    usleep(10000);
    ret += dmtpc::control::SerialUtility::SerialRead(handle,res+ret,79-ret);

    // // for debugging
    // if (ret) 
    // {
    //   printf("Read so far %d bytes, string is: %s\n", ret, res); 
    //   printf("Codes are "); 
    //   for (unsigned i = 0; i < ret; i++)
    // 			{
    //     printf("<%d>",int(res[i]));  
    // 			}
    //   printf("\n"); 
    //   fflush(stdout);
    // }
    
  }
  if (nloop==400) fprintf(stderr,"WARNING: nloop in rawCAENcmd reached %d\n", nloop);

  tcflush(handle,TCIFLUSH);

  if (result == NULL) free(res);
  else *result = res; 

  return ret; 
}


double dmtpc::control::CaenUtility::readParameter(SerialHandle handle,
						  unsigned brd, unsigned ch, 
						  std::string param)
{
  unsigned nch = dmtpc::control::CaenUtility::NCHANNELS;
  if ((handle < 0) || !handle)
    {
      cout << "SerialHandle passed to readParameter not valid!" << endl;
      return -1;
    }
  if (ch>nch)
    {
      std::cout << "Channel " << ch << " not supported!" << std::endl;
      return -1;
    }
  if (ch==nch)
    {
      cout << "Please use readParameters() if you want to read all " << nch << " channels" << endl;
      return -1;
    }
  else return dmtpc::control::CaenUtility::readParameters(handle,brd,param).at(ch);
}

std::vector<double> dmtpc::control::CaenUtility::readParameters(SerialHandle handle, 
								unsigned brd, 
								std::string param)
{
  unsigned nch = dmtpc::control::CaenUtility::NCHANNELS;

  std::vector<double> resultVector;
  if ((handle < 0) || !handle)
    {
      cout << "SerialHandle passed to readParameters not valid!" << endl;
      return resultVector;
    }
  if (brd>31)
    {
      std::cout << "Board " << brd << " not recognized!" << std::endl;
      return resultVector;
    }
  if (param!="VMON" && param!="IMON" && 
      param!="VSET" && param!="ISET" && 
      param!="RUP"  && param!="RDW"  &&
      param!="PDWN" && param!="TRIP" &&
      param!="STAT" && param!="BDALARM")
    {
      std::cout << "Parameter " << param << " not recognized!" << std::endl;
      return resultVector;
    }
  
  // prepare commands to be sent to caen
  char cmd[128]; 
  if (param=="BDALARM") // slight change in string formatting
    sprintf(cmd,"$BD:%u,CMD:MON,PAR:%s",brd,param.c_str()); 
  else sprintf(cmd,"$BD:%u,CMD:MON,CH:%u,PAR:%s",brd,nch,param.c_str()); 
  char * result; 
  //std::cout << "cmd = " << cmd << std::endl;
  if (rawCAENcmd(handle,cmd,&result))
    {
      //std::cout << "result = " << result << std::endl;
      char * val = strstr(result,"VAL:");

      // get the polarity
      // sleep a little bit so we get a good read
      //gSystem->Sleep(100);
      char * pol_result;
      sprintf(cmd,"$BD:%u,CMD:MON,CH:%u,PAR:POL",brd,nch);
      rawCAENcmd(handle,cmd,&pol_result);
      //std::cout << "pol_result = " << pol_result << std::endl;
      char * pol = strstr(pol_result,"VAL:");

      if (val == NULL || pol == NULL)
	{
	  //Handle error so we don't seg fault
	  std::cout << "VAL: string not found in result" << std::endl;
	  std::cout << "result = " << result << std::endl;
	  return resultVector; 
	}
      val+=4; //4 is strlen("VAL:")
      pol+=4;

      char *save_ptr1, *save_ptr2;
      char * val2 = strtok_r(val,";",&save_ptr1);
      char * pol2 = strtok_r(pol,";",&save_ptr2);
      while (val2 != NULL && pol2 != NULL)
	{
	  if (pol2[0]=='-') resultVector.push_back(-atof(val2));
	  else resultVector.push_back(atof(val2));
	  val2 = strtok_r(NULL,";",&save_ptr1);
	  pol2 = strtok_r(NULL,";",&save_ptr2);
	}
    }

  return resultVector;
}

int dmtpc::control::CaenUtility::setParameter(SerialHandle handle, 
					      unsigned brd, unsigned ch, 
					      std::string param, double val)
{
  if ((handle < 0) || !handle)
    {
      cout << "SerialHandle passed to setParameter not valid!" << endl;
      return -1;
    }
  if (brd>31)
    {
      std::cout << "Board " << brd << " not recognized!" << std::endl;
      return -1;
    }
  if (ch>dmtpc::control::CaenUtility::NCHANNELS)
    {
      std::cout << "Channel " << ch << " not supported!" << std::endl;
      return -1;
    }
  if (param!="VSET"  && param!="ISET" && 
      param!="RUP"   && param!="RDW"  &&
      param!="PDWN"  && param!="TRIP" && 
      param!="ON"    && param!="OFF"  &&
      param!="BDCLR" && param!="IMRANGE")
    {
      std::cout << "Parameter " << param << " not recognized!" << std::endl;
      return -1;
    }
  if (val==-999.9 && param!="ON" && param!="OFF" && 
      param!="PDWN" && param!="BDCLR" && param!="IMRANGE")
    {
      std::cout << "Parameter " << param << " requires value!" << std::endl;
      return -1;
    }
  
  // prepare commands to be sent to caen
  char cmd[128];
  if (param=="ON" || param=="OFF") // then use slightly different string syntax 
    sprintf(cmd,"$BD:%u,CMD:SET,CH:%u,PAR:%s",brd,ch,param.c_str()); 
  else if (param=="PDWN") // always ramp down, not kill
    sprintf(cmd,"$BD:%u,CMD:SET,CH:%u,PAR:%s,VAL:RAMP",brd,ch,param.c_str());  
  else if (param=="BDCLR") // clear any board alarms
    sprintf(cmd,"$BD:%u,CMD:SET,PAR:%s",brd,param.c_str()); 
  else if (param=="IMRANGE")
    sprintf(cmd,"$BD:%u,CMD:SET,CH:%u,PAR:%s,VAL:HIGH",brd,ch,param.c_str());  
  else sprintf(cmd,"$BD:%u,CMD:SET,CH:%u,PAR:%s,VAL:%06.1f",brd,ch,param.c_str(),val); 
  char * result; 
  rawCAENcmd(handle,cmd,&result); 
  //gSystem->Sleep(500);

  //std::cout << "cmd = " << cmd << std::endl;
  //std::cout << "result = " << result << std::endl;

  // now read 'result' for status of sent command
  char * stat = strstr(result,"CMD:OK"); 
  if (stat!=NULL) return 1; // successful command!
  else 
    {
      std::cout << "Caen set command " << cmd << " not executed properly! " << std::endl;
      return 0;
    }
}

int dmtpc::control::CaenUtility::setParameters(SerialHandle handle, unsigned brd,
					       std::string param, double val)
{
  unsigned nch = dmtpc::control::CaenUtility::NCHANNELS;
  if ((handle < 0) || !handle)
    {
      cout << "SerialHandle passed to setParameters not valid!" << endl;
      return -1;
    }
  return dmtpc::control::CaenUtility::setParameter(handle,brd,nch,param,val);
}
