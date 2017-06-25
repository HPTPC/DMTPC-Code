#include "Scope_CAEN.hh" 
#include "CAENDigitizer.h"
#include "ScopeConfig.hh"
#include "M3GlobalData.hh"
#include <iostream>
#include <sstream>
#include "TString.h"
#include "Waveform.hh"
#include "TTimeStamp.h"
#include <time.h>
#include <ctime>

using std::cout;
using std::endl;
using std::deque;

//char * testBuffer = NULL; //bad
//CAEN_DGTZ_UINT16_EVENT_t    *Event16=NULL;
//uint32_t AllocatedSize;

dmtpc::daq::CAENDigitizer::CAENDigitizer(int h, int b)
{
  handle = h; 
  board = b; 
}

struct BufferHeader
{
  size_t actual_size; //number of bytes actually read by ScopeCAEN
};

int dmtpc::daq::CAENDigitizer::boardSetup(int h)
{

  cout << "boardSetup handle = " << h << endl;
  int ret = 0;
  cout << "boardSetup ret = " << ret << endl;
  ret = CAEN_DGTZ_Reset(h);  //resets board to default mode 
  sleep(1);
  cout << "boardSetup ret = " << ret << endl;
  ret += CAEN_DGTZ_ClearData(h);  //clears data stored in digitizer buffers
  sleep(1);
  cout << "boardSetup ret = " << ret << endl;
  ret += CAEN_DGTZ_Calibrate(h); //NB: should be done after ADC chip temps stabilize
  cout << "boardSetup ret = " << ret << endl;

 for(int i=0;i<8;i++)
 { 
   uint32_t temp;
   CAEN_DGTZ_ReadTemperature(handle, i, &temp);
   cout << "temp ch " << i << " = " << temp << endl;
 }

  if(ret !=0) {cout << "Board setup error" << endl;}
  return ret;
}

int dmtpc::daq::CAENDigitizer::enableExtendedTimeStamp(int h)
{
  
  /* change [22:21] to 10 */
  uint32_t myData;
  CAEN_DGTZ_ReadRegister(h, 0x811C, &myData); //front panel I/O Control
  //cout << "0x811C before writing 10 to [22:21] " << myData << endl; //returns 0
  uint32_t myDataOld = myData;
  myData = myData | (1 << 22); //4194304 = 2^22, sets [22] to 1
  //cout << "I/O Control register [22]->1: " << myData << endl; //changed myData
  //if((myData >> 22) & 1)cout <<  "myData bit 22 set to 1!" << endl; //checks bit 22 value
  myData = myData & (~(1<<21)); //sets [21] to 0
  //cout << "I/O Control register [21]->0: " << myData << endl; //changed myData
  //if(((myData >> 21) & 1) == 0)cout << "myData bit 21 set to 0!" << endl; //checks bit 21 value   
  int ret =CAEN_DGTZ_WriteRegister(h,0x811C,myData);

  return ret;
}

uint32_t dmtpc::daq::CAENDigitizer::getChannelMask(const std::vector<unsigned> * ch)
{

  uint32_t myChMask=0;
  for(int i = 0; i<(ch->size()); i++)
  {
    unsigned int num = ch->at(i);
    myChMask += pow(2,num);
    //or myChMask += 2<<num; if there are problems
  }
  
    return myChMask; //2^ch, Ex. ch 5 chMask 32
}

std::string dmtpc::daq::CAENDigitizer::getChannelName(const std::vector<unsigned> * ch, int n, std::string r)
{
      //TODO: add readout channel name to config  
      std::stringstream namestringstream;
      std::string namestring;
      namestringstream << r << "_ch" << ch->at(n);
      namestring = namestringstream.str();
      
        return namestring;
}

uint32_t dmtpc::daq::CAENDigitizer::setWindowSize(int h, unsigned int us)
{
  uint32_t size = 500*us; //us*rate = samples //TODO: add sample rate as config
    CAEN_DGTZ_SetRecordLength(h, size); //TODO: check if 2ms crash is CAEN bug 
    uint32_t mySizeCheck;
    CAEN_DGTZ_GetRecordLength(h, &mySizeCheck);
      if(mySizeCheck != size){ cout << "Error: Acquisition window size changed from " << 
        size << " to " << mySizeCheck << endl;} 
    
      return mySizeCheck;
}

