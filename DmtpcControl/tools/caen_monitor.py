#! /usr/bin/env python

import caen_hv
import time;
import sys

port = "/dev/ttyACM0"

if len(sys.argv) > 1: 
  port = sys.argv[1]

caen = caen_hv.caen_hv(port)

while True:

  time.sleep(1)
  print"[measurement]"
  print "t=%.5f" % time.time() 
  print "Voltages: "
  caen.readHV(4,imon=False,pol=False)
  print "t=%.5f" % time.time() 
  print "Currents: "
  caen.readHV(4,vmon=False,pol=False)
  print "t=%.5f" % time.time() 
  print "" 
