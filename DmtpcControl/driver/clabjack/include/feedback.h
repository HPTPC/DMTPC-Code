#ifndef _clabjack_feedback_h
#define _clabjack_feedback_h

/* Labjack feedback utility functions
 *
 *  Cosmin Deaconu <cozzyd@mit.edu> 
 *
 */ 

#include "common.h" 
#ifdef __cplusplus
extern "C" { 
#endif


typedef struct
{
  labjack_ext_cmd cmd; 
  labjack_ext_cmd response; 
  BYTE cmds[32]; 
  double total_wait; 
  BYTE ncmds; 
  BYTE write_size; 
  BYTE read_size; 
}labjack_feedback; 

typedef enum
{
  IO_TYPE_AIN = 1, 
  IO_TYPE_WaitShort = 5, 
  IO_TYPE_WaitLong = 6, 
  IO_TYPE_LED = 9, 
  IO_TYPE_BitStateRead = 10, 
  IO_TYPE_BitStateWrite = 11, 
  IO_TYPE_BitDirRead = 12, 
  IO_TYPE_BitDirWrite = 13, 
  IO_TYPE_PortStateRead = 26, 
  IO_TYPE_PortStateWrite = 27, 
  IO_TYPE_PortDirRead = 28, 
  IO_TYPE_PortDirWrite = 29, 
  IO_TYPE_DAC0_8 = 34, 
  IO_TYPE_DAC1_8 = 35, 
  IO_TYPE_DAC0_16 = 38, 
  IO_TYPE_DAC1_16 = 39, 
  IO_TYPE_Timer0 = 42, 
  IO_TYPE_TimerConfig0 = 43, 
  IO_TYPE_Timer1 = 44, 
  IO_TYPE_TimerConfig1 = 45, 
  IO_TYPE_Counter0 = 54, 
  IO_TYPE_Counter1 = 55, 
  IO_TYPE_Buzzer = 63 
}labjack_feedback_iotype; 



int labjack_feedback_init(labjack_feedback * feedback, BYTE echo); 
int labjack_feedback_process(HANDLE labjack, labjack_feedback * feedback); 

int labjack_feedback_add_cmd(labjack_feedback * feedback,labjack_feedback_iotype cmd, BYTE * vals); 
int labjack_feedback_read_response(const labjack_feedback * feedback,labjack_feedback_iotype cmd, BYTE index, BYTE * vals); 


int labjack_feedback_add_AIN(labjack_feedback * feedback, BYTE positive_channel, BYTE negative_channel, BYTE long_settling, BYTE quick_sample);  
int labjack_feedback_add_DAC8(labjack_feedback * feedback, BYTE channel, BYTE value); 
int labjack_feedback_add_DAC16(labjack_feedback * feedback, BYTE channel, unsigned short value); 
int labjack_feedback_add_WaitShort(labjack_feedback * feedback, BYTE times_128us); 
int labjack_feedback_add_WaitLong(labjack_feedback * feedback, BYTE times_16384us); 


int labjack_feedback_add_PortStateWrite(labjack_feedback * feedback, 
                                        BYTE fio_mask, BYTE fio_vals, 
                                        BYTE eio_mask, BYTE eio_vals, 
                                        BYTE cio_mask, BYTE cio_vals ); 

int labjack_feedback_read_AIN(const labjack_feedback * feedback, BYTE index, unsigned short *val); 


#ifdef __cplusplus
}
#endif

#endif  // include guard 
