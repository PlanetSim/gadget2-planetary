#!/usr/bin/env python

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
tracerfile = "tracerparticles.txt"


def read_params( file ):
    global inputdir, outputdir, width, size, pixsize, tracerfile
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
    fp.readline()
    tracerfile = fp.readline().strip()
    fp.close()



def main():
    global inputdir, outputdir, width, size, pixsize, tracerfile

    if( len(sys.argv) != 2 ):
        sys.exit("Usage: jystilts stiltsplot.py <parameter file>")

    read_params( sys.argv[1] )

    tracertable = stilts.tread( tracerfile, "ascii" )

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
        
        slice = table.cmd_select("z<("+width+"/2)").cmd_select("z>(-"+width+"/2)")

        match = stilts.tmatch2( in1=table, in2=tracertable, find="best",
                                values1="id", values2="id", matcher="exact", progress="none" )

        core = slice.cmd_select("material==1");
        mantle = slice.cmd_select("material==2");
        snapnum = file[-7:-4]

        plot2d(title="Snapshot "+snapnum, fontsize=20,
               in_1=slice, xdata_1="x", ydata_1="y",
               in_2=match, xdata_2="x", ydata_2="y", txtlabel_2="num",
               xhi=size, yhi=size, xlo="-"+size, ylo="-"+size, xpix=pixsize, ypix=pixsize,
               subset_11="material==1", name_11="Core1", colour_11="red", size_11=0,
               subset_12="material==2", name_12="Mantle1", colour_12="green", size_12=0,
               subset_21="material==1", name_21="Core2", colour_21="magenta", size_21=3,
               subset_22="material==2", name_22="Mantle2", colour_22="orange", size_22=3,
               grid="false", legend="false", xlabel="x /cm", ylabel="y /cm",
               out=outputdir+file[:-3]+"png", ofmt="png")

    sys.stdout.write("]\n")



if __name__ == "__main__":
    main()
