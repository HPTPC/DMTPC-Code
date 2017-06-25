#!/usr/bin/env python
import sys
#print 'Number of arguments:', len(sys.argv), 'arguments.'
#print 'Argument List:', str(sys.argv)
if  len(sys.argv)!=2:
   print "   provide XML file name as argument, exit"
   exit(1)

xmlName=sys.argv[1]

import xml.etree.ElementTree as ET
tree = ET.parse(xmlName)
#tree = ET.parse('oneChan.xml')
root = tree.getroot()

print 'root=',root.tag,'  attrib:',root.attrib
for child1 in root:
   print '  ',child1.tag,child1.text,child1.attrib
   for child2 in child1:
      print '     ',child2.tag,child2.text,child2.attrib
      for child3 in child2:
         print '          ',child3.tag,child3.text,child3.attrib
