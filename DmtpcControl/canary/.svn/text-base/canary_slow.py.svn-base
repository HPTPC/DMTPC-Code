#! /usr/bin/env python

import MySQLdb as mdb
import sys 
import u3
import time 
import os 


dd_read_port = 0 
dd_temperature_port = 4 
dd_inhibit_port = 4  #FIO 

dd_dir_port = 6100 + dd_inhibit_port; 
dd_write_port= 6000 + dd_inhibit_port; 

limit = 100  #uA
#limit = 100 



emergency = "7758469105@txt.att.net" 

sys.path.append("../tools/") 
import caen_hv 


db = mdb.connect(user="dmatter", passwd="seedark",db="canary_db") 
lj = u3.U3() 


lj.writeRegister(dd_dir_port,1) 
lj.writeRegister(dd_write_port,0) #don't inhibit 


port = "/dev/ttyUSB0"

if len(sys.argv) > 1:
  port = sys.argv[1] 


caen = caen_hv.caen_hv(port) 


last_on_count = [0,0,0,0]
last_set_count = [0,0,0,0]
last_inhibit_count = 0

last_imon_value = [-10000,-100000,-100000,-100000] 
last_vmon_value = [-100000,-100000,-100000,-100000] 
last_dd_imon_value = -10000 

last_dd_temperature_value = 0
last_time = 0
c = db.cursor() 

while True: 


  # check on caen status 
  for ch in range(4): 
    c.execute("select count(*) from caen_ch%d_on" % (ch))
    howmany = c.fetchone()[0]
    if howmany > last_on_count[ch]: 
      c.execute("select value from caen_ch%d_on order by timestamp desc limit 1" % (ch))
      on = c.fetchone()[0] != 0

      if (on):
        caen.setOn(ch)
      else:
        caen.setOff(ch)
      last_on_count[ch] = howmany 

  # check on caen set 
  for ch in range(4): 
    c.execute("select count(*) from caen_ch%d_set" % (ch))
    howmany = c.fetchone()[0]
    if howmany > last_set_count[ch]: 
      c.execute("select value from caen_ch%d_set order by timestamp desc limit 1" % (ch))
      caen.setHV(ch, c.fetchone()[0])
      last_set_count[ch] = howmany 



  # read caen
  answer = caen.readHV(echo=False)


  voltages = answer[0] 
  currents = answer[1] 
  polarities = answer[2] 

  for i in range(4):
    if polarities[i] == "-":
      voltages[i] *= -1 

  for ch in range(4): 
    if voltages[ch] != last_vmon_value[ch]:
      last_vmon_value[ch] = voltages[ch] 
      q = "insert into caen_ch%d_vmon (value) values (%f)" %(ch,voltages[ch])
      print q
      c.execute(q) 

    if currents[ch] != last_imon_value[ch]:
      last_imon_value[ch] = currents[ch] 
      q = "insert into caen_ch%d_imon (value) values (%f)" %(ch,currents[ch])
      print q
      c.execute(q) 

  db.commit(); 

  #read dd 
  v = lj.readRegister(dd_read_port) 
  v *= 60. # rough calibration
  if v != last_dd_imon_value: 
    last_dd_imon_value = v; 
    q = "insert into dd_current_monitor (value) VALUES (%f) "%  (v)
    print q
    c.execute(q)

    db.commit(); 

  t = time.time()

  if v > limit and t - last_time > 1800 : 
    last_time = t
    os.system("echo CURRENT MONITOR IS %f+ | mail %s" %(v, emergency)) 
    q = "insert into dd_inhibit (value) VALUES(1)"
    c.execute(q) 

  db.commit(); 
 



  #check on inhibit status 
  c.execute("select count(*) from dd_inhibit") 
  howmany = c.fetchone()[0] 
  if howmany > last_inhibit_count: 
    c.execute("select value from dd_inhibit order by timestamp desc limit 1") 
    on = int(c.fetchone()[0])
    lj.writeRegister(dd_write_port,on) 
    last_inhibit_count = howmany 


  # read temperature sensor
  v = lj.readRegister(dd_temperature_port) 
  v *= 100 
  if v != last_dd_temperature_value:
    last_dd_temperature_value = v
    q = "insert into dd_temperature (value) VALUES (%f) " % (v) 
    print q
    c.execute(q) 
    db.commit()

  time.sleep(0.5)


