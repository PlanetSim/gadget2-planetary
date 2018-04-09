#!/usr/bin/env python

import sys

#
# phaseinfo.py
# A script to take a tabular form of a Gadget2 snapshot with columns for
# "material", "MassFractionVapour", "mass" and "Bound" (where Bound column
# is 1s or 0s) & prints global information on the proportions of various
# phases and materials.
# Assumes two materials: 1) Iron, 2) Mantle
#
#



def get_info( file ):
    try:
        fp = open( file, "r" )
    except:
        sys.exit( "Could not open table: "+file )
    header = fp.readline()
    header = header[1:] #lose the hash
    headers = header.split()
    matcol = mfvcol = bcol = mcol = -1
    count = 0
    for h in headers:
        if h == "material":
            matcol = count
        elif h == "MassFractionVapourised":
            mfvcol = count
        elif h == "Bound":
            bcol = count
        elif h == "mass":
            mcol = count
        count += 1
    if matcol == -1 or mfvcol == -1 or bcol == -1 or mcol == -1:
        sys.exit( "Invalid table: "+file )
    total = [0,0,0,0]
    bnd = [0,0,0,0]
    ubnd = [0,0,0,0]
    irn = [0,0,0,0]
    mntl = [0,0,0,0]
    bndirn = [0,0,0,0]
    bndml = [0,0,0,0]
    ubndirn = [0,0,0,0]
    ubndml = [0,0,0,0]
    melt = [0,0,0,0,0,0,0,0]
    vap = [0,0,0,0,0,0,0,0]
    sc = [0,0,0,0,0,0,0,0]
    while 1:
        line = fp.readline()
        if not line:
            break
        cols = line.split()
        m = float(cols[mcol])
        f = float(cols[mfvcol])
        bound = int(cols[bcol])
        mat = int(cols[matcol])
        total[3] += m
        if f == -1:
            #supercritical phase
            total[2] += m
            if bound:
                bnd[2] += m
                bnd[3] += m
                sc[2] += m
                if mat == 1:
                    bndirn[2] += m
                    bndirn[3] += m
                    sc[4] += m
                else:
                    bndml[2] += m
                    bndml[3] += m
                    sc[5] += m
            else:
                ubnd[2] += m
                ubnd[3] += m
                sc[3] += m
                if mat == 1:
                    ubndirn[2] += m
                    ubndirn[3] += m
                    sc[6] += m
                else:
                    ubndml[2] += m
                    ubndml[3] += m
                    sc[7] += m
            if mat == 1:
                irn[2] += m
                irn[3] += m
                sc[0] += m
            else:
                mntl[2] += m
                mntl[3] += m
                sc[1] += m
        else:
            #sub-critical phase
            total[0] += (1-f)*m
            total[1] += f*m
            if bound:
                bnd[0] += (1-f)*m
                bnd[1] += f*m
                bnd[3] += m
                melt[2] += (1-f)*m
                vap[2] += f*m
                if mat == 1:
                    bndirn[0] += (1-f)*m
                    bndirn[1] += f*m
                    bndirn[3] += m
                    melt[4] += (1-f)*m
                    vap[4] += f*m
                else:
                    bndml[0] += (1-f)*m
                    bndml[1] += f*m
                    bndml[3] += m
                    melt[5] += (1-f)*m
                    vap[5] += f*m
            else:
                ubnd[0] += (1-f)*m
                ubnd[1] += f*m
                ubnd[3] += m
                melt[3] += (1-f)*m
                vap[3] += f*m
                if mat == 1:
                    ubndirn[0] += (1-f)*m
                    ubndirn[1] += f*m
                    ubndirn[3] += m
                    melt[6] += (1-f)*m
                    vap[6] += f*m
                else:
                    ubndml[0] += (1-f)*m
                    ubndml[1] += f*m
                    ubndml[3] += m
                    melt[7] += (1-f)*m
                    vap[7] += f*m
            if mat == 1:
                irn[0] += (1-f)*m
                irn[1] += f*m
                irn[3] += m
                melt[0] += (1-f)*m
                vap[0] += f*m
            else:
                mntl[0] += (1-f)*m
                mntl[1] += f*m
                mntl[3] += m
                melt[1] += (1-f)*m
                vap[1] += f*m

    #normalise agaisnt masses
    melt[0] /= total[0]
    melt[1] /= total[0]
    melt[2] /= total[0]
    melt[3] /= total[0]
    melt[4] /= total[0]
    melt[5] /= total[0]
    melt[6] /= total[0]
    melt[7] /= total[0]
    vap[0] /= total[1]
    vap[1] /= total[1]
    vap[2] /= total[1]
    vap[3] /= total[1]
    vap[4] /= total[1]
    vap[5] /= total[1]
    vap[6] /= total[1]
    vap[7] /= total[1]
    sc[0] /= total[1]
    sc[1] /= total[1]
    sc[2] /= total[1]
    sc[3] /= total[1]
    sc[4] /= total[1]
    sc[5] /= total[1]
    sc[6] /= total[1]
    sc[7] /= total[1]
    total[0] /= total[3]
    total[1] /= total[3]
    total[2] /= total[3]
    bnd[0] /= bnd[3]
    bnd[1] /= bnd[3]
    bnd[2] /= bnd[3]
    bnd[3] /= total[3]
    ubnd[0] /= ubnd[3]
    ubnd[1] /= ubnd[3]
    ubnd[2] /= ubnd[3]
    ubnd[3] /= total[3]
    irn[0] /= irn[3]
    irn[1] /= irn[3]
    irn[2] /= irn[3]
    irn[3] /= total[3]
    mntl[0] /= mntl[3]
    mntl[1] /= mntl[3]
    mntl[2] /= mntl[3]
    mntl[3] /= total[3]
    bndirn[0] /= bndirn[3]
    bndirn[1] /= bndirn[3]
    bndirn[2] /= bndirn[3]
    bndirn[3] /= total[3]
    bndml[0] /= bndml[3]
    bndml[1] /= bndml[3]
    bndml[2] /= bndml[3]
    bndml[3] /= total[3]
    ubndirn[0] /= ubndirn[3]
    ubndirn[1] /= ubndirn[3]
    ubndirn[2] /= ubndirn[3]
    ubndirn[3] /= total[3]
    ubndml[0] /= ubndml[3]
    ubndml[1] /= ubndml[3]
    ubndml[2] /= ubndml[3]
    ubndml[3] /= total[3]
            
    fp.close()
    return total, bnd, ubnd, irn, mntl, bndirn, bndml, ubndirn, ubndml, melt, vap, sc




