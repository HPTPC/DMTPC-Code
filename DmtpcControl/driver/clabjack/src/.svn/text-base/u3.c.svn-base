#include "u3.h" 


int u3_build_config_io(labjack_ext_cmd * cmd, BYTE write_mask, BYTE timer_counter_config, BYTE dac1_enable, BYTE fio_analog, BYTE eio_analog) 
{
  cmd->cmd = LABJACK_CMD_EXT; 
  cmd->nwords = 0x03; 
  cmd->extcmd = LABJACK_EXTCMD_CONFIGIO; 
  cmd->data[0] = write_mask; 
  cmd->data[1] = 0; 
  cmd->data[2] = timer_counter_config; 
  cmd->data[3] = dac1_enable; ; 
  cmd->data[4] = fio_analog; 
  cmd->data[5] = eio_analog; 
  labjack_fill_cksum_ext(cmd); 
  return 0; 
}

int u3_build_config_io_eio_analog(labjack_ext_cmd * cmd, BYTE eio_analog) 
{
  return u3_build_config_io(cmd, 1 << 3,0,0,0,eio_analog); 
}

int u3_build_config_io_fio_analog(labjack_ext_cmd * cmd, BYTE fio_analog) 
{
  return u3_build_config_io(cmd, 1 << 2,0,0,0,fio_analog); 
}


int u3_decode_config_io(const labjack_ext_cmd * cmd, BYTE * error_code, BYTE* timer_counter_config, BYTE* dac1_enable, BYTE * fio_analog, BYTE * eio_analog) 
{
  if (error_code) *error_code= cmd->data[0]; 
  if (timer_counter_config) *timer_counter_config = cmd->data[2];
  if (dac1_enable) *timer_counter_config = cmd->data[3];
  if (fio_analog) *fio_analog = cmd->data[4];
  if (eio_analog) *eio_analog = cmd->data[5];

  return 0; 
}

