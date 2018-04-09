#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globalvars.h"


void readparam( char * pfile )
{

  FILE *fp1;
  char buf[100];
  double temp;
  int i;

  if(!(fp1 = fopen(pfile, "r"))){
    printf("could not fine parameter file %s\n", pfile);
    exit(13);
  }


  fgets(buf, 100, fp1);
  fscanf(fp1, "%s", &buf);
  strcpy(body1, buf);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%s", &buf);
  strcpy(body2, buf);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%s", &buf);
  strcpy(fout, buf);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &R1);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &R2);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &relV);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &b);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &sep);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &Rcrust1);
  fgets(buf, 100, fp1);   
  
  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &Rcrust2);
  fgets(buf, 100, fp1);   

  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &temp);
  ID_SKIP = (int) temp;
  fgets(buf, 100, fp1);

  fclose(fp1);

  return;

}
