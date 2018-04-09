#include <stdio.h>
#include "globvars.h"


/* --------- For loadsnap.c ---------*/
struct io_header_1
 header1;
int     NumPart, Ngas;
struct particle_data 
 *P;
int *Id;
double  Time, Redshift;


/* --------- For parameter file data ---------*/
char inputdir[200];
char saoutput[200];
char poutput[200];
double outerR;
int nMat;
char eostablebase[100];
int nRadialbins;
int binaverage;
int nBinaverage;
char tpbase[100];
char rhopbase[100];
char prespbase[100];
char upbase[100];
char spbase[100];
char snapbase[100];
long int idskip;
