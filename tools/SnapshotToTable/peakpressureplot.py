#!/usr/bin/python

import stilts
from stilts import plot2d
import sys
import os

width = "3e7"
size = "6e8"
pixsize = "1024"


if( len(sys.argv) != 6 ):
    sys.exit("Usage: jystilts pressureplot.py [input file] [output file] [sectionwidth] [plotsize] [plotpxsize]")


inputfile = sys.argv[1]
outputfile = sys.argv[2]
width = str(float( sys.argv[3] ))
size = str(float( sys.argv[4] ))
pxsize = str(int( sys.argv[5] ))

slice = stilts.tread(inputfile, "ascii").cmd_select("z<("+width+"/2)").cmd_select("z>(-"+width+"/2)")

plot2d(in_=slice, xdata_="x", ydata_="y",
       xhi=size, yhi=size, xlo="-"+size, ylo="-"+size, xpix=pixsize, ypix=pixsize,
       xerror_="0.15*HSML", yerror_="0.15*HSML", errstyle_="filled_ellipse",
       auxdata_="PeakPressure", auxlog="false", auxshader="rainbow",
       grid="false", legend="false", xlabel="x /cm", ylabel="y /cm",
       out=outputfile, ofmt=outputfile[:-3])

