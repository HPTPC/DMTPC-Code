#include "SynAccessNetBooter.hh"
#include "TSocket.h"
#include "TSystem.h"
#include <iostream>

// curl specific
#include <curl/curl.h>
#include <cstdlib>
#include <cstring>
#include <sstream>

using std::cout;
using std::endl;


dmtpc::control::SynAccessNetBooter::SynAccessNetBooter(const char * address, 
						       TMutex * passedMutex) : 
  m_IPaddress(address), myMutex(passedMutex)
{
  for (unsigned i=0;i<m_NCHANNELS;i++) m_portStatus.push_back(false);
}

struct my_string
{
  char *buf; 
  unsigned pos; 
};

size_t my_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{

  my_string * str = (my_string*) userdata; 
  memcpy(str->buf + str->pos, ptr, size*nmemb); 
  str->pos += size*nmemb; 

  return size*nmemb; 
}

// Searches for <tp0> value on netBooter status.xml webpage
// TODO: extend this to other parameters on same xml page
int dmtpc::control::SynAccessNetBooter::getTempFromStatusXML(const char * username, 
							     const char * password) 
{
  CURL *curl = curl_easy_init();
  if(curl) 
    {
      CURLcode res;
      char url[40];
      sprintf(url,"http://%s/status.xml",m_IPaddress);
      curl_easy_setopt(curl, CURLOPT_URL, url);
      curl_easy_setopt(curl, CURLOPT_USERNAME, username);
      curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
      curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

      static my_string str; 
      if (str.buf==0) str.buf = (char*) calloc(10000,1); 
      str.pos = 0; 

      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_callback); 

      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str); 
      res = curl_easy_perform(curl);
      if (curl_easy_perform(curl)) {
	cout << "ERROR in curl perform of SynAccessNetBooter::getTempFromStatusXML()" << endl;
	return -1;
      }

      char * tp0 = strstr(str.buf, "<tp0>"); 
      char * tp0_close = strchr(tp0,'/'); 
      char temperature_str[3]; 
      strncpy(temperature_str,tp0 + sizeof("<tp0>")-1, tp0_close - tp0-sizeof("<tp0>")+1);
      temperature_str[3]=0; // null terminate 

      m_temp = atoi(temperature_str);
      //printf("%d\n", atoi(temperature_str)); 
      curl_easy_cleanup(curl);

      return m_temp;
    }
  else return -1;
}


int dmtpc::control::SynAccessNetBooter::connect(const char * username, 
						const char * password)
{

  // should we lock and unlock mutex?
  //bool lock = myMutex != 0; 
  //if (lock) myMutex->Lock(); 
  //if (lock) myMutex->UnLock(); 

  if (mySocket && mySocket->IsValid())
    {
      cout << "Socket is already open!" << endl;
      return 0;
    }
  else 
    {
      cout << "Opening SynAccess NetBooter socket ..." << endl;
      mySocket = new TSocket(m_IPaddress, 23);
      if (mySocket && !mySocket->IsValid())
	{
	  cout << "Socket not valid " << endl;
	  return 0;
	}
      else 
	{
	  cout << "Successfully opened socket" << endl;
	  gSystem->Sleep(100);
	  // check initial message
	  char rmsg[2048];
	  //cout << "Checking initial message"<<endl;
	  int nBytes = mySocket->RecvRaw( rmsg, 2048, kDontBlock);
	  gSystem->Sleep(250);
	  cout << rmsg << endl;
	  cout << "nBytes read = " << nBytes << endl;

	  char cmd[10];
	  sprintf(cmd,"\r");
	  mySocket->SendRaw(cmd,30);
	  gSystem->Sleep(500);

	  sprintf(cmd,"%s\r",username);
	  mySocket->SendRaw(cmd,30);
	  gSystem->Sleep(500);
	  
	  sprintf(cmd,"%s\r",password);
	  mySocket->SendRaw(cmd,30);
	  gSystem->Sleep(500);
	  return 1;
	}
    }
}


