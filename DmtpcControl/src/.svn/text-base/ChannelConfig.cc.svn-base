#include "ChannelConfig.hh" 

dmtpc::control::ChannelConfig::ChannelConfig(const char * file) 
{
  const char * device_keys[] = {"comedi","caenhv","labjack"}; 
  registerEnum("device_type",sizeof(device_keys)/sizeof(char*), device_keys, NULL); 

  const char * type_keys[] = {"analog","digital","hv"}; 
  registerEnum("channel_type",sizeof(type_keys)/sizeof(char*), type_keys, NULL); 

  const char * dir_keys[] = {"read","write"}; 
  registerEnum("direction",sizeof(dir_keys)/sizeof(char*), dir_keys, NULL); 

  registerString("comedi_device","/dev/comedi0"); 

  registerUInt("labjack_devnum",1); 

  const char * labjack_keys[] = {"u3","u6","ue9","u12"}; 
  int labjack_indices[] = {3,6,9,1}; 

  registerEnum("labjack_type",sizeof(labjack_keys) / sizeof(char*), labjack_keys,labjack_indices,3); 
  registerUIntList("channel"); 

  registerUInt("fire_length",1000); 



}


