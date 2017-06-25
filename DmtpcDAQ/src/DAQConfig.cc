#include "DAQConfig.hh"

dmtpc::daq::DAQConfig::DAQConfig(const char * file)
{

  /** Basic options **/ 
  registerBool("verbose",false); 
  registerString("out_dir", "."); 
  registerString("tag", "run"); 


  const char * compression_algos[] = {"zlib","lzma"};  
  registerEnum("compression_algo", sizeof(compression_algos)/sizeof(*compression_algos), compression_algos,0,1);  
  registerUInt("compression_level",5); 

  /** Database Options **/ 
  registerString("db_config","db.conf"); 


  /** Trigger Options **/ 

  registerUInt("trigger_pretrigger",10); 
  registerUInt("trigger_posttrigger",10); 
  

  /** CCD Options **/ 

  //basic
  registerUInt("ccd_xbin",4); 
  registerUInt("ccd_ybin",4); 
  registerUInt("ccd_exposure",1000); 
  registerUInt("ccd_nflushes",1); 
  registerDouble("ccd_temp",0); 

  /*At some point we're going to need to be able to specify binning on a CCD basis... probably
   * have a map of serials to binnings for individual cameras */ 

  //advanced
  registerBool("ccd_crop",false); 
  registerDouble("ccd_ul_x_frac",0); 
  registerDouble("ccd_ul_y_frac",0); 
  registerDouble("ccd_lr_x_frac",1); 
  registerDouble("ccd_lr_y_frac",1); 
  registerBool("ccd_dark_shutter_open",false); 
  registerBool("ccd_shutter_always_open",true); 
  registerUInt("ccd_readout_speed_h",1e6);  // 1 MhZ 
  registerUInt("ccd_readout_speed_v",1e6);  // 1 MhZ 
  registerUInt("ccd_buffer_size",20); 
  registerUInt("n_fake_cameras",0); 

  //camera specific
  registerString("andor_init_dir","/usr/local/etc/andor/"); 
  
  //selectively disable cameras
  registerBool("enable_andor_ikon",true); 
  registerBool("enable_fli_proline",true); 

  

  //external trigger
  registerBool("ccd_external_trigger",false); 
  registerString("ccd_external_trigger_channel_conf","external.conf"); 

  registerBool("ccd_threaded_readout",false); 

  /** Scope Options **/ 


  registerBool("scope_enabled",false); 
  registerUInt("scope_wait_timeout",100); 
  registerUInt("scope_buffer_size",10); //should we change to something larger? NG

  //scope specific 
  const char * caen_connection_types[] = {"usb","pcielink","vmelink"}; 
  registerUInt("caen_nboards",1); 
  std::vector<int> default_connection_types(1, 1); 
  registerEnumList("caen_connection_types", sizeof(caen_connection_types) / sizeof(const char *), caen_connection_types,0,&default_connection_types); 
  std::vector<unsigned> default_link_nums(1); 
  registerUIntList("caen_link_nums",&default_link_nums); 
  std::vector<unsigned> default_conet_nodes(1); 
  registerUIntList("caen_conet_nodes",&default_conet_nodes); 

  //registerUIntList(const char * name, const vector<unsigned> *default_val = 0); 

   /*Board Configs*/
  
  std::vector<unsigned> default_record_length(1,10); 
  registerUIntList("caen_record_length",&default_record_length); //in us
  std::vector<unsigned> default_post_trigger(1,75);
  registerUIntList("caen_post_trigger",&default_post_trigger); //percent of window after trigger
  std::vector<unsigned> default_num_events_blt(1,10);
  registerUIntList("caen_num_events_BLT",&default_num_events_blt);
 
  const char * caen_board_configs[] = {"primus","segundus"};
  std::vector<int>default_board_config(1, 1);
  registerEnumList("caen_board_cfg", sizeof(caen_board_configs) / sizeof(const char *), caen_board_configs,0,&default_board_config); 
  registerBool("software_trigger",false); //software sends acquisition trigger 

  /*Channel Configs*/
  
  registerUInt("caen_cfg_single_channel",0);
  const unsigned enabled_channels_a[] = {0,1,2,3,4,5,6,7}; 
  const std::vector<unsigned> channels_a(enabled_channels_a,enabled_channels_a + sizeof(enabled_channels_a)/sizeof(unsigned));
  registerUIntList("caen_cfg_channel_a",&channels_a);

  const unsigned enabled_channels_b[] = {0,1,2,3,4,5,6,7}; 
  const std::vector<unsigned> channels_b(enabled_channels_b,enabled_channels_b + sizeof(enabled_channels_b)/sizeof(unsigned));
  registerUIntList("caen_cfg_channel_b",&channels_b);

  std::vector<unsigned> default_vpp(1,2000);
  registerUIntList("caen_vpp", &default_vpp); //in mV
  //registerUInt("caen_vpp_ch0");
  //registerUInt("caen_vpp_ch1");
  //const char * caen_vpp_list[] = {"caen_vpp_ch0","caen_vpp_ch1"};
  //int n_vpp = sizeof(caen_vpp_list)/sizeof(const char *);
  //registerEnumList("caen_vpp_brd0",n_vpp,caen_vpp_list);

  const char * caen_trigger_mode[] = {"disabled","extonly","acqonly","both"};
  int ntrigger_mode = sizeof(caen_trigger_mode)/sizeof(const char *); 
  registerEnum("caen_default_self_trigger_mode", ntrigger_mode,caen_trigger_mode,0,0);  //most channels don't trigger
  registerEnum("caen_default_ext_trigger_mode", ntrigger_mode,caen_trigger_mode,0, 2); //most channels triggered externally

  //registerIntMap(const char * name, const vector<const char *> * default_keys = 0, const vector<int> * default_vals = 0);
  
    
  registerEnumList("caen_cfg_self_trigger_mode",ntrigger_mode, caen_trigger_mode); 
  registerEnumList("caen_cfg_ext_trigger_mode", ntrigger_mode, caen_trigger_mode); 
  //registerEnumList("caen_cfg_acq_mode");

  std::vector<int> default_dc_offset(1,-800);
  registerIntList("caen_dc_offset_voltage",&default_dc_offset); //mV

    
  registerIntList("caen_vmin"); //mV
  registerIntList("caen_vmax"); //mV
  
  std::vector<int> default_trigger_value(1,-250); //mV
  registerIntList("caen_channel_trigger_level_volts",&default_trigger_value); //trigger in volts 
  //registerUInt("caen_channel_trigger_level_adc",9000); //trigger in ADC counts
  
  const char * scope_polarity[] = { "positive","negative" }; 
  registerIntList("caen_pulse_polarity",0); //0=rising, 1=falling
  registerIntList("caen_trigger_polarity",0); //0=rising, 1=falling

  //"caen_cfg_channel" here?
  //channel vs channel mask


  if (file)
    parseFile(file);
}

