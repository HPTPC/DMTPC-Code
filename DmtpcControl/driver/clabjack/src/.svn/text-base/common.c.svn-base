#include "common.h" 

#include <stdio.h>
#include <errno.h>

#ifdef DEBUG
#define DBG 1
#else
#define DBG 0 
#endif 




uint16_t labjack_checksum16(const uint8_t * buf, unsigned size)
{
  uint16_t a = 0; 
  unsigned i;

  for (i = 0; i < size; i++)
  {
    a+= buf[i]; 
  }

  return a; 
}

uint8_t labjack_checksum8(const BYTE * buf, unsigned size) 
{

  uint16_t a,q,r,i; 
  a=0; 

  for (i = 0; i < size; i++)
  {
    a+= buf[i]; 
  }

  q = a/256; 
  r = a % 256; 
  a = q+r; 
  q = a/256; 
  r = a % 256; 

  return (uint8_t) (q+r); 
}

int labjack_fill_cksum_ext(labjack_ext_cmd * cmd) 
{
  uint16_t c16; 
  c16 = labjack_checksum16(cmd->data, cmd->nwords*2); 
  if (DBG)
  {
    fprintf(stdout, "c16: %u\n", c16); 

  }
  cmd->cksum16.bytes[0] = (uint8_t)(c16 & 0xff); 
  cmd->cksum16.bytes[1] = (uint8_t)((c16/256) & 0xff); 
  cmd->cksum8 = labjack_checksum8(&(cmd->cmd), 5); 
  return 0;
}

int labjack_check_cksum_ext(const labjack_ext_cmd * cmd) 
{
  uint16_t c16; 
  if (cmd->cksum8 != labjack_checksum8(&(cmd->cmd), 5)) 
    return 2;
  c16 = labjack_checksum16(cmd->data, cmd->nwords*2); 
  return cmd->cksum16.bytes[0] != (uint8_t)(c16 && 0xff)|| 
         cmd->cksum16.bytes[1] != (uint8_t)((c16/256) & 0xff); 
}




int labjack_send(HANDLE labjack, const labjack_ext_cmd * cmd) 
{
  unsigned long nbytes, written; 
  int i; 
  nbytes = 6 + 2 * cmd->nwords; 
  written = LJUSB_Write(labjack, (BYTE*)cmd, nbytes); 
  if (DBG)
  {
    fprintf(stdout, "  cmd: "); 
    for (i = 0; i < nbytes; i++) fprintf(stdout,"0x%02x ", ((BYTE*) cmd)[i]); 
    fprintf(stdout, "\nLJUSB_Write(%x, %p, %d) returns %d\n",labjack,cmd,nbytes,written) ; 
  }
  return nbytes!= written; 
}

int labjack_send_receive(HANDLE labjack, labjack_ext_cmd * cmd)
{
  int ret; 
  if (!(ret=labjack_send(labjack,cmd)))
  {
    return labjack_receive(labjack,cmd); 
  }
  
  return ret; 

}

int labjack_receive(HANDLE labjack, labjack_ext_cmd * cmd)
{
  return labjack_receive_to(labjack,cmd,1000); 
}

int labjack_receive_to(HANDLE labjack, labjack_ext_cmd * cmd, unsigned int to) 
{

  int i; 
  unsigned long read; 
  read = LJUSB_Read_TO(labjack, (BYTE*) cmd, 256, to); 
  if (DBG)
  {
    fprintf(stdout,"LJUSB_Read(%x, %p, %d) returns %d\n",labjack,cmd,255,read) ; 
    fprintf(stdout," returned: "); 
    for (i = 0; i < read; i++) fprintf(stdout, "0x%02x ", ((BYTE*) cmd)[i]); 
    fprintf(stdout,"\n"); 
  }

  if (labjack_checksum8(&(cmd->cmd),5) != cmd->cksum8)
  {
    fprintf(stderr,"checksum8 failed,",errno); 
    return 2; 
  }

  if (labjack_checksum16(cmd->data,2*cmd->nwords) != cmd->cksum16.sh)
  {
    fprintf(stderr,"checksum61 failed,",errno); 
    return 3; 
  }



  return 0; 
}


