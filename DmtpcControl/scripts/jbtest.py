#!/usr/bin/python

import numpy
import MySQLdb as sql

print "Content-type: text/html\n\n"
print "<html><head><title>title</title></head>"
print "<body>"

database = "DMTPC_TEST"
hostname = "rubin.lns.mit.edu"
username = "dmatter"
password = "seedark"

db = sql.connect(hostname, username, password, database)

query = "SHOW tables;"

print query

cursor = db.cursor()
cursor.execute(query)
result = cursor.fetchall()

tables = []
for record in result:
    print record

print "</body>"
print "</html>"

#
#print "in python!<br>"
###command='root -l -q -b test.C'
###os.system(command)
#print "out of python!<br>"
#

