#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

#include "MySQLStore.hh"
#include "InficonController.hh"
#include "SynAccessNetBooter.hh"
#include "Channel.hh"
#include "CaenSupply.hh"

#include <vector>
#include <string>
#include <sstream>

// temporary
#include <time.h>

// CONFIG //
const bool debug = false;
const char* dbfilename = "/etc/dbaccess.txt";
const char* portInficon = "/dev/ttyS0";
const char* ipSynAccess = "198.125.161.189";
const char* username = "dmatter";
const char* password = "seedark";
// order of pressure gauges on Inficon controller
const std::string pressureNames[3] = {"chamber_pressure_cdg", 
				      "turbo_pressure_cdg",
				      "chamber_pressure_bpg"};

// Caen init parameters
static const unsigned noCaen = 2;
static const char* portCaen[noCaen] = {"/dev/ttyACM0", "/dev/ttyUSB0"};
static const unsigned caenBoard[noCaen] = {0, 2};
static const unsigned caenRampSpeed[noCaen] = {100, 20}; // [V/s]
static const unsigned caenSetCurrent[noCaen] = {200, 100}; // [uA]
static const unsigned caenTripTime[noCaen] = {2, 2}; // [s]



template <typename T>
string ToString(T val)
{
  std::stringstream stream;
  stream << val;
  return stream.str();
}

// calculate percent change between old value (val1) and new value (val2)
// and compare to specified cut value
bool compareValues(double val1, double val2, double cut=0.005)
{
  if (val1==val2) return false;
  else if (val1==0 || val2==0) return true;
  return (fabs((val2-val1)/val1) > cut);
}


