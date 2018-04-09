#!/usr/bin/env python

#
# plotpressuredrop.py
# takes output of pressuredrop.py and the pressure output of tracer evolutions
# and uses gnuplot to plot them together.
#




def main():
    if not( len(argv) == 3 ):
        sys.exit( "Usage: ./plotpressuredrop.py <pressure drop table> <pressure evolution table>" )
    pdtable = sys.argv[1]
    petable = sys.argv[2]
    
    


if __name__ == "__main__":
    main()
