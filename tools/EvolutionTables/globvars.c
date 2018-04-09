#include <stdio.h>
#include "globvars.h"


/* --------- For loadsnap.c ---------*/
struct io_header_1
 header1;
int     npart, Ngas;
struct particle_data 
 *P;
int *Id;
double  Time, Redshift;


/* --------- For parameter file data ---------*/
char inputdir[200];
char outputdir[200];
int nMat;
char eostablebase[100];
char snapbase[100];
long int idskip;
