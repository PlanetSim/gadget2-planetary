#!/usr/bin/env python

import sys
import commands


def load_eos_table( file ):
    fp = open( file, "r" )
    if not fp:
        exit( "Could not find EOS table "+file )
    data = fp.read().split()
    data.reverse()
    fp.close()
    nRho = int(data.pop())
    nS = int(data.pop())
    Rho = []
    S = []
    P = []
    T = []
    U = []
    C = []
    for i in range( nRho ):
        Rho.append( float(data.pop()) )
    for i in range( nS ):
        S.append( float(data.pop()) )
    for i in range( nS ):
        P.append( [] )
        for j in range( nRho ):
            P[i].append( float(data.pop()) )
    for i in range( nS ):
        T.append( [] )
        for j in range( nRho ):
            T[i].append( float(data.pop()) )
    for i in range( nS ):
        U.append( [] )
        for j in range( nRho ):
            U[i].append( float(data.pop()) )
    for i in range( nS ):
        C.append( [] )
        for j in range( nRho ):
            C[i].append( float(data.pop()) )

    return nRho, nS, Rho, S, P, T, U, C




def save_table( file, isobars, S, pressures, nS ):
    indexes = []
    onebarindex = 0
    skip = 1
    outputlines = []
    fp = open( file, "w" )
    if not fp:
        exit( "Could not open table for writing: "+file )
    for i in range(len(pressures)):
        if pressures[i] <= _MAXPRESSURE:
            indexes.append(i)
        if abs(pressures[i] - 1e6) < abs(pressures[onebarindex] - 1e6):
            onebarindex = i
    if _NOISOBARS == 0:
        skip = 1
    else:
        skip = int(round(len(indexes)/_NOISOBARS))
        if skip == 0:
            skip = 1
    outputlines.append( "#Entropy" )
    for i in range( 0, len(indexes), skip ):
        outputlines[0] += "\tP"+str(pressures[indexes[i]])
    outputlines[0] += "\t*P"+str(pressures[onebarindex])
    for i in range( nS ):
        outputlines[i] += "\n"
        outputlines.append( str(S[i]) )
        for j in range( 0, len(indexes), skip ):
            outputlines[i+1] += "\t" + str(isobars[indexes[j]][i])
        outputlines[i+1] += "\t" + str(isobars[onebarindex][i])
    fp.writelines(outputlines)
    fp.close()




def find_isobars( infile, outfile ):
    print( "+ load EOS table: "+infile )
    nRho, nS, Rho, S, P, T, U, C = load_eos_table( infile )
    isobars = []
    pressures = []
    for S_0 in range( 0, nS, Sskip ):
        for i in range( nRho ):
            pressure = P[S_0][i]
            pressures.append( pressure )
            isobars.append( [] )
            for j in range( nS ):
                if j == 0:
                    #first one is trivial since we have exact pressure
                    isobars[-1].append( T[S_0][i] )
                else:
                    #the general case
                    k = 0
                    while (k < nRho) and (P[j][k] < pressure):
                        k += 1
                    if k == nRho:
                        #pressure is greater than values in this line of the table
                        #therefore extrapolate
                        deltaT = T[j][k-1] - T[j][k-2]
                        deltaP = P[j][k-1] - P[j][k-2]
                        if deltaP == 0:
                            isobars[-1].append( float('nan') )
                        else:
                            isobars[-1].append( T[j][k-1] + \
                                               (deltaT/deltaP) * \
                                               (pressure - P[j][k-1]) )
                    elif k == 0:
                        #pressure is less than values in this line of the table
                        #therefore extrapolate
                        deltaT = T[j][1] - T[j][0]
                        deltaP = P[j][1] - P[j][0]
                        if deltaP == 0:
                            isobars[-1].append( float('nan') )
                        else:
                            isobars[-1].append( T[j][0] - \
                                               (deltaT/deltaP)*(P[j][0] - pressure) )
                    else:
                        #interpolate
                        deltaT = T[j][k] - T[j][k-1]
                        deltaP = P[j][k] - P[j][k-1]
                        if deltaP == 0:
                            isobars[-1].append( float('nan') )
                        else:
                            isobars[-1].append( T[j][k-1] + \
                                               (deltaT/deltaP)*(pressure - P[j][k-1]) )
    print( "Saving output table to: "+outfile )
    save_table( outfile, isobars, S, pressures, nS )
    return isobars, S