uint32_t dmtpc::daq::CAENDigitizer::setPreTriggerSize(int h, unsigned int percent, unsigned int us)
{

    uint32_t post = percent; //percent of window to be post-trigger, 100 beginning, 0 end
    CAEN_DGTZ_SetPostTriggerSize(h, post);
    
    double myPreTrigger = (1-((double)post / 100.))*(double)setWindowSize(h,us); //counts
    uint32_t preTrigger = (uint32_t)myPreTrigger;
    double preTriggerTime = myPreTrigger/500; //(#samples/rate=time)
    cout << "Acquiring " << us << " us (" << 500*(double)us << " sample) window with " << preTriggerTime << " us ( " << 
      preTrigger << " samples) pretrigger" << endl;
      return preTrigger; //value set in ScopeInfo
}

int dmtpc::daq::CAENDigitizer::setChannelTrigger(int h, unsigned ch, double vpp, double vmin, double trglvl)
{

    //TODO: implement trigger in volts or ADC counts
    //TODO: convert to mV?
    /*
    double dcOffset = (double)dc + 1; //[0,-1,-2] pos/center/neg
    double triggerCounts = trglvl*conversion; //volts->ADC counts
    //CAEN_DGTZ_PulsePolarity_t polarity;
    //CAEN_DGTZ_GetChannelPulsePolarity(h, ch, &polarity);
    //if(polarity = CAEN_DGTZ_PulsePolarityNegative){triggerCounts = triggerCounts*(-1);}
    double adc_per_dac = 16384./65536. ; // adc/dac
    double dcOffset_adc = dcOffset*adc_per_dac; //[16384, 8192, 0] pos/center/neg
    cout << "DC Offset (cts)= " << adc_per_dac << " times " << dcOffset << " = " << dcOffset_adc << endl;
    double zeroLevel = (dcOffset_adc - 16384)*(-1); //(-1)*[0, -8192, -16384]
    double myTval = zeroLevel + triggerCounts; //V
    uint32_t Tval = (uint32_t)myTval; //rounds down
    cout << "Ch" << ch << " trigger threshold set to: " << Tval << endl;
    */

    double conversion = 16383.; //counts per volt
    //double zeroLevel_adc = dc*conversion*(-1.);
    //cout << "zero = " << zeroLevel_adc << " (ADC)" << endl;
    double trigLevel_adc = ((trglvl - vmin)/vpp) * conversion;
    cout << "trig = " << trglvl << " (V) and " << trigLevel_adc << " (ADC)" << endl;
    //double myTval = zeroLevel_adc + trigLevel_adc;
    //cout << "zero + trig = " << myTval << " (ADC)" << endl;
    if(trigLevel_adc > conversion || trigLevel_adc < 0. ){cout << "TRIGGER LEVEL OUT OF RANGE!" << endl;}
    uint32_t Tval = (uint32_t)trigLevel_adc;
    cout << "trig set to " << Tval << " ADC" << endl;
    
    int ret = CAEN_DGTZ_SetChannelTriggerThreshold(h,ch,Tval);

    return ret;
}

uint32_t dmtpc::daq::CAENDigitizer::getDC_dac(double dc, double vpp, double vmax)
{
  //if(dc > 0. || dc < -2.) { cout << "DC offset is not safe" << endl; }  
  double dac = 65535.;
  double mydcOffset_dac = ((vmax - dc)/vpp)*dac; // dac*(vmax/vpp) - dac*(dc/vpp);  
  if(mydcOffset_dac > 65535. || mydcOffset_dac < 0.){cout << "DC OFFSET OUT OF RANGE!" << endl;}
  uint32_t dcOffset = (uint32_t)mydcOffset_dac;
  //cout << "vmax will be " << (vmax - dc) << " V (" << mydcOffset_dac << " DAC)" << endl;
  /*
  double dcOffset_dac = (double) dc; //[0, -1, -2] neg/center/pos
  double v_per_dac = 2./65536.;
  double dcOffset_v = dcOffset_dac*v_per_dac;
  double vmin = dcOffset_v - v;
  */
    return dcOffset;
}

