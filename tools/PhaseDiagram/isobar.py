#!/usr/bin/env python

import sys
import phasediagram


def get_isobar( pressure, Rho, S, P, T ):
    output = [] #list of tuples (S,T,Rho,P) in cgs along the isobar
    nRho = len(Rho)
    #loop over all Ss, for each find the Rho that corresponds to a P of "pressure",
    #then find the corresponding temperature. This involves interpolation between
    #bounding points of density
    for i in range( len(S) ):
        #find bounding indices of density for pressure
        j_lo = j_hi = 0
        while( j_hi < nRho and P[i][j_hi] < pressure ):
            j_hi += 1
        if j_hi == 0:
            j_hi = 1
        else:
            if j_hi == nRho:
                j_hi -= 1
            j_lo = j_hi-1
        #interpolate to find temperature
        deltaP = P[i][j_hi] - P[i][j_lo]
        if deltaP == 0:
            t = rho = p = float("nan")
        else:
            deltaT = T[i][j_hi] - T[i][j_lo]
            deltaRho = Rho[j_hi] - Rho[j_lo]
            deltaP2 = P[i][j_hi] - P[i][j_lo]
            t = T[i][j_lo] + (deltaT/deltaP)*( pressure - P[i][j_lo] )
            rho = Rho[j_lo] + (deltaRho/deltaP)*( pressure - P[i][j_lo] )
            p = P[i][j_lo] + (deltaP2/deltaP)*( pressure - P[i][j_lo] )
        output.append( (S[i], t, rho, p) )
        
    return output

def main():
    if len(sys.argv) != 4:
        sys.exit( "Usage: ./onebarisobar.py <input eos> <pressure> <output file>" )

    inputfile = sys.argv[1]
    pressure = float(sys.argv[2])
    outputfile = sys.argv[3]

    nRho, nS, Rho, S, P, T, U, C = phasediagram.load_eos_table( inputfile )

    output = get_isobar( pressure, Rho, S, P, T )

    outputlines = [ "#S\tT\tRho\tP\n" ]

    for o in output:
        outputlines.append( str(o[0])+"\t"+str(o[1])+"\t"+str(o[2])+"\t"+str(o[3])+"\n" )

    try:
        fp = open( outputfile, "w" )
    except:
        sys.exit( "Could not open output file for writing: "+outputfile )
    fp.writelines( outputlines )
    fp.close()
    


if __name__ == "__main__":
    main()
