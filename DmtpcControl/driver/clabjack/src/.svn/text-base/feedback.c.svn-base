#include "feedback.h" 

#include <string.h>

const int feedback_write_size[64] =
{
  [IO_TYPE_AIN]=3,
  [IO_TYPE_WaitShort]=2,
  [IO_TYPE_WaitLong]= 2,
  [IO_TYPE_LED]=2,
  [IO_TYPE_BitStateRead] =2,
  [IO_TYPE_BitStateWrite] = 2,
  [IO_TYPE_BitDirRead]=2,
  [IO_TYPE_BitDirWrite]= 2,
  [IO_TYPE_PortStateRead]=1,
  [IO_TYPE_PortStateWrite]=7,
  [IO_TYPE_PortDirRead]=1,
  [IO_TYPE_PortDirWrite]=7, 
  [IO_TYPE_DAC0_8]=2,2, /* DAQ8*/
  [IO_TYPE_DAC0_16]=3,3, /*DAC 16*/ 
  [IO_TYPE_Timer0]=4,4,4,4, /*Timer and Timer Configs*/ 
  [IO_TYPE_Counter0]=2,2, /*Counters*/
  [IO_TYPE_Buzzer]=6 
};

const int feedback_read_size[64] =
{
  [IO_TYPE_AIN]=2,
  [IO_TYPE_WaitShort]=0,
  [IO_TYPE_WaitLong]= 0,
  [IO_TYPE_LED]=0,
  [IO_TYPE_BitStateRead] =1,
  [IO_TYPE_BitStateWrite] = 0,
  [IO_TYPE_BitDirRead]=1,
  [IO_TYPE_BitDirWrite]= 0,
  [IO_TYPE_PortStateRead]=3,
  [IO_TYPE_PortStateWrite]=0,
  [IO_TYPE_PortDirRead]=3,
  [IO_TYPE_PortDirWrite]=0, 
  [IO_TYPE_DAC0_8]=2,2, /* DAC8*/
  [IO_TYPE_DAC0_16]=3,3, /*DAC 16*/ 
  [IO_TYPE_Timer0]=4,4,4,4, /*Timer and Timer Configs*/ 
  [IO_TYPE_Counter0]=2,2, /*Counters*/
  [IO_TYPE_Buzzer]=6 
};


int count_io_type_write_size(const labjack_feedback * feedback)
{
  int i; 
  int size = 0; 
  for (i =0; i < feedback->ncmds; i++)
  {
    size += feedback_write_size[feedback->cmds[i]];
  }
  return size; 
}


int count_io_type_read_size(const labjack_feedback * feedback)
{
  int i; 
  int size = 0; 
  for (i =0; i < feedback->ncmds; i++)
  {
    size += feedback_read_size[feedback->cmds[i]];
  }
  return size; 
}

int labjack_feedback_init(labjack_feedback * feedback, BYTE echo) 
{

  int i; 
  feedback->cmd.cmd = LABJACK_CMD_EXT; 
  feedback->cmd.extcmd = LABJACK_EXTCMD_FEEDBACK; 
  feedback->cmd.data[0] = echo; 
  feedback->cmd.data[1] = 0;  //offset  
  feedback->response.cmd = 0; 
  feedback->ncmds = 0; 
  feedback->write_size = 0; 
  feedback->read_size = 0; 
  feedback->total_wait = 0; 
}

int labjack_feedback_add_cmd(labjack_feedback * feedback,labjack_feedback_iotype cmd, BYTE * vals) 
{
  /** too big! **/
  if (feedback->write_size + feedback_write_size[cmd] > 57 || feedback->read_size + feedback_read_size[cmd] > 55)
  {
    return 1; 
  }
  
  feedback->ncmds++; 
  feedback->cmds[feedback->ncmds] = cmd; 
  feedback->cmd.data[1 + feedback->write_size] = cmd;  //ofset by 1 because of echo byte 
  if (feedback_write_size[cmd] > 1)
  {
    memcpy(&(feedback->cmd.data[2 + feedback->write_size]), vals, feedback_write_size[cmd]-1); 
  }
  feedback->write_size += feedback_write_size[cmd]; 
  feedback->read_size += feedback_read_size[cmd]; 
}

