#!/usr/bin/env python

import sys

if len(sys.argv) != 2:
    sys.exit("Usage: python calontable.py [table]");

FILE = open( sys.argv[1], "r" )

# Miss header row
FILE.readline()

tot = 0

while 1:
    line = FILE.readline()
    if not line:
        break

    linelist = line.split("\t")

    spU = float(linelist[10])
    mass = float(linelist[9])

    U = spU*mass

    tot += U

print "Total internal energy:", tot,
