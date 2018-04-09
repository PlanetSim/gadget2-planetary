#!/usr/bin/env python

fp = open( "iron_bell.txt", "r" )

lines = []

while 1:
    line = fp.readline()
    if not line:
        break
    lines.append( line )

fp.close()

for i in range( 1, len(lines) ):
    lines[i] = lines[i][:-1]+"\t"+str(i)+"\n"

fp = open( "iron_bell2.txt", "w" )
fp.writelines(lines)
fp.close()
