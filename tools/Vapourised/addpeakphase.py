#!/usr/bin/env python

import sys
import os

#
# addvapourised.py
# Uses largely the same code as vapourised.py but runs over a directory of input tables and adds
# the fraction vapourised column to the end of those tables.
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
    inputfile = fp.readline().strip()
    
    fp.close()

    return  bellfiles, critpoints, inputfile



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
    tablelines = []
    thermdata = []
    headerline = fp.readline()
    tablelines.append(headerline)
    headerline = headerline[1:] #lose the opening hash
    headers = headerline.split()
    scol = tcol = idcol = matcol = pcol = -1
    count = 0
    for h in headers:
        if h == "Id":
            idcol = count
        elif h == "EntropyAtPeakPressure":
            scol = count
        elif h == "TemperatureAtPeakPressure":
            tcol = count
        elif h == "Material":
            matcol = count
        elif h == "PeakPressure":
            pcol = count
        count += 1
    if scol == -1 or tcol == -1 or idcol == -1 or matcol == -1 or pcol == -1:
        sys.exit( "Invalid table: "+file )
    while 1:
        line = fp.readline()
        tablelines.append(line)
        if not line:
            break
        values = line.split()
        thermdata.append( ( int(values[idcol]), \
                            int(values[matcol]),\
                            float(values[scol]),\
                            float(values[tcol]),\
                            float(values[pcol]) ) )
    fp.close()
    return thermdata, tablelines




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



def add_mass_fractions( bcs, td, cps, tablelines, file ):
    count = 1
    tablelines[0] = tablelines[0][:-1] + "\tMassFractionVapourised\n"
    for t in td: #t is a tuple (id,mat,s,t,p)
        s_c, t_c, p_c = cps[t[1]-1]
        if t[3] > t_c and t[4] > p_c:
            #we have supercritical
            f = -1
        else:
            #we have a sub-critical phase
            lefts, rights = get_intersections( bcs[t[1]-1], t[3] )
            if t[2] < lefts:
                f = 0
            elif t[2] > rights:
                f = 1
            else:
                f = (t[2]-lefts)/(rights-lefts)
        tablelines[count] = tablelines[count][:-1] + "\t" + str(f) + "\n"
        count += 1
    try:
        fp = open( file, "w" )
    except:
        sys.exit( "Could not open file for writing " + file )
    fp.writelines( tablelines )
    fp.close()





def main():
    if len(sys.argv) != 2:
        sys.exit( "Usage: ./addvapourised.py <parameter file>" )
    paramfile = sys.argv[1]
    bellfiles, critpoints, inputfile = read_params( paramfile )
    bellcurves = []
    for i in range( len(bellfiles) ):
        bellcurves.append( get_bell_curve( bellfiles[i] ) )
    thermdata, tablelines = open_table( inputfile )
    add_mass_fractions( bellcurves, thermdata, critpoints, tablelines, inputfile )



if __name__=="__main__":
    main()
