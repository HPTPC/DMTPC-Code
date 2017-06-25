#!/usr/bin/python

#print "<html><head><title>title</title></head>"
#print "<body>"

import MySQLdb as sql
import cgi
import sys

print "Content-type: text/html\n\n"
                                                                            
database = "DMTPC_TEST"
hostname = "m3slow.lns.mit.edu"
username = "dmatter"
password = "seedark"

fields = cgi.FieldStorage()

db = sql.connect(hostname, username, password, database)

if "name" not in fields or "start_time" not in fields and "end_time" not in fields:
    print ""
    sys.exit(1)

name = fields.getvalue("name")
start_time = fields.getvalue("start_time")
end_time = fields.getvalue("end_time")

query = "SELECT avg(value) FROM "+str(name)+" WHERE timestamp > '"+str(start_time)+"' AND timestamp < '"+str(end_time)+"';"

cursor = db.cursor()
cursor.execute(query)
result = cursor.fetchall()

tables = []

for record in result:
    print record[0]
    

#print "</body>"
#print "</html>"
