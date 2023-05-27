#include "globalvars.h"

char infile[200]; // base name for the snapshot files
char fout[200];   // output file
double tol;       // percentage tollerance for mass change between iterations
int maxit;        // maximum number of iterations
int minparts;     // minimum number of particles to be considered a remnant
int nMat;         // number of materials present
int idskip;       // id skip between materials

