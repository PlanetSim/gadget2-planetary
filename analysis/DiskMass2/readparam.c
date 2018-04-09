#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globalvars.h"


void readparam(void)
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
  strcpy(snapbase, buf);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%s", &buf);
  strcpy(fout, buf);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &gamma1);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &b);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%d", &n1);
  fgets(buf, 100, fp1);
  
  fclose(fp1);

  return;

}