def main():
    if not( len(sys.argv) == 2 ):
        sys.exit( "Usage: ./phaseinfo.py <table file>" )
    tablefile = sys.argv[1]
    totals, bnd, ubnd, irn, mntl, bndirn, bndml, ubndirn, ubndml, melt, vap, sc = get_info( tablefile )
    print "PHASE BY TYPE:"
    print "           \tMelt    \tVapour  \tSuperCritical\tOfTotal"
    print "Total:     \t%f\t%f\t%f\t1.0" % \
          (totals[0],  totals[1],  totals[2])
    print "Bound:     \t%f\t%f\t%f\t%f" % \
          (bnd[0],     bnd[1],     bnd[2],     bnd[3])
    print "Unbound:   \t%f\t%f\t%f\t%f" % \
          (ubnd[0],    ubnd[1],    ubnd[2],    ubnd[3])
    print "Iron:      \t%f\t%f\t%f\t%f" % \
          (irn[0],     irn[1],     irn[2],     irn[3])
    print "Mantle:    \t%f\t%f\t%f\t%f" % \
          (mntl[0],    mntl[1],    mntl[2],    mntl[3])
    print "Bound Fe:  \t%f\t%f\t%f\t%f" % \
          (bndirn[0],  bndirn[1],  bndirn[2],  bndirn[3])
    print "Bound Mntl:\t%f\t%f\t%f\t%f" % \
          (bndml[0],   bndml[1],   bndml[2],   bndml[3])
    print "Unbound Fe:\t%f\t%f\t%f\t%f" % \
          (bndirn[0], ubndirn[1], ubndirn[2], ubndirn[3])
    print "Unbound Ml:\t%f\t%f\t%f\t%f" % \
          (ubndml[0],  ubndml[1],  ubndml[2],  ubndml[3])
    print ""
    print "TYPE BY PHASE:"
    print "                Iron      Mantle    Bound     Unbound   BoundFe   BoundML   UBoundFe  UBoundML"
    print "Melt:           %f  %f  %f  %f  %f  %f  %f  %f" % \
          (melt[0],melt[1],melt[2],melt[3],melt[4],melt[5],melt[6],melt[7])
    print "Vapour:         %f  %f  %f  %f  %f  %f  %f  %f" % \
          (vap[0],vap[1],vap[2],vap[3],vap[4],vap[5],vap[6],vap[7])
    print "SuperCritical:  %f  %f  %f  %f  %f  %f  %f  %f" % \
          (sc[0],sc[1],sc[2],sc[3],sc[4],sc[5],sc[6],sc[7])



if __name__ == "__main__":
    main()
