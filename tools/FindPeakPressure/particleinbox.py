#!/usr/bin/env python

#
# particleinbox.py
# Takes an ascii table from SnapToTable and finds the id of a particle
# within a radius of a specific point.
#

import sys
from math import sqrt

if len(sys.argv) != 6:
    sys.exit( "Usage: python particleinbox.py [snapshot table] [x] [y] [z] [radius]" );

filename = sys.argv[1]
x = float(sys.argv[2])
y = float(sys.argv[3])
z = float(sys.argv[4])
r = float(sys.argv[5])
id = -1

FILE = open( filename, "r" )

#find the required columns
header = FILE.readline()
headings = header.split("\t")
headings[0] = (headings[0])[1:] # remove hash from start

xcol = ycol = zcol = idcol = -1
count = 0

for heading in headings:
    if heading == 'x':
        xcol = count
    elif heading == 'y':
        ycol = count
    elif heading == 'z':
        zcol = count
    elif heading == 'id':
        idcol = count
    count += 1

if (xcol == -1) or (ycol == -1) or (zcol == -1) or (idcol == -1):
    sys.exit( "Invalid table." );

while 1:
    line = FILE.readline()
    if not line:
        break
    linelist = line.split("\t")
    X = float(linelist[xcol])
    Y = float(linelist[ycol])
    Z = float(linelist[zcol])
    ID = int(linelist[idcol])
    R = sqrt( (x-X)*(x-X) +
              (y-Y)*(y-Y) +
              (z-Z)*(z-Z) )
    if R < r:
      id = ID
      break

if id == -1:
    print "No particle found within "+str(r)+"cm of ( "+str(x)+", "+str(y)+", "+str(z)+" )."
else:
    print "Particle "+str(id)+" found at ( "+str(X)+", "+str(Y)+", "+str(Z)+" )."

FILE.close()

