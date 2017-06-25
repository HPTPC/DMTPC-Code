#include "feedback.h" 
#include "u3.h" 

#include <labjackusb.h>

#include <stdio.h>

int main (int nargs, char ** args) 
{

  labjack_ext_cmd cfg; 
  u3_build_config_io_fio_analog(&cfg,0); 

  labjack_feedback fb; 
  labjack_feedback_init(&fb,0); 
  labjack_feedback_add_PortStateWrite(&fb,0xff,0xff,0,0,0,0);  
  labjack_feedback_add_WaitLong(&fb,50); 
  labjack_feedback_add_PortStateWrite(&fb,0xff,0,0xff,0,0xff,0);  

  HANDLE labjack = LJUSB_OpenDevice(1,0,3); 
  printf("Handle: %x\n", labjack); 


  printf("configio returns: %d\n",labjack_send_receive(labjack, &cfg)); 

  printf("feedback returns: %d\n", labjack_feedback_process(labjack, &fb)); 


  LJUSB_CloseDevice(labjack); 

  return 0; 


}

