#! /usr/bin/env python

import serial 
import sys

class caen_hv: 


  def __init__(self, port = "/dev/ttyUSB0", board = 0): 
    self.p  = serial.Serial(port,9600,xonxoff=True,timeout=1) 
    self.board = board; 


  def cmd(self, command ): 
    self.p.write(command + "\r\n"); 
    return self.p.readline() 

  def setHV(self, channel, value): 
    self.cmd("$BD:%d,CMD:SET,CH:%d,PAR:VSET,VAL:%06.1f" % (self.board, channel, value))

  def setOn(self, channel, on=True): 
    if on:
      self.cmd("$BD:%d,CMD:SET,CH:%d,PAR:ON" % (self.board, channel))
    else:
      self.cmd("$BD:%d,CMD:SET,CH:%d,PAR:OFF" % (self.board, channel))


  def setOff(self, channel):
    self.setOn(channel,False)

  def readHV(self, channel=4, vmon = True, imon = True, pol = True, echo = True): 

    answer = []  

    if vmon:
      ret = self.cmd("$BD:%d,CMD:MON,CH:%d,PAR:VMON" % (self.board, channel))
      if echo:
        sys.stdout.write(ret)
      answer.append([float(i) for i in ret.split(":")[3].split(";")]) 


    if imon:
      ret = self.cmd("$BD:%d,CMD:MON,CH:%d,PAR:IMON" % (self.board, channel))
      if echo:
        sys.stdout.write(ret)
      answer.append([float(i) for i in ret.split(":")[3].split(";")]) 

    if pol:
      ret = self.cmd("$BD:%d,CMD:MON,CH:%d,PAR:POL" % (self.board, channel))
      if echo: 
        sys.stdout.write(ret)
      answer.append(ret.split(":")[3].split(";")) 

    return answer 