int dmtpc::daq::CAENDigitizer::init(const dmtpc::core::ScopeConfig * sc)
{
  //TODO: Add ret = CAEN_DGTZ_Function; if (ret !=0)printf("something broke\n"); in necessary places
  // only for heartbeat
  gM3Data.mySqlOnlM3->command(Form("insert into avr_occupancy_scope%d (value, timeStamp) VALUES (%f,NOW()) ", sc->scope_id, -1.0));

  scopeCfg = sc;
  
  boardSetup(handle);
 
  CAEN_DGTZ_BoardInfo_t myboardinfo;
  
  CAEN_DGTZ_GetInfo(handle, &myboardinfo);
  
  cout << "Model Name: " << myboardinfo.ModelName << endl;
  cout << "Serial Number: " << myboardinfo.SerialNumber << endl;
  
  /* Populate ScopeInfo */

  info.scope_number = sc->scope_id;
  
  cout << "Scope Number: " << sc->scope_id << endl;
  
  //cout << "test: " << cfg->getBool("scope_enabled") << endl;
  
  // retrieve board and channel configuration from ScopeConfig
  std::vector<unsigned> channelsEnabled;
  for (unsigned int ch=0; ch<sc->scopeChannelConfigs.size(); ch++)
    {
      const dmtpc::core::ScopeChannelConfig scc = sc->scopeChannelConfigs.at(ch);
      channelsEnabled.push_back(scc.channel_id);
    }
  const std::vector<unsigned> * pChannelsEnabled = &channelsEnabled;
  cout << "channelsEnabled size: " << channelsEnabled.size() << endl;

  uint32_t chMask = getChannelMask(pChannelsEnabled);

  for(unsigned i = 0; i < channelsEnabled.size(); i++)
  {
    unsigned myChannel = channelsEnabled.at(i); //chEnabled[i];
    cout << "channel: " << myChannel << " chMask: " << chMask << endl;
  
    //unsigned int Nchannels = myboardinfo.Channels; 
      
    /* Populate ScopeChannelInfo */
    dmtpc::core::ScopeChannelInfo myScopeChannelInfo;
    const dmtpc::core::ScopeChannelConfig * scc = &(sc->scopeChannelConfigs.at(i)); 
  
    //std::string readout = "readout";  //less than 10 chars 
    //strncpy(myScopeChannelInfo.name, getChannelName(pChannelsEnabled,i,readout).c_str(), DMTPC_SCOPE_CHANNEL_NAME_LENGTH);
    strncpy(myScopeChannelInfo.name, scc->name.Data(), DMTPC_SCOPE_CHANNEL_NAME_LENGTH);

    // TODO MAL: myScopeChannelInfo.name = scc->name.Data();
    cout << "channel name: " << myScopeChannelInfo.name << endl;
    cout << "channel name from config: " << scc->name << endl;

     //0x7FFF centered (default), 0x0000 positive, 0xFFFF negative
    double myOffsetValue = (double)scc->dc_offset/1000.; 
    myScopeChannelInfo.vmin = (double)scc->vmin/1000.;
    myScopeChannelInfo.vmax = (double)scc->vmax/1000.;
    double vpp = myScopeChannelInfo.vmax - myScopeChannelInfo.vmin;
    if (vpp != 2.0) cout << "CAENDigitizer::init(): Warning: Vpp for channel " << i << "!= 2.0" << endl;
    
    uint32_t dc_offset_dac = getDC_dac(myOffsetValue, vpp, myScopeChannelInfo.vmax);
      cout << "vmin: " << myScopeChannelInfo.vmin << " vmax: " << myScopeChannelInfo.vmax << endl;
      cout << "dc offset: " << myOffsetValue << " (V) and " << dc_offset_dac << " (DAC) for vpp: " << vpp << endl;
    
    /* checking channel DAC */
        uint32_t dacDatapre; //before setting dc offset
        uint32_t dacRegister = 0x1098 | (myChannel << 8 );
        CAEN_DGTZ_ReadRegister(handle, dacRegister, &dacDatapre); //dac gain, 0x1n98
        //cout  << "DC Offset readback from register before setting DC offset: " << dacDatapre << endl;

    /* set channel DC offset */
  
    sleep(1);  //give DAC output a chance to stabilize

    // MAL: I am here now
    if(int err= CAEN_DGTZ_SetChannelDCOffset(handle, myChannel, dc_offset_dac)) //dc_offset_dac 
    { fprintf(stderr, "Error in SetChannelDCOffset: %d\n",err);}
    
    //TODO: Adjust getPhysical() y-axis to reflect DC offset
       
    /* checking channel DAC */
        uint32_t dacData;
        CAEN_DGTZ_ReadRegister(handle, dacRegister, &dacData); //dac gain, 0x1n98
        //cout  << "DC Offset readback from register: " << dacData << endl;
    

    myScopeChannelInfo.nbits = myboardinfo.ADC_NBits; //14-bit ADC 
    myScopeChannelInfo.nbytes_raw = 2;  
    myScopeChannelInfo.sample_rate = 500000; // kHz; memory is 5.12 MS/s, sample rate is 500 MS/s
    //double window = 10;
    myScopeChannelInfo.nsamples = setWindowSize(handle,sc->record_length);
    myScopeChannelInfo.nsamples_pretrigger = setPreTriggerSize(handle,sc->post_trigger,sc->record_length);
    myScopeChannelInfo.channel = myChannel; //channelsEnabled->at(j); //channel
    myScopeChannelInfo.scope = info.scope_number;
    
    cout << "CAENDigitizer::init() Adding a ScopeChannelInfo to vector of length " << info.channel_info.size() << endl;
    info.channel_info.push_back(myScopeChannelInfo);
    cout << "CAENDigitizer::init() ScopeChannelInfo for channel " << info.channel_info[i].channel << " is populated." << endl;
          
    /* set channels to be masked during acquisition */
    CAEN_DGTZ_SetChannelEnableMask(handle, chMask); //enable mask, bit n to channel n
   
    /*  set pulse polarity */
    const int caen_pulse_polarity = scc->pulse_polarity_falling;
    CAEN_DGTZ_SetChannelPulsePolarity(handle,myChannel,(CAEN_DGTZ_PulsePolarity_t)caen_pulse_polarity);

    /*  set trigger polarity */
    const int caen_trigger_polarity = scc->trigger_polarity_falling;
    CAEN_DGTZ_SetTriggerPolarity(handle,myChannel,(CAEN_DGTZ_TriggerPolarity_t)caen_trigger_polarity);
    /*
    CAEN_DGTZ_TriggerPolarity_t pol;
    CAEN_DGTZ_GetTriggerPolarity(handle,myChannel, &pol);
    //cout << "trigger polarity: " << pol << endl;
      //options are TriggerOnFallingEdge, TriggerOnRisingEdge
      */
    
    /* set channel trigger threshold */
    double v_trig = (double)scc->trigger_threshold/1000.; //mV
    setChannelTrigger(handle, myChannel, vpp, myScopeChannelInfo.vmin, v_trig);

    /* set channel self trigger mode */
    CAEN_DGTZ_SetChannelSelfTrigger(handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY,chMask); //chMask
     //chMask of first ch in pair
     //p27 "only applies to channels with relevant bit = 1 in mask"
         
    cout << "info.scope_number = " << info.scope_number << endl;     
  }
  
  /*** end of things which are channel/channel mask-dependent ***/
  
   /* set external trigger */
  CAEN_DGTZ_SetExtTriggerInputMode(handle,CAEN_DGTZ_TRGMODE_DISABLED);
   //options are DISABLED, EXTOUT_ONLY, ACQ_ONLY, and ACQ_AND_EXTOUT
   //TODO: make this configurable for different boards
      
   CAEN_DGTZ_SetIOLevel(handle,CAEN_DGTZ_IOLevel_NIM); //TTL or NIM
  
  /* Set max events for buffer to read out */
  uint32_t maxEvents= sc->num_events_BLT;
  //TODO: figure out how many is actual maximum number
  CAEN_DGTZ_SetMaxNumEventsBLT(handle, maxEvents);
  
  /* setting acquisition mode */
  CAEN_DGTZ_SetAcquisitionMode(handle, CAEN_DGTZ_SW_CONTROLLED);
  // can be CAEN_DGTZ_SW_CONTROLLED, CAEN_DGTZ_S_IN_CONTROLLED, CAEN_DGTZ_FIRST_TRG_CONTROLLED

  /* set software trigger */
  //if(cfg->getBool("software_trigger"))
  //{ 
  //CAEN_DGTZ_SetSWTriggerMode(handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY); //enables software trigger for acquisition
  CAEN_DGTZ_SetSWTriggerMode(handle,CAEN_DGTZ_TRGMODE_DISABLED); //disables software trigger
  //}

  /* set up synchronization of acquisition across boards */
  /*
    CAEN_DGTZ_SetRunSynchronizationMode(handle,CAEN_DGTZ_RUN_SYNC_TrgOutTrgInDaisyChain); 
      //other options are Disabled, TrgOutSinDaisyChain, SinFanout, GpioGpioDaisyChain   
  */

  /* Enable 16-bit TriggerTimeTag extension in header word 2 [23:8] */
  
  enableExtendedTimeStamp(handle);
     
  /* it doesn't seem like there is a direct way to get the readout buffer size, so we cheat. */ 
 
  char * fake_buf = 0; 
  uint32_t fake_buf_size;
  if (int err = CAEN_DGTZ_MallocReadoutBuffer(handle, &fake_buf, &fake_buf_size)) 
  {
    fprintf(stderr, "Error in CAEN_DGTZ_MallocReadoutBuffer: %d\n",err);
  } 
  if (int err = CAEN_DGTZ_FreeReadoutBuffer(&fake_buf)) 
  {
    fprintf(stderr, "Error in CAEN_DGTZ_FreeReadoutBuffer: %d\n",err);
  } 
  buf_size = (size_t) fake_buf_size + sizeof(BufferHeader); 
  cout << "buf_size: " << buf_size << endl;

 // CAEN_DGTZ_AllocateEvent(handle, (void**)&Event16);
 // CAEN_DGTZ_MallocReadoutBuffer(handle, &testBuffer,&AllocatedSize);

  return 0;
}


