#ifndef _GLOBALVARS_H
#define _GLOBALVARS_H

#include <stddef.h>
#define G 6.673e-8

// Parameters to be read in
extern char infile[200];
extern char fout[200];
extern double tol;
extern int maxit;
extern int minparts;
extern int nMat;
extern int idskip;

typedef struct io_header {
  int npart[6];
  double mass[6];
  double time;
  double redshift;
  int flag_sfr;
  int flag_feedback;
  int npartTotal[6];
  int flag_cooling;
  int num_files;
  double BoxSize;
  double Omega0;
  double OmegaLambda;
  double HubbleParam;
  int flag_age;
  int flag_metals;
  int nallhw[6];
  int flag_entr_ics;
  char fill[256 - 6 * 4 - 6 * 8 - 2 * 8 - 2 * 4 - 6 * 4 - 2 * 4 - 4 * 8 -
            2 * 4 - 6 * 4 - 1 * 4];
} IOHeader;

void readparam(char *, char *);

#endif
