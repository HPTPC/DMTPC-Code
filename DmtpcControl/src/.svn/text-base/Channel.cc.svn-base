#include "Channel.hh" 
//#include "ChannelConfig.hh" 
#include "Store.hh" 
#include "TMutex.h" 
#include <vector>

/** This file contains all the hardware implementation for the Channel classes. The interface is purposefully opaque.
 *
 *  Please only edit stuff here if you know what you're doing. 
 **/ 

enum channel_type
{
  type_analog,
  type_digital,
  type_hv
};

enum direction
{
  dir_read,
  dir_write,
}; 

enum device_type 
{
   dev_comedi, 
   dev_caenhv, 
   dev_labjack 
}; 



#ifdef _ENABLE_COMEDI_ 
  #include <comedi.h> 
#endif


#ifdef _ENABLE_LABJACK_ 

  #ifndef MAX_LABJACKS
  #define MAX_LABJACKS 16 
  #endif


  static HANDLE labjacks[MAX_LABJACKS] = {0};
  static TMutex labjack_locks[MAX_LABJACKS]; 

#endif



/*
class ComediDIOChannel : public dmtpc::control::DIOChannel
{
 public: 
   ComediDIOChannel(comedi_t * device, int subdev, int channel, int dir) 
     : dev(device), subdev(subdev),channel(channel), dir(dir); 
   {

   }
   virtual int write(bool) 
   {


   }
   virtual bool state() 
   {

   }
   virtual int fire() 
   {

   }

   private:
   int subdev;
   int channel; 
   int dir; 
   comedi_t * dev; 
}; 
*/ 




#ifdef _ENABLE_LABJACK_ 


uint16_t u3_configure_channels(HANDLE handle, int nchan, const unsigned * chans, bool analog)
{

  //set up a configdio to get current state

  uint8_t buf[12]; 
  buf[1] = 0xf8; 
  buf[2] = 0x03; 
  buf[3] = 0x0b; 
  buf[6] = 0; 
  buf[7] = 0; 
  buf[8] = 0; 
  buf[9] = 0; 
  buf[10] = 0; 
  buf[11] = 0; 

  buf[0] = u3checksum8(buf+1,5); 
  uint16_t c16 = u3checksum16(buf+6,6); 
  buf[4] = uint8_t(c16 && 0xff); 
  buf[5] = uint8_t((c16/256) & 0xff); 

  if (LJUSB_Write(handle,buf,12)!=12) 
  {
    fprintf(stderr,"write error in ljusb_write\n"); 
    return 1; 
  }

  if (LJUSB_Read(handle,buf,12)!=12) 
  {
    fprintf(stderr,"read error in ljusb_read\n"); 
    return 1; 
  }

  uint16_t c16 = u3checksum16(buf+6,6); 
  if (buf[0] != u3checksum8(buf+1,5) ||  buf[4] != uint8_t(c16 && 0xff)  || buf[5] = uint8_t((c16/256) & 0xff)) 
  {

    fprintf(stderr,"bad checksum\n"); 
    return 1; 
  }

  //change the channels we want to change
  for (unsigned i = 0; i < nchan; chans++)
  {
    unsigned chan = chans[i]; 

    if (chan >= 16)  continue; 
    unsigned j = chan > 8 ? 1: 0; 

    if (analog)  
      buf[10+j] |= (1 << chan); 
    else
      buf[10+j] &= ~(1 << chan);  
  }

  buf[0] = u3checksum8(buf+1,5); 
  uint16_t c16 = u3checksum16(buf+6,6); 
  buf[4] = uint8_t(c16 && 0xff); 
  buf[5] = uint8_t((c16/256) & 0xff); 


  if (LJUSB_Write(handle,buf,12)!=12) 
  {
    fprintf(stderr,"write error in ljusb_write\n"); 
    return 1; 
  }

  return 0; 
}


/** U3 digital out channel. Use Feedback + PortStateWrite **/ 
class U3DOChannel : public dmtpc::control::DOChannel
{

  public:
    U3DOChannel(HANDLE handle, unsigned nchan, const unsigned * chans, long fire_length)  
      : hand(handle), nchan(nchan), chans(chans), fire_length(fire_length) 
    {
      u3_configure_channels(handle,nchan,chans,false); 

      fiomask = 0; 
      eiomask = 0; 
      ciomask = 0; 

      for (unsigned i = 0; i < nchan; i++)
      {
        if (chans[i] < 8 ) 
        {
          fiomask |= (1 << chans[i]); 
        }
        else if (chans[i] < 16 ) 
        {
          eiomask |= (1 << (chans[i] -8)); 
        }
        else 
        {
          ciomask |= (1 << (chans[i] -16)); 
        }
      }

    }

