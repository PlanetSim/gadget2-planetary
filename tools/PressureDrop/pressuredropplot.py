#!/usr/bin/env python

import sys
import commands


def create_plot_file( infile, outfile, peaks, peaksfile ):
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



def readparams( file ):
    global gnuplotfile, filedefault, delfiles
    
    fp = open( file, "r" )
    if not fp:
        sys.exit( "Could not read parameter file "+file )
        
    fp.readline()
    evolutiontable = fp.readline().strip()
    fp.readline()
    droptable = fp.readline().strip()
    fp.readline()
    pngfilename = fp.readline().strip()
    fp.readline()
    gnuplotfile = fp.readline().strip()
    fp.readline()
    delfiles = int(fp.readline().strip())
    
    fp.close()

    filedefault = [ "set terminal png nocrop size 768,576\n",
                    "", #for the "set output" statement
                    "set title \"Tracer particle pressure evolution matched to the .\"\n",
                    "set xlabel \"Entropy /(erg/gK)\"\n",
                    "set ylabel \"Temperature /K\"\n",
                    "set xrange ["+str(smin)+":"+str(smax)+"]\n",
                    "set yrange ["+str(tmin)+":"+str(tmax)+"]\n",
                    "set style line 1 lt 1 lw 1\n",
                    "set style line 2 lt 3 lw 2\n",
                    "plot " ]

    return evolutiontable, droptable, pngfilename



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
