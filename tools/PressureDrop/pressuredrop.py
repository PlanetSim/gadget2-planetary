#!/usr/bin/env python

import sys
import os

#
# pressuredrop.py
# Takes a directory of tables representing Gadget2 snapshots
# and outputs a table listing, for each particle, the
# snapshot number at which the pressure drops to 1/10th of
# its maximum
#



def read_params( file ):
    try:
        fp = open( file, "r" )
    except:
        sys.exit( "Could not open parameter file for reading: "+file )

    fp.readline()
    indir = fp.readline().strip()
    if indir[-1] != "/":
        indir += "/"
    fp.readline()
    outfile = fp.readline().strip()
    fp.readline()
    snapbase = fp.readline().strip()
    fp.readline()
    facdropto = float(fp.readline().strip())

    fp.close()
    return indir, outfile, snapbase, facdropto



def read_table( file ):
    try:
        fp = open( file, "r" )
    except:
        sys.exit( "Could not open table: "+file )
    pcol = idcol = -1
    headerline = fp.readline()
    headerline = headerline[1:]
    headerlist = headerline.split()
    count = 0
    for h in headerlist:
        if h == "P":
            pcol = count
        elif h == "id":
            idcol = count
        count += 1
    if pcol == -1 or idcol == -1:
        sys.exit( "Invalid table: "+file )
    ids = []
    ps = []
    while 1:
        line = fp.readline()
        if not line:
            break
        linelist = line.split()
        ids.append( int(linelist[idcol]) )
        ps.append( float(linelist[pcol]) )
    fp.close()
    return ids, ps



def main():

    if not( len(sys.argv) == 2 ):
        sys.exit( "Usage: ./pressuredrop.py <parameter file>" )

    indir, outfile, snapbase, facdropto = read_params( sys.argv[1] )
    
    filelist = os.listdir( indir )

    print "Analysing approx. "+str(len(filelist))+" files."
    sys.stdout.write("[0")
    sys.stdout.flush()
    count = 0

    #Take the first file in the filelist to define dictionaries
    file = filelist.pop(0)
    ids, tmpps = read_table( indir+file )
    peakps = dict(zip(ids,tmpps)) #dictionary of pressures to peak pressures
    dropps = dict(zip(ids,tmpps)) #dictionary of dropped-to pressures
    min1s = [-1] * len(ids)
    snapdrop = dict(zip(ids,min1s))

    for file in filelist:

        count += 1
        sys.stdout.write( ","+str(count) )
        sys.stdout.flush()

        if file[:len(snapbase)] != snapbase:
            continue

        number = int(file[-7:-4]) #assumes formatted as snapbase###.txt

        tmpids, tmpps = read_table( indir+file )

        for i in range(len(tmpids)):
            if tmpps[i] > peakps[tmpids[i]]:
                peakps[tmpids[i]] = tmpps[i]
                #drop pressure is no longer valid
                snapdrop[tmpids[i]] = -1
            elif snapdrop[tmpids[i]] == -1:
                #when we have no drop pressure
                if tmpps[i] <= facdropto*peakps[tmpids[i]]:
                    #we have a new drop pressure
                    dropps[tmpids[i]] = tmpps[i]
                    snapdrop[tmpids[i]] = number

    sys.stdout.write("]\n")
    sys.stdout.flush()

    #turn dictionaries into output list
    output = ["#Id\tPeakPressure\tDropped-toPressure\tDropSnapshot\n"]
    for id in ids:
        oline = str(id)+"\t"+str(peakps[id])+"\t"
        if snapdrop[id] == -1:
            oline += "null\tnull\n"
        else:
            oline += str(dropps[id])+"\t"+str(snapdrop[id])+"\n"
        output.append( oline )

    #save output file
    try:
        fp = open( outfile, "w" )
    except:
        sys.exit( "Could not open file for writing: "+outfile )
    fp.writelines(output)
    fp.close()


if __name__ == "__main__":
    main()
