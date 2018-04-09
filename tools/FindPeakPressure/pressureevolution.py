#!/usr/bin/env python

import sys
import os

#
# pressureevolution.py
# Given a list of particle ids creates a table of their pressure evolution
# over a list of snapshot files
#

#list of particle ids to be tracked
#ids = [200092932, 200067035, 200040135, 200019649, 200008895,\
#       36712,     20628,     9797,      2314,      243,\
#       12,        451,       2632,      8123,      14460,\
#       36628,     200006767, 200020554, 200037546, 200055197,\
#       200078312,\
#       200154999, 147304,    146461,    200154396]

#check arguments
if len(sys.argv) != 4:
    sys.exit( "Usage: python pressureevolution.py [input dir] [output dir] [tracer ids file]" )

#get arguments
inputdir = sys.argv[1]
outputdir = sys.argv[2]
tracerfile = sys.argv[3]

#load in tracer id list
ids = []
TFILE = open( tracerfile, "r" )
while 1:
    line = TFILE.readline()
    if not line:
        break
    ids.append( int(line) )

pressurelines = [] #holds the lines to be written to the output file
entropylines = []

#store the header line
headerline = "#Snapnum"
for id in ids:
    headerline += "\tID:"+str(id)
pressurelines.append( headerline )
entropylines.append( headerline )

#get the input directory
inputfilelist = os.listdir(inputdir)
if inputdir[:-1] != "/":
    inputdir += "/"

print "Getting "+str(len(ids))+" particles from "+str(len(inputfilelist))+" files."

filenum = 0
sys.stdout.write("[")
sys.stdout.flush()
#loop over input files
for file in inputfilelist:
    FILE = open( inputdir + file, "r" )
    header = FILE.readline()
    headings = header.split("\t")
    headings[0] = (headings[0])[1:] #remove hash character
    #get the columns for id and pressure
    idcol = pcol = scol = -1
    count = 0
    for heading in headings:
        if heading == 'P':
            pcol = count
        elif heading == 'S':
            scol = count
        elif heading == 'id':
            idcol = count
        count += 1
    if idcol == -1 or pcol == -1 or scol == -1:
        sys.exit( "Invalid table: "+file )

    #get the pressures & entropies for each id
    pressures = [-1]*len(ids)
    entropies = [-1]*len(ids)
    while 1:
        fileline = FILE.readline()
        if not fileline:
            break
        line = fileline.split('\t')
        count = 0
        for id in ids:
            if line[idcol] == str(id):
                pressures[count] = line[pcol]
                entropies[count] = line[scol]
                break
            count += 1
    FILE.close()
    count = 0
    #check all particles are accounted for
    for p in pressures:
        if p == -1:
            sys.exit( "File "+file+" did not contain the id "+str(ids[count]) )
        count += 1
    #write an output line
    snapnum = int( file[-7:-4] )
    poutputline = soutputline = str(snapnum)
    for p in pressures:
        poutputline += "\t"+str(p)
    for s in entropies:
        soutputline += "\t"+str(s)
    pressurelines.append( poutputline+"\n" )
    entropylines.append( soutputline+"\n" )
    #status
    sys.stdout.write(str(filenum)+","),
    sys.stdout.flush()
    filenum += 1
sys.stdout.write("]\n");
sys.stdout.flush()

    
if outputdir[:-1] != '/':
    outputdir += '/'
OUTFILE = open( outputdir+"pressureevolution.txt", "w" )
OUTFILE.writelines( pressurelines )
OUTFILE.close()
OUTFILE = open( outputdir+"entropyevolution.txt", "w" )
OUTFILE.writelines( entropylines )
OUTFILE.close()
