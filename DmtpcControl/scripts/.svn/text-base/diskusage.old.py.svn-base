#!/usr/bin/python

# D.Dujmic, MIT
# (based on script by A.Dvoretskii, Caltech)

import popen2
import re
import os
import time
import sys
import glob
import socket
from optparse import OptionParser
from ConfigParser import ConfigParser

def factorBytes(size,maxlevel=6):
    """Factor bytes into a human readable format. Default maxlevel of 6
    corresponds to petabytes.
    """

    size = float(size)
    level = 0
    while size >= 1024 and level < maxlevel:
        size /= 1024
        level += 1

    return (size,level)

def formatBytes(size,suffixes=["", "KB", "MB", "GB", "TB", "EB", "PB"]):
    """Format bytes into a human readable string
    """

    maxlevel = len(suffixes) - 1
    size,level = factorBytes(size)
    
    s = "%.1f" % size
    s += suffixes[level]
    return s
    

def freedisk(path):
    """Use the df command to discover the amount of free disk space in kB for
    a particular mount.
    """
    
    cmd = "df %s" % path
    print cmd
    stdout,stdin = popen2.popen2(cmd)
    stdin.close()
    line = stdout.readlines()[2]
    
    #match = re.match(r"(.*):([^\s]+)\s+(\d+)\s+(\d+)\s+(\d+).*",line)
    match = re.match("\s+(\d+)\s+(\d+)\s+(\d+).*",line)
    assert match is not None
    
    total = long(match.group(1))*1024
    used = long(match.group(2))*1024
    free = long(match.group(3))*1024
    
    return total,used,free


def dirusage(root):
    """Use the ds command to get a usage summary for all subdirectories
    of a particular root.
    """

    files = glob.glob("%s/*" % root)
    ret = []
    
    for file in files:

        cmd = "du -s %s" % file

        stdout,stdin = popen2.popen2(cmd)
        stdin.close()
        input = stdout.readlines()

        for line in input:

            match = re.match("([0-9]+)\t+(.+)\n", line)
            assert match is not None

            size = long(match.group(1)) * 1024
            path = match.group(2).strip()
            root, dir = os.path.split(path)
            ret.append((file,size))

    return ret

if __name__ == "__main__":

    parser = OptionParser()
    parser.add_option("--threshold",help="Threshold size in KB",type="float",default=1000.)
    parser.add_option("--format",help="Output format",default="html")
    parser.add_option("--config",help="Configuration file",default="/usr/local/apache2/cgi-bin/diskusage.cfg")
    opts,args = parser.parse_args()

    threshold = opts.threshold * 1024.
    format=opts.format
    config = opts.config
    hostname = socket.gethostname()

    if not os.path.exists(config):

        raise IOError,"Configuration file %s does not exist" % config

    cp = ConfigParser()
    cp.read(config)
    
    title = "Disk usage"
            
    print """Content-type:text/html\n\n

            
    <head>
    <title>%s</title>
    <link rel="stylesheet" href="style.css" type="text/css" />
    </head>
    <body>
    """ % (title)
            
    t = time.asctime(time.localtime(time.time()))
    print '<div id="generated">'
    print 'Last update: %s    from host: %s.' % (t,hostname)
    print '</div>'
            
    print "<h1>%s</h1>" % (title)
            
    for section in cp.sections():
                
        name = section
        if cp.has_option(section,"name"): name = cp.get(section,"name")
                
        volumes = []
        if cp.has_option(section,"volumes"): volumes = cp.get(section,"volumes").split()
                
        paths = []
        if cp.has_option(section,"paths"): paths = cp.get(section,"paths").split()
                
        print "<h2>%s</h2>" % name
                
        print "<h3>Volumes</h3>" 
                
        print "<table>"
        print "<thead>"
        print "<tr>"
        print "<th>Volume</th>"
        print "<th>Total</th>"
        print "<th>Used</th>"
        print "<th>Free</th>"
        print "<th>Use%</th>"
        print "</tr>"
        print "</thead>"
        print "<tbody>"
                
        for volume in volumes:
                    
            total,used,free = freedisk(volume)
                    
            st = formatBytes(total)
            su = formatBytes(used)
            sf = formatBytes(free)
            sc = "%4.1f%%" % (100.*used/total)
            print "<tr>"
            print "<td>%s</td>" % (volume)
            print "<td>%s</td>" % (st)
            print "<td>%s</td>" % (su)
            print "<td>%s</td>" % (sf)
            print "<td>%s</td>" % (sc)
            print "</tr>"
                    
            print "</tbody>"
            print "</table>"
                                                
        print """</body>

</html>"""    

        
