#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


#include "globalvars.h"
#include "prototypes.h"




int main( int argc, char **argv )
{

  FILE *fp1;

  if( argc != 2 ) {
    printf("\n>./InitPlanet <parameter file>\n\n");
    exit(0);
  }

  strcpy(pfile, argv[1]);

  // number of density and temperature points in each EOS table
  double *dsize = malloc(numMaterials * sizeof(double));
  double *tsize = malloc(numMaterials * sizeof(double));

  char filename[200];
  int i;


  readparam();

  nRho = 0;
  nT   = 0;


  for(i=0; i < numMaterials; i++)
    {
      sprintf(filename, "%s%d%s", tableName, i+1, ".txt");

      if(!(fp1 = fopen(filename, "r")))
        {
	  printf("Could not find EOS table %s\n", filename);
	  exit(15);
        }

      fscanf(fp1, "%lf", &dsize[i]);
      fscanf(fp1, "%lf", &tsize[i]);

      if((int)dsize[i] > nRho)
	nRho = (int) dsize[i];
      if((int)tsize[i] > nT)
	nT = (int) tsize[i];

      fclose(fp1);
    }

  InitPlanet(dsize, tsize);

  return 0;

}