    virtual unsigned nchannels() const { return nchan; } 
    virtual int write(bool *vals )  
    {
      char buf[14]; 


      buf[1] = 0xf8; 
      buf[2] = 4; 
      buf[3] = 0; 

      buf[6] = 0; 
      buf[7] = 27; 
      buf[8] = fiomask; 
      buf[9] = eiomask; 
      buf[10] = ciomask; 

      uint8_t fio_val = 0; 
      uint8_t eio_val = 0; 
      uint8_t cio_val = 0; 
      for (unsigned i = 0; i < nchan; i++)
      {
        if (!vals[i]) continue; 

        if (chans[i] < 8 ) 
        {
          fio_val |= (1 << chans[i]); 
        }
        else if (chans[i] < 16 ) 
        {
          eio_val |= (1 << (chans[i] -8)); 
        }
        else 
        {
          cio_val |= (1 << (chans[i] -16)); 
        }
      }

      buf[11] = fio_val; 
      buf[12] = eio_val; 
      buf[13] = cio_val; 
      buf[14] = 0; 

      buf[0] = u3checksum8(buf+1,5); 
      uint16_t c16 = u3checksum16(buf+6,8); 
      buf[4] = uint8_t(c16 && 0xff); 
      buf[5] = uint8_t((c16/256) & 0xff); 

      //todo: error checking
      LJUSB_Write(hand, buf,14); 
      LJUSB_Read(hand, buf,12); 

    }
    virtual int setFireLength(long usecs) {fire_length = usecs; } 

    virtual int fire() 
    {


    }

  private:
    HANDLE hand; 
    unsigned nchan;
    unsigned * chans; 
    uint8_t fiomask;
    uint8_t eiomask;
    uint8_t ciomask;


    long fire_length ; 


}; 

#endif 


int dmtpc::control::Channel::save(dmtpc::control::Store * store)
{
  return store->save(getName(), getValue()); 
}

double dmtpc::control::Channel::readValue(dmtpc::control::Store * store)
{
  return store->getValue(getName());
}


dmtpc::control::DIChannel * dmtpc::control::factory::makeDI(const ChannelConfig * cfg)
{
  return cfg->getEnum("channel_type") == type_digital ? 
                                                      (dmtpc::control::DIChannel*) dmtpc::control::factory::make(cfg) :
                                                      0; 
}

dmtpc::control::DOChannel * dmtpc::control::factory::makeDO(const ChannelConfig * cfg)
{
  return cfg->getEnum("channel_type") == type_digital ? 
                                                      (dmtpc::control::DOChannel*) dmtpc::control::factory::make(cfg) :
                                                      0; 
}

dmtpc::control::WriteChannel * dmtpc::control::factory::makeWrite(const ChannelConfig * cfg)
{

  return cfg->getEnum("channel_type") == type_analog && cfg->getEnum("direction") == dir_write ? 
                                                      dynamic_cast<dmtpc::control::WriteChannel*> (dmtpc::control::factory::make(cfg)) :
                                                      0; 
}

dmtpc::control::ReadChannel * dmtpc::control::factory::makeRead(const ChannelConfig * cfg)
{

  return cfg->getEnum("channel_type") == type_analog && cfg->getEnum("direction") == dir_read ? 
                                                      dynamic_cast<dmtpc::control::ReadChannel*> (dmtpc::control::factory::make(cfg)) :
                                                      0; 
}


dmtpc::control::HVChannel * dmtpc::control::factory::makeHV(const ChannelConfig * cfg) 
{
  return cfg->getEnum("channel_type") == type_hv  ? 
                                                  dynamic_cast<dmtpc::control::HVChannel*> (dmtpc::control::factory::make(cfg)) :
                                                  0; 
}


dmtpc::control::Channel * dmtpc::control::factory::make (const ChannelConfig * cfg)
{

  unsigned devtype = cfg->getEnum("device_type"); 

  if (devtype == dev_comedi) 
  {
     #ifndef _ENABLE_COMEDI_
       fprintf(stderr,"ERROR: comedi device, but comedi not enabled\n"); 
       return 0; 
     #else

     #endif
  }

  if (devtype == dev_labjack)
  {
     #ifndef _ENABLE_LABJACK_
       fprintf(stderr,"ERROR: labjack device, but labjack not enabled\n"); 
       return 0; 
     #else
       unsigned devnum = cfg->getUInt("labjack_devnum");  

       unsigned type = cfg->getEnum("labjack_type"); 
       if (! labjacks[devnum-1]) 
       {
         labjacks[devnum-1] = LJUSB_OpenDevice(devnum, 0, type); 
         if (!labjacks[devnum-1])
         {
           fprintf(stderr,"ERROR: Could not open LabJack device."); 
           return 0; 
         }

       }
       std::vector<unsigned> * channel_list = cfg->getUIntList("channel"); 
       if (!channel_list->size())
       {
          fprintf(stderr,"ERROR, no channels specified!"); 
          return 0; 
       }

       if (cfg->getEnum("channel_type") == type_digital && type == 3) 
       {
         {
            return new U3DOChannel(labjacks[devnum-1], channel_list->size(), &(channel_list->at(0)), cfg->getUInt("fire_length")); 
         }
       }
       fprintf(stderr,"LABJACK MODE NOT IMPLEMENTED YET\n"); 

     #endif

  }


  fprintf(stderr,"Unresolved flow in dmtpc::control::factory::make\n"); 
  return 0; 

}