def plot_isobars( infile, outfile, peaks, peaksfile ):
    onebar = False
    INFILE = open( infile, "r" )
    headers = INFILE.readline().split('\t')
    headers.pop(0) #remove entropy header
    INFILE.close()
    if headers[-1][0] == "*":
        onebar = True
        headers[-1] = headers[-1][1:]
    pressures = []
    for h in headers:
        pressures.append( float(h[1:]) )
    filedefault[1] = "set output \'"+outfile+"\'\n"
    if not onebar:
        filedefault[2] = filedefault[2][:-2] + "\\nIsobars running from "+str(pressures[0])+" to "+str(pressures[-1])+" dyn/cm^2.\"\n"
    else:
        filedefault[2] = filedefault[2][:-2] + "\\nIsobars running from "+str(pressures[0])+" to "+str(pressures[-2])+" dyn/cm^2.\"\n"
    count = 2
    for p in pressures:
        filedefault.append( "\'"+infile+"\' u 1:"+str(count)+" w lines title \'\' ls 1,\\\n" )
        count += 1
    filedefault[-1] = filedefault[-1][:-3]
    if onebar:
        filedefault[-1] = filedefault[-1][:-6] + "P = "+str(pressures[-1])+"dyn/cm^2.\' ls 2,\\\n"
    if peaks:
        filedefault.append( "\'" + peaksfile + "\' u 1:2 w points title \'\'\n" )
    else:
        filedefault[-1] = filedefault[-1][:-3]                                       
    OUTFILE = open( gnuplotfile, "w" )
    OUTFILE.writelines( filedefault )
    OUTFILE.close()
    print commands.getoutput( "gnuplot "+gnuplotfile )
    if delfiles:
        commands.getoutput( "rm "+gnuplotfile )
        commands.getoutput( "rm "+peaksfile )



def find_peak_d2TdS2( isobars, S, file ):
    peaks = []
    peaks.append( "#S\tT\tIndex\n" )
    for i in range(len(isobars)):
        peakpS = 0
        peakpT = 0
        peakpd2TdS2 = 0
        peaknS = 0
        peaknT = 0
        peaknd2TdS2 = 0
        prevdTdS = 0
        dTdS = 0
        for j in range(len(isobars[i])-1):
            prevdTdS = dTdS
            if j == 0:
                continue
            dTdS = (isobars[i][j+1]-isobars[i][j])/(S[j+1]-S[j])
            d2TdS2 = (dTdS-prevdTdS)/(S[j+1]-S[j])
            #check if in plotting range
            if S[j+1] > smax or S[j] < smin or isobars[i][j+1] > tmax or isobars[i][j] < tmin:
                continue
            if d2TdS2 > peakpd2TdS2:
                peakpd2TdS2 = d2TdS2
                peakpS = S[j]
                peakpT = isobars[i][j]
            if d2TdS2 < peaknd2TdS2:
                peaknd2TdS2 = d2TdS2
                peaknS = S[j]
                peaknT = isobars[i][j]
        peaks.append( str(peakpS) + "\t" + str(peakpT) + "\t" + str(i) + "\n" )
        peaks.append( str(peaknS) + "\t" + str(peaknT) + "\t" + str(i) + "\n" )
            
    FILE = open( file, "w" )
    FILE.writelines(peaks)
    FILE.close()




def readparams( file ):
    global gnuplotfile, peaksfilename, _NOISOBARS, _MAXPRESSURE, Sskip, smin, smax, tmin, tmax, filedefault, delfiles
    
    fp = open( file, "r" )
    if not fp:
        sys.exit( "Could not read parameter file "+file )
        
    fp.readline()
    eosfilename = fp.readline().strip()
    fp.readline()
    tablefilename = fp.readline().strip()
    fp.readline()
    pngfilename = fp.readline().strip()
    fp.readline()
    peaks = int(fp.readline().strip())
    fp.readline()
    gnuplotfile = fp.readline().strip()
    fp.readline()
    peaksfilename = fp.readline().strip()
    fp.readline()
    smin = float(fp.readline().strip())
    fp.readline()
    smax = float(fp.readline().strip())
    fp.readline()
    tmin = float(fp.readline().strip())
    fp.readline()
    tmax = float(fp.readline().strip())
    fp.readline()
    _NOISOBARS = int(fp.readline().strip())
    fp.readline()
    _MAXPRESSURE = float(fp.readline().strip())
    fp.readline()
    Sskip = int(fp.readline().strip())
    fp.readline()
    delfiles = int(fp.readline().strip())
    fp.readline()
    materialname = fp.readline().strip()
    
    fp.close()

    filedefault = [ "set terminal png nocrop size 768,576\n",
                    "", #for the "set output" statement
                    "set title \"Isobaric phase diagram for "+materialname+".\"\n",
                    "set xlabel \"Entropy /(erg/gK)\"\n",
                    "set ylabel \"Temperature /K\"\n",
                    "set xrange ["+str(smin)+":"+str(smax)+"]\n",
                    "set yrange ["+str(tmin)+":"+str(tmax)+"]\n",
                    "set style line 1 lt 1 lw 1\n",
                    "set style line 2 lt 3 lw 2\n",
                    "plot " ]

    return eosfilename, tablefilename, pngfilename, peaks



def main():
    if len(sys.argv) != 2:
        sys.exit( "Usage: python phasediagram.py <parameter file>" )
    paramfile = sys.argv[1]
    eosfilename, tablefilename, pngfilename, peaks = readparams( paramfile )
    print( "FIND ISOBARS in "+eosfilename )
    isobars, S = find_isobars( eosfilename, tablefilename )
    if peaks:
        find_peak_d2TdS2( isobars, S, peaksfilename )
    else:
        commands.getoutput( "rm "+peaksfilename )
    print( "PLOT ISOBARS in "+tablefilename )
    plot_isobars( tablefilename, pngfilename, peaks, peaksfilename )
    commands.getoutput( "eog "+pngfilename )




if __name__ == "__main__":
    main()
