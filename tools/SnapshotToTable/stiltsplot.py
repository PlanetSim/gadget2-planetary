#!/usr/bin/python

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
    print "Trying to read parameters from ", file
    try:
        fp = open( file, "r" )
    except:
        sys.exit( "Could not open parameter file: "+file )

    print "Opened parameter file"
    fp.readline()
    inputdir = fp.readline().strip()
    if not( inputdir[-1] == "/" ):
        inputdir += "/"
    print "Input directory is ", inputdir
    fp.readline()
    outputdir = fp.readline().strip()
    print "Output dir is ", outputdir
    if not( outputdir[-1] == "/" ):
        outputdir += "/"
    print "Read second line of parameter file"
    fp.readline()
    width = str(float(fp.readline().strip()))
    print "Width is ", width
    print "Read width in parameter file"
    fp.readline()
    size = str(float(fp.readline().strip()))
    print "Size is ", size
    fp.readline()
    pixsize = str(int(fp.readline().strip()))
    print "Pixsize is ", pixsize
    fp.close()


def main():
    global inputdir, outputdir, width, size, pixsize
    print "Checking usage"
    if( len(sys.argv) != 2 ):
        sys.exit("Usage: jystilts stiltsplot.py <parameter file>")
        
    print "Reading parameters"

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

        print "Trying to read " + inputdir + file
    
        table = stilts.tread(inputdir+file, "ascii")

        slice = table.cmd_select("z<("+width+"/2)").cmd_select("z>(-"+width+"/2)")

        core = slice.cmd_select("material==1");
        mantle = slice.cmd_select("material==2");

        plot2d(in_=slice, xdata_="x", ydata_="y",
               xhi=size, yhi=size, xlo="-"+size, ylo="-"+size, xpix=pixsize, ypix=pixsize,
               subset_1="material==1", name_1="Core", colour_1="red", size_1="1",
               subset_2="material==2", name_2="Mantle", colour_2="green", size_2="1",
               grid="false", legend="false", xlabel="x /cm", ylabel="y /cm",
               out=outputdir+file[:-3]+"png", ofmt="png")

    sys.stdout.write("]\n")



if __name__=="__main__":
    main()
