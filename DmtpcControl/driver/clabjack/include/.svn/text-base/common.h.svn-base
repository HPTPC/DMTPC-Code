#ifndef _clabjack_common_h_
#define _clabjack_common_h_

/** higher level driver for labjack u3 */ 

#include "labjackusb.h" 
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

static const BYTE LABJACK_CMD_EXT = 0xf8; 
static const BYTE LABJACK_EXTCMD_CONFIGIO = 0x0b; 
static const BYTE LABJACK_EXTCMD_FEEDBACK = 0x00; 


typedef struct 
{
  BYTE cksum8; 
  BYTE cmd; 
  BYTE nwords; 
  BYTE extcmd; 
  union 
  {
    uint16_t sh; 
    BYTE bytes[2]; 
  } cksum16;
  BYTE data[249]; 
} labjack_ext_cmd;  


typedef struct
{
  BYTE cksum8; 
  BYTE cmd; 
  BYTE data[14]; 
}labjack_cmd; 



int labjack_check_cksum_ext(const labjack_ext_cmd * cmd); 
int labjack_fill_cksum_ext(labjack_ext_cmd * cmd); 


int labjack_send(HANDLE labjack, const labjack_ext_cmd * cmd); 
int labjack_receive(HANDLE labjack, labjack_ext_cmd * receive); 
int labjack_receive_to(HANDLE labjack, labjack_ext_cmd * receive, unsigned int timeout_in_ms); 
int labjack_send_receive(HANDLE labjack, labjack_ext_cmd * receive); 








#ifdef __cplusplus
}
#endif

#endif  // include guard 