int labjack_feedback_read_response(const labjack_feedback * feedback, labjack_feedback_iotype cmd, BYTE index, BYTE * vals)
{
  int read_index = 9; 
  int cmd_index = 0; 
  int i; 

  //probably not initialized 
  if (feedback->response.cmd != 0xf8)
  {
    return 1; 
  }

  for (i = 0; i < feedback->ncmds; i++)
  {
    if (feedback->cmds[i] == cmd)
    {
      if (cmd_index == index) 
      {
        memcpy(vals, feedback->response.data+read_index, feedback_read_size[cmd]); 
        return 0; 
      }
      else 
      {
        cmd_index++; 
      }
    }

    read_index += feedback_read_size[feedback->cmds[i]]; 
  }


  return 1; 
}

int labjack_feedback_process(HANDLE labjack, labjack_feedback * feedback)
{
  int err; 
  if (feedback->write_size % 2 == 0)
  {
    feedback->cmd.data[feedback->write_size+2] = 0; 
  }

  feedback->cmd.nwords = 1+feedback->write_size/ 2; 
  labjack_fill_cksum_ext(&(feedback->cmd)); 
  if (err = labjack_send(labjack, &feedback->cmd)) 
  {
    return err; 
  }

  if (err = labjack_receive(labjack, &feedback->response))
  {
    return err;
  }

  if (labjack_check_cksum_ext(&feedback->response))
  {
    return -67; 
  }

  if (feedback->response.data[0])
  {
    return feedback->response.data[1] + 1000; 
  }

  if (feedback->response.data[2] != feedback->cmd.data[0])
  {
    return -68; 
  }

  return 0; 
}

int labjack_feedback_add_AIN(labjack_feedback * feedback, BYTE positive_channel, BYTE negative_channel, BYTE long_settling, BYTE quick_sample)  
{
  BYTE vals[2];  
  vals[0] = 0; 
  vals[0] |= (positive_channel & 0x20); 
  if (long_settling)
    vals[0] |= (1 << 6); 
  if (quick_sample)
    vals[0] |= (1 << 7); 

  vals[1] = negative_channel; 

  return labjack_feedback_add_cmd(feedback, IO_TYPE_AIN, vals); 
}

int labjack_feedback_add_DAC8(labjack_feedback * feedback, BYTE channel, BYTE value) 
{
  return labjack_feedback_add_cmd(feedback, channel == 0 ? IO_TYPE_DAC0_8 : IO_TYPE_DAC1_8, &value);
}

int labjack_feedback_add_WaitShort(labjack_feedback * feedback, BYTE t) 
{
  return labjack_feedback_add_cmd(feedback,IO_TYPE_WaitShort, &t); 
}

int labjack_feedback_add_WaitLong(labjack_feedback * feedback, BYTE t) 
{
  return labjack_feedback_add_cmd(feedback,IO_TYPE_WaitLong, &t); 
}

int labjack_feedback_add_DAC16(labjack_feedback * feedback, BYTE channel, unsigned short value) 
{
  BYTE vals[2]; 
  vals[0] = value & 0xff; 
  vals[1] = value & 0xff00; 

  return labjack_feedback_add_cmd(feedback, channel == 0 ? IO_TYPE_DAC0_16 : IO_TYPE_DAC1_16, vals);
}

int labjack_feedback_add_PortStateWrite(labjack_feedback * feedback, 
                                        BYTE fio_mask, BYTE fio_vals, 
                                        BYTE eio_mask, BYTE eio_vals, 
                                        BYTE cio_mask, BYTE cio_vals ) 
{
  BYTE vals[6]; 

  vals[0] = fio_mask; 
  vals[1] = eio_mask; 
  vals[2] = cio_mask; 

  vals[3] = fio_vals; 
  vals[4] = eio_vals; 
  vals[5] = cio_vals; 

  return labjack_feedback_add_cmd(feedback, IO_TYPE_PortStateWrite, vals); 
}


int labjack_feedback_read_AIN(const labjack_feedback * feedback, BYTE index, unsigned short *val) 
{
  int err; 
  BYTE vals[2]; 
  if (err = labjack_feedback_read_response(feedback,IO_TYPE_AIN, index, vals))
  {
    return err; 
  }

  *val = 0; 
  *val |= vals[0]; 
  *val |= (vals[1] << 8); 

  return 0; 
}



