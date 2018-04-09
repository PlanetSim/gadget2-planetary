#!/usr/bin/env python

#
# Uses jystilts to plot a cross-section of all of the particles for each
# of a directory full of snapshots
#
# Assumes input files are formatted *0.txt where "0" is the snapshot number
#

import stilts
from stilts import plot2d
import sys
import os

width = "2e8"
size = "3e9"
pixsize = "1024"


def read_params( file ):
    global inputdir, outputdir, width, size, pixsize
    try:
        fp = open( file, "r" )
    except:
        sys.exit( "Could not open paramter file for reading: "+file )
    fp.readline()
    inputdir = fp.readline().strip()
    if not (inputdir[-1] == "/"):
        inputdir += "/"
    fp.readline()
    outputdir = fp.readline().strip()
    if not( outputdir[-1] == "/" ):
        outputdir += "/"
    fp.readline()
    width = str(float(fp.readline().strip()))
    fp.readline()
    size = str(float(fp.readline().strip()))
    fp.readline()
    pixsize = str(int(fp.readline().strip()))
    fp.close()



def main():
    global inputdir, outputdir, width, size, pixsize

    if( len(sys.argv) != 2 ):
        sys.exit("Usage: jystilts phaseplot.py <parameter file>")

    read_params( sys.argv[1] )

    inputfilelist = os.listdir(inputdir)

    print "Creating images for "+str(len(inputfilelist))+" files."
    sys.stdout.write( "[" )
    sys.stdout.flush()
    count = -1

    for file in inputfilelist:
        count += 1
        sys.stdout.write( str(count)+"," )
        sys.stdout.flush()
    
        table = stilts.tread(inputdir+file, "ascii")
        
        sliceFenSC = table.cmd_select("z<("+width+"/2) && z>(-"+width+"/2) && material == 1 && MassFractionVapourised != -1")
        sliceMlnSC = table.cmd_select("z<("+width+"/2) && z>(-"+width+"/2) && material == 2 && MassFractionVapourised != -1")
        sliceFeSC = table.cmd_select("z<("+width+"/2) && z>(-"+width+"/2) && material == 1 && MassFractionVapourised == -1")
        sliceMlSC = table.cmd_select("z<("+width+"/2) && z>(-"+width+"/2) && material == 2 && MassFractionVapourised == -1")

        snapnum = file[-7:-4]

        plot2d(title="Snapshot "+snapnum, fontsize=20,
               in_1=sliceFenSC, xdata_1="x", ydata_1="y", size_1=2, shape_1="filled_circle",
               in_2=sliceMlnSC, xdata_2="x", ydata_2="y", size_2=2, shape_2="open_circle",
               in_3=sliceFeSC, xdata_3="x", ydata_3="y", size_3=2, shape_3="filled_circle", colour_3="magenta",
               in_4=sliceMlSC, xdata_4="x", ydata_4="y", size_4=2, shape_4="open_circle", colour_4="magenta",
               xhi=size, yhi=size, xlo="-"+size, ylo="-"+size, xpix=pixsize, ypix=pixsize,
               auxdata_1="MassFractionVapourised", auxlog="false", auxshader="colour",
               auxdata_2="MassFractionVapourised", auxhi=1, auxlo=0,
               grid="false", legend="false", xlabel="x /cm", ylabel="y /cm",
               out=outputdir+file[:-3]+"png", ofmt="png")

    sys.stdout.write("]\n")



if __name__ == "__main__":
    main()