int dmtpc::daq::CAENDigitizer::start()
{
  
  /* start acquisition */
  struct timespec tp;
  clock_gettime(CLOCK_REALTIME, &tp);
  startTimeStamp = TTimeStamp((timespec_t)tp);
  cout << "CPU time digitizer start: " << startTimeStamp << endl;
  int ret =  CAEN_DGTZ_SWStartAcquisition(handle);
  cout << "CAENDigitizer::start()" << endl;
  
  return ret;
  /* software starts acquisition for now, but for multiple board systems, 
   * this does't start them both at the same time,
   * so acquisition needs signal (S-IN or GPI) to start and TRG-IN, TRG-OUT daisy chain */
}


int dmtpc::daq::CAENDigitizer::stop()
{

  /* stop acquisition */
  int ret = CAEN_DGTZ_SWStopAcquisition(handle);
  cout << "CAENDigitizer::stop()" << endl;
  struct timespec tp;
  clock_gettime(CLOCK_REALTIME, &tp);
  endTimeStamp = TTimeStamp((timespec_t)tp);
  cout << "CPU time digitizer stop: " << endTimeStamp << endl;
  //if(Event16)CAEN_DGTZ_FreeEvent(handle, (void**)&Event16);
  //CAEN_DGTZ_FreeReadoutBuffer(&testBuffer);

  return ret;
}


