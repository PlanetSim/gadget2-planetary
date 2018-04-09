#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globalvars.h"
#include "prototypes.h"



void readparam(void)
{

  FILE *fp1;
  char buf[100];
  double temp;
  int i;


  if(!(fp1 = fopen(pfile,"r"))){
    printf("Could not find parameter file parameter.txt\n");
    exit(13);
  }


  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &dconv);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &temp);
  maxitd = (int) temp;
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &temp);
  nr = (int) temp;
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &Psurf);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%d", &isotherm);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &isoT);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%s", &ttable);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%d", &numMaterials);
  fgets(buf, 100, fp1);

  Rbound = (double *) malloc(numMaterials * sizeof(double));
  rhoguess = (double*) malloc(numMaterials * sizeof(double));

  fgets(buf, 100, fp1);
  fscanf(fp1, "%s", &buf);
  strcpy(tableName, buf);
  fgets(buf, 100, fp1);

  for(i=0; i<numMaterials; i++)
    {
	fgets(buf, 100, fp1);
	fscanf(fp1, "%lf", &Rbound[i]);
	fgets(buf, 100, fp1);
    }

  for(i=0; i<numMaterials; i++)
    {
	fgets(buf, 100, fp1);
	fscanf(fp1, "%lf", &rhoguess[i]);
	fgets(buf, 100, fp1);
    }

  fgets(buf, 100, fp1);
  fscanf(fp1, "%s", &buf);
  strcpy(fout, buf);
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &temp);
  nSPH = (int) temp;
  fgets(buf, 100, fp1);

  fgets(buf, 100, fp1);
  fscanf(fp1, "%lf", &temp);
  ID_SKIP = (int) temp;

  fclose(fp1);
 
  return;

}




