#! /usr/bin/python

import cgi 
import gzip
import datetime
import time
import os
import MySQLdb as my 
import sys

cache_secs = 10 

fields = cgi.FieldStorage() 

dirty = False


start_time = fields.getvalue("start") 

utcoffset = datetime.datetime.now() - datetime.datetime.utcnow();

if start_time == None:
  start_time =  (datetime.datetime.now() - datetime.timedelta(hours=5)).strftime("%Y-%m-%d %H:%M:%S")
else:
  dirty = True
  start_time = (datetime.datetime(*(time.strptime(start_time,"%Y%m%d%H%M%S")[0:6])) + utcoffset).strftime("%Y-%m-%d %H:%M:%S")

end_time = fields.getvalue("end") 
if end_time == None:
  end_time =  datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
else:
  dirty = True
  end_time = (datetime.datetime(*(time.strptime(end_time,"%Y%m%d%H%M%S")[0:6])) + utcoffset).strftime("%Y-%m-%d %H:%M:%S")

if (fields.getvalue("cache_secs")!=None):
  cache_secs = int(field.getValue("cache_secs"))

min = fields.getvalue("min")

if min == None:
  min = "-100"
else:
  dirty = True

max = fields.getvalue("max")
if max == None:
  max = "1e10"
else:
  dirty = True

wavelength = fields.getvalue("wavelength")
if wavelength == None:
  wavelength = 10
else:
  wavelength = int(wavelength)
table_name = fields.getvalue("which")

col_name = fields.getvalue("col")
if col_name == None:
  col_name = "value"

nevery = fields.getvalue("nevery"); 
if int(nevery) != 1:
  dirty = True

print "Content-type:text/plain"


if table_name == None:
  print "NO TABLE SPECIFIED, SHAME ON YOU" 
  sys.exit(1)

print "Content-Encoding: gzip"

cache_file = "flot_cache/"+table_name

if dirty:
  cache_file+= "_dirty_" + str(os.getpid());  

if dirty or not os.path.exists(cache_file) or os.path.getmtime(cache_file) < time.time() - cache_secs:

  temp_name = cache_file + str(os.getpid())
  
  if not dirty and os.path.exists(temp_name):
    pass

  else:

    conn = my.connect("localhost","dmatter","seedark","DMTPC_TEST")
    cur = conn.cursor();

    sql = "SELECT timestamp,"+col_name+" FROM " + table_name + " WHERE "+col_name+" < '" + max + "' AND "+col_name+" >= '" + min + "' AND timestamp > '" + start_time + "' AND timestamp < '" + end_time + " '"# AND id mod "+nevery+" = 0"
    cur.execute(sql)


    f = gzip.open(temp_name,"w")
    rows = cur.fetchall()
    sys.stderr.write("Table: "+table_name+" rows: " + str( len(rows) ) )
    f.write("{\nlabel: '" + table_name + "',\ndata: ["); 
    
    npts = 1
    ave = 0
    for idx,row in enumerate(rows):
      ave = ave + row[1]/wavelength
      npts = npts + 1
      if npts % wavelength!=0:
        continue

      f.write("["+repr(1000. * float(row[0].strftime('%s'))) +"," + repr(ave) + "]")
      if idx < len(rows) - wavelength-1: f.write(",")
      npts = 1
      ave = 0
    f.write("]\n}"); 
    f.close()
    os.rename(temp_name,cache_file)
    cur.close()
    conn.close()


f = open(cache_file,"r")
zbuf = f.read() 
f.close()
 
print "Content-Length: " + str(len(zbuf))
print 

sys.stdout.write(zbuf)
sys.stdout.write("\r\n")

if dirty:
  os.remove(cache_file); 