size_t dmtpc::daq::CAENDigitizer::readOut(void * buf)
{
  //usleep(990000);
  //cout << "Reading out ..." << endl;

  uint32_t size; 
  CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,
                   (char*) buf + sizeof(BufferHeader), &size); 
  ((BufferHeader*)buf)->actual_size = size;
  
  return (size_t) size; 
}


TTimeStamp dmtpc::daq::CAENDigitizer::getTraceTime(char * eventPointer, CAEN_DGTZ_EventInfo_t evInfo)const 
{
    uint32_t * word2 = ((uint32_t *)eventPointer)+1; //16-bit extension of time stamp
    uint32_t separator = 16776960;  //binary 00000000111111111111111100000000
    uint32_t myTTTExt = (*word2 & separator)>>8; //removes parts before and after bits [23:8]
    //[31:27][26][25:24][23:8][7:0] and TTExt is [23:8]
    uint32_t timeStamp = evInfo.TriggerTimeTag;
    uint64_t oneBillion = 1000000000;
    uint64_t bigTimeTag = 4294967296*myTTTExt + timeStamp; 
    uint64_t timestamp_nsecs = (bigTimeTag % oneBillion) * 8;
    uint64_t timestamp_secs = (bigTimeTag / oneBillion) * 8;
    uint32_t maxValue = 4294967295;
    uint32_t addSeconds = (timestamp_nsecs / oneBillion);
    timestamp_secs += addSeconds; //adds extra seconds from nsecs conversion
    timestamp_nsecs = timestamp_nsecs % oneBillion;
      if (timestamp_nsecs > maxValue) cout << "DANGER! for nsecs" << endl;
      if (timestamp_secs > maxValue) cout << "DANGER! for secs" << endl; 
    TTimeStamp traceTimeStamp((time_t)timestamp_secs,(Int_t)timestamp_nsecs);
    traceTimeStamp.Add(startTimeStamp); //adds CPU time

    return traceTimeStamp;
}

