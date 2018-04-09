#!/usr/bin/env python

import sys
import os
import getopt

#
# tracerevolutions.py
# Given a list of particle ids creates tables evolving particle properties
# over a directory of snapshot files
#

#holds whether a file for each variable is required
variables = { "S":0,
              "P":0,
              "T":0 }
#converts between keys above and headings used in snapshot tables
snapheadings = { "S": "S",
                 "P": "P",
                 "T": "T" }



def load_tracers( file ):
    #load in tracer id list
    ids = []
    TFILE = open( file, "r" )
    while 1:
        line = TFILE.readline()
        if not line:
            break
        ids.append( int(line) )
    return ids




def main():
    #check arguments
    if len(sys.argv) != 5:
        sys.exit( "Usage: ./tracerevolutions.py [options] [input dir] [output dir] [tracer ids file]\n\
        Options:\n\
        -S Entropy\n\
        -P Pressure\n\
        -T Temperature\n\
        -A All" )

    #get arguments
    options = sys.argv[1]
    inputdir = sys.argv[2]
    outputdir = sys.argv[3]
    tracerfile = sys.argv[4]
    if inputdir[-1] != '/':
        inputdir += '/'
    if outputdir[-1] != '/':
        outputdir += '/'
    inputfilelist = os.listdir(inputdir)


    #parse options
    if options[0] == '-':
        options = options[1:]
    for o in options:
        if o in variables:
            variables[o] = 1
        if o == 'A':
            for k in variables:
                variables[k] = 1
            break

    tracerids = load_tracers( tracerfile )
     
    for k in variables:
        if variables[k] == 1:
            get_evolution( k, tracerids, inputfilelist, inputdir, outputdir )
             


def get_evolution( variable, ids, inputfilelist, inputdir, outputdir ):
    lines = [] #holds the lines to be written to the output file

    #store the header line
    headerline = "#Snapnum"
    for id in ids:
        headerline += "\t"+str(id)
    lines.append( headerline+"\n" )

    print variable+": Getting "+str(len(ids))+" particles from "+str(len(inputfilelist))+" files."

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
        idcol = vcol = -1
        count = 0
        for heading in headings:
            if heading == snapheadings[variable]:
                vcol = count
            elif heading == 'id':
                idcol = count
            count += 1
        if idcol == -1 or vcol == -1:
            sys.exit( "Invalid table: "+file )

        #get the values
        values = [-1]*len(ids)
        while 1:
            fileline = FILE.readline()
            if not fileline:
                break
            line = fileline.split()
            count = 0
            for id in ids:
                if line[idcol] == str(id):
                    values[count] = line[vcol]
                    break
                count += 1
        FILE.close()
        #check all particles are accounted for
        for v in values:
            if v == -1:
                sys.exit( "File "+file+" did not contain the id "+str(ids[count]) )
        #write an output line
        snapnum = int( file[-7:-4] )
        outputline = str(snapnum)
        for v in values:
            outputline += "\t"+str(v)
        lines.append( outputline+"\n" )
        #status
        sys.stdout.write(str(filenum)+","),
        sys.stdout.flush()
        filenum += 1
    sys.stdout.write("]\n");
    sys.stdout.flush()
    
    OUTFILE = open( outputdir+variable+"evolution.txt", "w" )
    OUTFILE.writelines( lines )
    OUTFILE.close()

if __name__ == "__main__":
    main()
