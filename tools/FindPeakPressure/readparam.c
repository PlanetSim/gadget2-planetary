#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globvars.h"



void read_param( char * filename ) {

  FILE *fp1;
  char buf[100];
  double temp;
  int i;

#define SKIP fgets(buf,100,fp1);

  if( !(fp1 = fopen(filename,"r")) ) {
    printf( "Could not find parameter file %s\n", filename );
    exit(0);
  }


  SKIP;
  fscanf(fp1, "%s", &inputdir);
  SKIP;

  SKIP;
  fscanf(fp1, "%s", &outputfile);
  SKIP;

  SKIP;
  fscanf(fp1, "%lf", &temp);
  nMat = (int) temp;
  SKIP;

  SKIP;
  fscanf(fp1, "%s", &eostablebase);
  SKIP;

  SKIP;
  fscanf(fp1, "%s", &snapbase);
  SKIP;

  SKIP;
  fscanf(fp1, "%lf", &temp);
  idskip = (long int) temp;
  SKIP;

  fclose(fp1);

  return;

}