static uint32_t events_counter = 0; 
static uint32_t counter = 0; 
size_t dmtpc::daq::CAENDigitizer::makeWaveforms(std::deque<dmtpc::core::Waveform*> * dest, const void * buf) const
{
  /* For each trace in buf, create a waveform and add it to the dest deq */
  uint32_t size = ((BufferHeader*)buf)->actual_size;
  uint32_t nEvents; //number of events in readout buffer
  CAEN_DGTZ_GetNumEvents(handle,(char*)buf + sizeof(BufferHeader),size,&nEvents);                                                 
  
  events_counter += nEvents; 
  counter++; 
  if (counter % 1000 == 0 )
  {
    double asdf_avg = double(events_counter)/ counter; 
    printf("Average occupancy (after %d) = %f\n", counter, asdf_avg);
    gM3Data.mySqlOnlM3->command(Form("insert into avr_occupancy_scope%d (value, timeStamp) VALUES (%f,NOW()) ", scopeCfg->scope_id, asdf_avg));
    //    fflush(stdout);
  }
     
  for (unsigned int n=0; n<nEvents; n++)
  {
    CAEN_DGTZ_EventInfo_t eventInfo;
    static CAEN_DGTZ_UINT16_EVENT_t * Evt =NULL; //points to decoded event
    static char * evtptr = NULL;

    CAEN_DGTZ_GetEventInfo(handle, (char*) buf + sizeof(BufferHeader), size, n, &eventInfo, &evtptr);
    CAEN_DGTZ_DecodeEvent(handle, evtptr, (void**)&Evt);  

    TTimeStamp scopeTimeStamp = getTraceTime(evtptr, eventInfo); 
    uint32_t evCount = eventInfo.EventCounter;
    /** if (counter % 1000 == 0 )
    {
      cout << "event mask: " << eventInfo.ChannelMask << " pattern: " << eventInfo.Pattern <<
         " board id: " << eventInfo.BoardId << " event size: " << eventInfo.EventSize << endl; 
    }  
    **/

    unsigned int scope = info.scope_number;
    //cout << "mw scope = " << scope << endl;
    for(int y=0; y < info.channel_info.size(); y++)
    { 
      unsigned int yChannel = info.channel_info[y].channel;
      unsigned int channel = yChannel;  //check y and i above are what I think they are 
    //TODO: check if chMask in GetEventInfo is needed for anything
       
      dest->push_back(new dmtpc::core::Waveform(TString::Format("sc%d_ch%d_ev%u",scope,channel,evCount),
          TString::Format("Scope %d, Channel %d, Event %u",scope,channel,evCount),(void*)Evt->DataChannel[channel], 
					      &info.channel_info[y],scopeTimeStamp.GetSec(),scopeTimeStamp.GetNanoSec()));  

    }
  }

  return nEvents;  //should return number made probably 
}


size_t dmtpc::daq::CAENDigitizer::dataAvailable()
{
  
  /* Check if data is available */
  //maybe base on trigger condition?
  
  return 1;
}

size_t dmtpc::daq::CAENDigitizer::waitForData(unsigned timeout_in_ms)
{
  
  /* Wait for data, use interrupts (see pg 14 of manual)*/
  //SetInterrupt may need to go in DAQConfig.cc
  // CAEN_DGTZ_IACKCycle(handle, id);
  //CAEN_DGTZ_RearmInterrupt(handle);

  //cout << "waiting for data..." << endl;
   
  //CAEN_DGTZ_SendSWtrigger(handle);
  return 1;    
}





