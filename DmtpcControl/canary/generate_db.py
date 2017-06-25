#! /usr/bin/env python

caen_on_channels = ["caen_ch0_on","caen_ch1_on","caen_ch2_on","caen_ch3_on"] 
caen_set_channels = ["caen_ch0_set","caen_ch1_set","caen_ch2_set","caen_ch3_set"] 
caen_read_channels = ["caen_ch0_vmon","caen_ch1_vmon","caen_ch2_vmon","caen_ch3_vmon"] 
caen_read_channels += ["caen_ch0_imon","caen_ch1_imon","caen_ch2_imon","caen_ch3_imon"] 
dd_channel = ["dd_current_monitor", "dd_inhibit"] 


channels  = caen_set_channels + caen_read_channels + dd_channel + caen_on_channels; 

for ch in channels:
  print "drop table if exists %s;" % (ch) 
  print "create table %s (value float not null, timestamp TIMESTAMP(6) not null default NOW(6) on update NOW(6) primary key);" % (ch)


for ch in caen_on_channels + caen_set_channels + ["dd_inhibit"] : 
  print "insert into %s (value) values (0);" % (ch)


