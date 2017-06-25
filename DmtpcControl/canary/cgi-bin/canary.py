#! /usr/bin/env python 

import cgi 
import subprocess
import cgitb 
import sys 
import time 
import MySQLdb as mdb 
import json 

print "content-type: text/html"
print


db = mdb.connect(user="dmatter",passwd="seedark", db="canary_db")

cgitb.enable() 
form = cgi.FieldStorage() 


if not "cmd" in form: 
  sys.exit(1) 


cmd = form["cmd"].value 


if cmd == "setHV" and "value" in form and "ch" in form: 
  value = float(form["value"].value) 
  ch = int(form["ch"].value) 
  c = db.cursor() 
  c.execute("insert into caen_ch%d_set (value) VALUES (%f)" % (ch, value)) 

if cmd == "setEnabled" and "value" in form and "ch" in form: 
  value = int(form["value"].value) 
  ch = int(form["ch"].value) 
  c = db.cursor() 
  c.execute("insert into caen_ch%d_on (value) VALUES (%d)"% (ch, value)) 

if cmd == "getEnabled" and "ch" in form: 
  ch = int(form["ch"].value) 
  c = db.cursor() 
  c.execute("select value from caen_ch%d_on order by timestamp desc limit 1"% (ch)) 
  if c.fetchone()[0]: 
    print "<b>on</b>"
  else:
    print "off"

if cmd == "getInhibit": 
  c = db.cursor() 
  c.execute("select value from dd_inhibit order by timestamp desc limit 1") 
  if c.fetchone()[0]: 
    print "<b>INH</b>"
  else:
    print "..."


if cmd == "inhibit" and "value" in form:
  value = int(form["value"].value )
  c = db.cursor() 
  c.execute("insert into dd_inhibit (value) VALUES (%d)" % (value)) 



##DOESn'T WORK YET
if cmd == "check_slow":
  proc = subprocess.Popen("pgrep -f \"python.*canary_slow.py\"", shell=True, stdout=subprocess.PIPE)
  lines =  proc.stdout.readlines()
  print lines
  if len(lines) == 0: 
    print "<b>slow_canary.py NOT running</b>"
  else:
    for line in lines: 
      print "slow_canary.py executing as pid %s" %(line)




if cmd == "getAll":
  answer = []
  c = db.cursor() 

  for ch in range(4): 
    table = "caen_ch%d_vmon" % (ch)
    query = "select value,timestamp from %s order by timestamp desc limit 1" % (table)
    c.execute(query)
    answer.append([table,str(c.fetchone()[0])]) 

    table = "caen_ch%d_imon" % (ch)
    query = "select value,timestamp from %s order by timestamp desc limit 1" % (table)
    c.execute(query)
    answer.append([table,str(c.fetchone()[0])]) 

    table = "caen_ch%d_on" % (ch)
    query = "select value from %s order by timestamp desc limit 1" % (table)
    c.execute(query)
    answer.append([table,"ON" if c.fetchone()[0] else "OFF"]) 

  c.execute("select value,timestamp from dd_current_monitor order by timestamp desc limit 1") 
  answer.append(["dd_current_monitor",str(c.fetchone()[0])]) 

  c.execute("select value from dd_inhibit order by timestamp desc limit 1") 
  val = "..."
  if c.fetchone()[0]: 
    val= "<b>INH</b>"

  answer.append(["dd_inhibit",val]) 

  c.execute("select value from dd_temperature order by timestamp desc limit 1") 
  answer.append(["dd_temperature",str(c.fetchone()[0])]) 

  print "{"
  for i in answer:
    print "\"%s\": \"%s\"," % (i[0],i[1])

  print "\"when\": \"%s\"" % (time.strftime("%Y-%m-%d %H:%M:%S %Z"))  
  print "}"



  

  

if cmd == "getV" or cmd == "getI" or cmd == "getDD" or cmd == "getT": 

  isCaen = cmd == "getV" or cmd=="getI" 
  ch = 0


  n = 1
  if isCaen:
    ch  = int(form["ch"].value) 

  if "n" in form: 
    n = int(form["n"].value) 
  c = db.cursor() 

  table = "dd_current_monitor"

  if cmd == "getV": 
    table = "caen_ch%d_vmon" % (ch)
  if cmd == "getI": 
    table = "caen_ch%d_imon" % (ch)
  if cmd == "getT": 
    table = "dd_temperature" % (ch)

  query = "select value from %s order by timestamp desc limit %d" % (table, n)
  c.execute(query) 

  if n == 1: 
    print c.fetchone()[0]
  else:
    vals = [] 
    for t in c.fetchall(): 
      vals.append(t[0])
    print vals 


db.commit() 
db.close() 





print 






