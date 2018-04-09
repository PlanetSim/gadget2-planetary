#!/usr/bin/env python

# Uses SnapToTable to analyse a directory of snapshots
# Assumes that the snapshot files all end with a 3-numeral digit
# e.g. snapshot_081 which is used as a temperoral id

import sys
import os
import commands

if (len(sys.argv) != 5):
     sys.exit("Usage: python analysedir.py [dir] [snapbase] [options] [outputdir]")

inputdir = sys.argv[1]
if inputdir[-1] != "/":
     inputdir += "/"
snapbase = sys.argv[2]
options = sys.argv[3]
outputdir = sys.argv[4]
if outputdir[-1] != "/":
     outputdir += "/"

filelist = os.listdir(inputdir)

print "Analysing approx. "+str(len(filelist))+" files."
sys.stdout.write( "[" )
sys.stdout.flush()
count = -1

for file in filelist:

     count += 1
     sys.stdout.write( str(count)+"," )
     sys.stdout.flush()

     if( file[:len(snapbase)] != snapbase ):
          continue

     number = int(file[-3:]) # This line checks if it's an actual nummber
     number = file[-3:]       # but we really want the 3-digit version as a string
     commands.getoutput("./SnapToTable "+
                        inputdir+file+" "+
                        outputdir+"snapshot"+number+".txt"+" "+
                        options)
     # print "Running ./SnapToTable " + inputdir+file+" "+outputdir+"snapshot"+number+".txt"+" "+options

sys.stdout.write("]\n")