int main(int argc, char *argv[])
{
  // initalize persistency tool
  dmtpc::control::MySQLStore* mysql = new dmtpc::control::MySQLStore(dbfilename);

  // initialize the pressure controller
  dmtpc::control::InficonController* myInficon = new dmtpc::control::InficonController(portInficon);
  if (!myInficon->initController())
    cerr << "ERROR: Unable to intialize InficonController!" << endl;

  // initialize the master Mutex's 
  //TMutex * mut_db     = new TMutex();
  TMutex * mut_usb    = new TMutex();
  TMutex * mut_synacc = new TMutex();

  // initialize the SynAccess netBooter
  dmtpc::control::SynAccessNetBooter* mySynAccess = new dmtpc::control::SynAccessNetBooter(ipSynAccess, mut_synacc);
  //mySynAccess->connect(username,password);

  // for retrieving database entries later
  vector<dmtpc::control::Channel*> myVoltageChannels, myCurrentChannels, myStatusChannels,
    mySetVoltageChannels;

  // initialize the Caen channels
  std::vector <dmtpc::control::CaenSupply*> myCaens;
  static unsigned noCaenChannels[noCaen];
  for (unsigned int i=0; i<noCaen; i++)
    {
      myCaens.push_back(new dmtpc::control::CaenSupply(portCaen[i],caenBoard[i],mut_usb));
      if (!myCaens.at(i)->initSupply())
	cerr << "ERROR: Unable to initialize CaenSupply " << i << "!" << endl;
      
      noCaenChannels[i] = myCaens.at(i)->getNumberChannels();
      myCaens.at(i)->clearBoardAlarm();
      myCaens.at(i)->setPowerDownValuesToRamp();
      myCaens.at(i)->setRampUpValues(caenRampSpeed[i]);
      myCaens.at(i)->setRampDownValues(caenRampSpeed[i]);
      if (caenSetCurrent[i] >= 300) myCaens.at(i)->setImonRangeToHigh();
      myCaens.at(i)->setCurrents(caenSetCurrent[i]);
      myCaens.at(i)->setTripValues(caenTripTime[i]);

      for (unsigned j=0; j<noCaenChannels[i]; j++)
	{
	  std::string caenChName = "caen_brd"+ToString(i)+"_ch"+ToString(j);
	  myVoltageChannels.push_back(new dmtpc::control::Channel(caenChName+"_hv"));
	  myCurrentChannels.push_back(new dmtpc::control::Channel(caenChName+"_i"));
	  myStatusChannels.push_back(new dmtpc::control::Channel(caenChName+"_status"));
	  mySetVoltageChannels.push_back(new dmtpc::control::Channel(caenChName+"_set_hv"));
	}
    }

  // initialize the pressure and temp channels
  vector <dmtpc::control::Channel*> myTempChannels;
  myTempChannels.push_back(new dmtpc::control::Channel("temp0"));

  // initialize HV status channel
  dmtpc::control::Channel* myHVStatusChannel = new dmtpc::control::Channel("hv_status");

  vector <dmtpc::control::Channel*> myPressureChannels;
  for (unsigned i=0; i<3; i++) 
    myPressureChannels.push_back(new dmtpc::control::Channel(pressureNames[i]));


  // synchronize Store
  // e.g. channel->synchronizeDB(mut_db);
  
  int nSleep = 0; // read pressure every nSleep (in ms)
  int nSkip  = 1; // read voltage every nSleep*nSkip
  if (argc>1) 
    {
      nSleep=atoi( argv[1] );
      cout << argc << "   " << (argv[1]) << endl;
    }
  cout << "Sleep set to " << nSleep << " ms" << endl;


  int nRead = 0;
  while (1)
    {
       // Retrieve pressure values from gauges and store in db
      for (std::vector<int>::size_type i=0; i != myPressureChannels.size(); i++)
	{
	  double newPressure = (myInficon) ? myInficon->readChannel(i+1) : 0.0;
	  if (newPressure > 0 && compareValues(myPressureChannels[i]->getValue(), newPressure))
	    {
	      myPressureChannels[i]->value = newPressure;
	      myPressureChannels[i]->save(mysql);
	    }
	}

      //mySynAccess->updateValues();
      if (nRead++ % (nSkip*5) == 0)
	{
	  // Retrieve temperature from SynAccess and store in db
	  int synAccessTemp = (mySynAccess) ? mySynAccess->getTempFromStatusXML(username,password) : 0;
	  //cout << "temp0 = " << synAccessTemp << endl;
	  if (synAccessTemp > 0 && compareValues(myTempChannels[0]->getValue(), synAccessTemp))
	    {
	      myTempChannels[0]->value = synAccessTemp;
	      myTempChannels[0]->save(mysql);
	    }
	}

      if (nRead % nSkip == 0) 
	{
	  for (unsigned int i=0; i<noCaen; i++)
	    {
	      std::vector<double> caenVoltages, caenCurrents, caenSetVoltages, caenStatusValues;
	      caenVoltages     = myCaens.at(i)->readVoltages();
	      caenCurrents     = myCaens.at(i)->readCurrents();
	      caenSetVoltages  = myCaens.at(i)->readSetVoltages();
	      caenStatusValues = myCaens.at(i)->readStatusValues();

	      unsigned int caenBoardAlarmValue = myCaens.at(i)->readBoardAlarmValue(true);

	      bool hvON = (myHVStatusChannel->readValue(mysql) > 0) ? true : false;
	      //cout << "HV STATUS: " << hvON << endl;

	      if (!hvON) myCaens.at(i)->setChannelsOff();

	      if ((caenVoltages.size()     != noCaenChannels[i]) || 
		  (caenCurrents.size()     != noCaenChannels[i]) ||
		  (caenStatusValues.size() != noCaenChannels[i]) ||
		  (caenSetVoltages.size()  != noCaenChannels[i]))
		{
		  cout << "ERROR: Number of CAEN supply channels != returned vector" << endl;
		  continue;
		}
	      if (caenVoltages.size()==0 || caenCurrents.size()==0 || 
		  caenStatusValues.size()==0 || caenSetVoltages.size()==0)
		{
		  cout << "ERROR: size of at least one returned vector is 0" << endl;
		  continue;
		}

	      for (unsigned int j=0; j<noCaenChannels[i]; j++)
		//for (std::vector<int>::size_type j=0; j!=noCaenChannels[i]; j++)
		{
		  unsigned int index = (i > 0) ? noCaenChannels[i-1] : 0;
		  // check if database values should be updated wrt values from Caen
		  if (compareValues(myVoltageChannels[index+j]->readValue(mysql), caenVoltages.at(j), 0.0005))
		    {
		      myVoltageChannels[index+j]->value = caenVoltages.at(j);
		      myVoltageChannels[index+j]->save(mysql);
		    }
		  if ((compareValues(myCurrentChannels[index+j]->readValue(mysql), caenCurrents.at(j), 0.0005)) ||
		      (index==1 && (caenCurrents.at(j) > 0.0))) // special case for anode HV supply: record all values above 0 
		    {
		      myCurrentChannels[index+j]->value = caenCurrents.at(j);
		      myCurrentChannels[index+j]->save(mysql);
		    }
		  if (myStatusChannels[index+j]->readValue(mysql)!=fabs(caenStatusValues.at(j)))
		    {
		      myStatusChannels[index+j]->value = fabs(caenStatusValues.at(j));
		      myStatusChannels[index+j]->save(mysql);
		    }
	      
		  // check if Caen set values should be updated wrt values from database
		  double vset_db = mySetVoltageChannels[index+j]->readValue(mysql);
		  if ((fabs(vset_db) >= 0.0) && (fabs(caenSetVoltages.at(j))!=fabs(vset_db)))
		    {
		      mySetVoltageChannels[index+j]->value = vset_db;
		      myCaens.at(i)->setVoltage(j,vset_db);
		    } 

		  // do not proceed if these alarms exist on Caen board
		  if ((caenBoardAlarmValue & (int)exp2(j)) || // j-th channel in alarm status
		      (caenBoardAlarmValue >= 16)) // power fail, over power, internal clock fail
		    continue;

		  // Turn on channel only if no status errors and channel is OFF
		  if (hvON && (fabs(vset_db) > 0.) && caenStatusValues.at(j)==0) 
		    myCaens.at(i)->setChannelOn(j);
		  else if (vset_db==0.0) myCaens.at(i)->setChannelOff(j);
		}
	      if (!hvON) myCaens.at(i)->setChannelsOff(); // double check
	    }
	}
      
      gSystem->Sleep(nSleep);
    }

  if (myInficon) myInficon->closeController();
  //mySynAccess->disconnect();  

  for (unsigned int i=0; i<noCaen; i++)
    {
      if (myCaens.at(i)) myCaens.at(i)->closeSupply();
      delete myCaens.at(i);
    }

  delete mySynAccess;
  delete myInficon;
  delete mysql;

  return 0;
}
