#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globalvars.h"


void readparam( char* fname, char * pfile ) {

  FILE *fp1;
  char buf[200];

  if( !(fp1 = fopen(pfile, "r")) ) {
    printf( "Could not open parameter file: %s\n", pfile );
    exit(13);
  }


  //fgets(buf, 100, fp1);
  sscanf(fname, "%s", &buf);
  strcpy(infile, buf);
  //fgets(buf, 100, fp1);

//  fgets(buf, 100, fp1);
//  fscanf(fp1, "%s", &buf);
  strcpy(fout, infile);
  strcat(fout,"_b.txt");
//  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &tol);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%d", &maxit);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%d", &minparts);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%d", &nMat);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%d", &idskip);
  fgets(buf, 100, fp1);
  
  fclose(fp1);

  return;

}
