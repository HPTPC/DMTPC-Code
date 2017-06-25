#ifndef _clabjack_u3_h
#define _clabjack_u3_h

#include "common.h" 

#ifdef __cplusplus
extern "C" { 
#endif

int u3_build_config_io(labjack_ext_cmd * cmd, BYTE write_mask, BYTE timer_counter_config, BYTE dac1_enable,  BYTE fio_analog, BYTE eio_analog); 
int u3_build_config_io_eio_analog(labjack_ext_cmd * cmd, BYTE eio_analog); 
int u3_build_config_io_fio_analog(labjack_ext_cmd * cmd, BYTE fio_analog); 


int u3_decode_config_io(const labjack_ext_cmd * cmd, BYTE * errorcode, BYTE* timer_counter_config, BYTE* dac1_enable, BYTE * fio_analog, BYTE * eio_analog) ; 



#ifdef __cplusplus
}
#endif

#endif