int dmtpc::control::SynAccessNetBooter::disconnect()
{
  if (checkSocket()) 
    {
      mySocket->Close();
      return 1;
    }
  return 0;
}


int dmtpc::control::SynAccessNetBooter::checkSocket()
{
  if (mySocket==NULL) 
    {
      cout << "NULL socket" << endl;
      return 0;
    }
  if (mySocket->IsValid()) return 1;
  else 
    {
      cout << "Socket not open!" << endl;
      return 0;
    }
}


int dmtpc::control::SynAccessNetBooter::getHandshake(const char* myResult)
{
  if (myResult==NULL) 
    {
      cout << "Command not executed!" << endl;
      return 0;
    }
  if (strstr(myResult,"$A0,")!=NULL) return 1;
  if (strstr(myResult,"$AF,")!=NULL)
    {
      cout << "Command failed!" << endl;
      return -1;
    }
  return 0;
}


int dmtpc::control::SynAccessNetBooter::updateValues()
{
  if (checkSocket())
    {
      mySocket->SendRaw("\r $A5 \r", 10); // TODO: try adding \n
      gSystem->Sleep(200);
      char rmsg[2048];
      int nBytes = mySocket->RecvRaw( rmsg, 2048, kDontBlock);
      gSystem->Sleep(100);
      //cout << "nBytes = " << nBytes << endl;
      if (nBytes > 50)
	{
	  char * result = strstr(rmsg,"$A0,");
	  if (result == NULL)
	    {
	      // handle error so we don't get seg fault
	      cout << "$A0, string not found in result" << std::endl;
	      return 0;
	    }
	  result += 4;
	  char * val = strtok(result,",");
	  unsigned i=0;
	  while (val != NULL && i<4)
	    {
	      if (i==0)
		  for (unsigned j=0;j<m_NCHANNELS;j++) 
		    m_portStatus.at(j) = (val[j]=='1'); 
	      else if (i==1) m_currentA = atof(val);
	      else if (i==2) m_currentB = atof(val);
	      else if (i==3) m_temp = atof(val);
	      val = strtok(NULL,",");
	      i++;
	    }
	  return 1;
	}
    }
  return 0;
}

int dmtpc::control::SynAccessNetBooter::getPortStatus(unsigned port)
{
  if (port==0 || port > m_NCHANNELS)
    {
      cout << "Port " << port << " out of range!" << endl;
      return -1;
    }
  return m_portStatus.at(port-1);
}


bool dmtpc::control::SynAccessNetBooter::switchGroupOn(unsigned group)
{
  if (checkSocket())
    {
      char cmd[24];
      sprintf(cmd, "gps %u 1\n", group);
      mySocket->SendRaw(cmd, 15);
      return 1;
    }
  
  return 0;
}

bool dmtpc::control::SynAccessNetBooter::switchGroupOff(unsigned group)
{
  if (checkSocket())
    {
      char cmd[24];
      sprintf(cmd, "\r gps %u 0 \r", group);
      mySocket->SendRaw(cmd, 10);
      return 1;
    }
  
  return 0;
}

int dmtpc::control::SynAccessNetBooter::switchPortOn(unsigned port)
{
  return switchPort(port,true);
}

int dmtpc::control::SynAccessNetBooter::switchPortOff(unsigned port)
{
  return switchPort(port,false);
}

int dmtpc::control::SynAccessNetBooter::switchPort(unsigned port, bool onoff)
{
  if (checkSocket())
    {
      char cmd[24];
      if (onoff) sprintf(cmd, "\r $A3 %u 1 \r", port);
      else sprintf(cmd, "\r $A3 %u 0 \r", port);
      mySocket->SendRaw(cmd, 20);
      gSystem->Sleep(200);
      char rmsg[2048];
      mySocket->RecvRaw( rmsg, 2048, kDontBlock);
      gSystem->Sleep(300);
      return getHandshake(rmsg);
    }
  
  return -1;
}

