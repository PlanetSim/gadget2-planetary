#!/usr/bin/env python

import sys


#
# vapourised.py
# Takes a l-v phase boundary curve ("bell curve") and a table of data to be used
# to analyse phase and returns a table of particle ids & the fraction by which
# they are vapourised or "null" if they remain super-critical.
#
# "Bell curve" input table must be of the curve in S-T space with first column
# S and second T, must to a sensible degree vaugely resemble a bell curve in
# order for the lever rule to be used. Points on the curve must be in the correct
# order (generally low-to-high-S, but it can double back on itself so long as
# order is preserved. If gnuplot plots it with lines correctly, then it'll work.
#
# The "inputtable" must also be an ascii table suitable for plotting with gnu
# plot. First line must be a line of column headers starting with a "#" character.
# Must have columns named "Id", "Material", "P", "T", "S" and "Bound" with the
# obvious data in the corresponding columns. These columns can come in any order
# and with any other columns present in the table.
#
# Note since the phase curve will only apply to one material an integer in the
# parameter file needs to specify the material, only those materials that match
# this in the input table will be considered.
#



def read_params( file ):
    try:
        fp = open( file, "r" )
    except:
        sys.exit( "Unable to read parameter file: "+file )

    fp.readline()
    nomats = int(fp.readline().strip())
    bellfiles = []
    for i in range( nomats ):
        fp.readline()
        bellfiles.append( fp.readline().strip() )
    critpoints = []
    for i in range( nomats ):
        fp.readline()
        values = fp.readline().strip().split(",")
        critpoints.append( (float(values[0]),float(values[1]),float(values[2])) )
    fp.readline()
    inputtable = fp.readline().strip()
    fp.readline()
    outfile = fp.readline().strip()

    fp.close()

    return  bellfiles, critpoints, inputtable, outfile



def get_bell_curve( file ):
    try:
        fp = open( file, "r" )
    except:
        sys.exit( "Could not open bell curve file: "+file )
    bellcurve = []
    while 1:
        line = fp.readline()
        if not line:
            break
        if line[0] == "#":
            continue
        values = line.split()
        bellcurve.append( (float(values[0]), float(values[1])) )
    fp.close()
    return bellcurve



def open_table( file ):
    try:
        fp = open( file, "r" )
    except:
        sys.exit( "Could not open input table file: "+file )
    thermdata = []
    headerline = fp.readline()
    headerline = headerline[1:] #lose the opening hash
    headers = headerline.split()
    scol = tcol = idcol = matcol = pcol = -1
    count = 0
    for h in headers:
        if h == "id":
            idcol = count
        elif h == "S":
            scol = count
        elif h == "T":
            tcol = count
        elif h == "material":
            matcol = count
        elif h == "P":
            pcol = count
        count += 1
    if scol == -1 or tcol == -1 or idcol == -1 or matcol == -1 or pcol == -1:
        sys.exit( "Invalid table: "+file )
    while 1:
        line = fp.readline()
        if not line:
            break
        values = line.split()
        thermdata.append( ( int(values[idcol]), \
                            int(values[matcol]),\
                            float(values[scol]),\
                            float(values[tcol]),\
                            float(values[pcol]) ) )
    fp.close()
    return thermdata




def get_intersections( bc, T ):
    #get the s values of the bell curve that span the left intersection
    leftlowindex = lefthighindex = 0
    for i in range(len(bc)):
        t = bc[i][1]
        if i == 0:
            if t > T:
                leftlowindex = 0
                lefthighlindex = 1
                break
            else:
                continue
        if t > T or i == (len(bc)-1):
            leftlowindex = i-1
            lefthighindex = i
            break
        
    #interpolate to find the left hand intersection s value
    lefts = bc[leftlowindex][0] + \
            (bc[lefthighindex][0] - bc[leftlowindex][0])/(bc[lefthighindex][1] - bc[leftlowindex][1]) * \
            (T - bc[leftlowindex][1])

    #do similarly to find the right hand intersection s value
    rightlowindex = righthighindex = 0
    for i in range(lefthighindex,len(bc)):
        t = bc[i][1]
        if t < T or i == (len(bc)-1):
            rightlowindex = i-1
            righthighindex = i
            break
    rights = bc[rightlowindex][0] + \
             (bc[righthighindex][0] - bc[rightlowindex][0])/(bc[righthighindex][1] - bc[rightlowindex][1]) * \
             (T - bc[rightlowindex][1])

    return lefts, rights



def get_mass_fractions( bcs, td, cps ):
    output = [ "#Id\tMassFractionVapour\n" ]
    for t in td: #t is a tuple (id,mat,s,t,p)
        s_c, t_c, p_c = cps[t[1]-1]
        if t[3] > t_c and t[4] > p_c:
            #we have super-critical
            f = -1
        else:
            #we have some sub-critical phase or mixture thereof
            lefts, rights = get_intersections( bcs[t[1]-1], t[3] )
            if t[2] < lefts:
                f = 0
            elif t[2] > rights:
                f = 1
            else:
                f = (t[2]-lefts)/(rights-lefts)
        output.append( str(t[0])+"\t"+str(f)+"\n" )
    return output




def main():
    if len(sys.argv) != 2:
        sys.exit( "Usage: ./vapourised.py <parameter file>" )
    paramfile = sys.argv[1]
    bellfiles, critpoints, inputtable, outfile = read_params( paramfile )
    bellcurves = []
    for i in range( len(bellfiles) ):
        bellcurves.append( get_bell_curve( bellfiles[i] ) )
    thermdata = open_table( inputtable )
    output = get_mass_fractions( bellcurves, thermdata, critpoints )
    try:
        fp = open( outfile, "w" )
    except:
        sys.exit( "Could not save to file: "+outfile )
    fp.writelines( output )
    fp.close()
    




if __name__=="__main__":
    main()
