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


def freedisk(path):
    """Use the df command to discover the amount of free disk space for
    a particular mount.
    """
    
    cmd = "df -h %s" % path
    stdout,stdin = popen2.popen2(cmd)
    stdin.close()
    print "<pre>"
    print "> ",cmd
    for line in stdout:
        print line,
    print "</pre>"


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
                
        for volume in volumes:
            freedisk(volume)
                                                
        print """</body>

</html>"""    

        
