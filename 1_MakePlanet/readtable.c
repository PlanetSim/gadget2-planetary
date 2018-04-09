#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "globalvars.h"
#include "prototypes.h"



void Readtable(double *darr, double *tarr, double *parr, double *earr, double *sarr, double *dsize, double *tsize)
     // Routine to read in the desired EOS table
     // Remove the header information from the table before using
     // The table must be formated with #density points, #temp points
     // density, temp, pressure, energy, entropy
{


  FILE *fp1;
  char filename[25];
  int mat, i, j;


  for(mat=0; mat < numMaterials; mat++)
    {

      sprintf(filename, "%s%d%s", tableName, mat+1, ".txt");

      if(!(fp1 = fopen(filename, "r")))
        {
	  printf("Could not find EOS table %s\n", filename);
	  exit(15);
        }
      // first comes the number of density points
      fscanf(fp1, "%lf", &dsize[mat]);

      // number of temperature points
      fscanf(fp1, "%lf", &tsize[mat]);

      // read in the density array
      for(i = 0; i < nRho; i++)
        {
	  if(i < (int)dsize[mat])
	    {
              fscanf(fp1, "%lf", &(darr[i + mat*nRho]));
	    }else
	    {
	      darr[i + mat*nRho] = 0.0;
	    }
        }

      // read in the temperature array
      for(j = 0; j < nT; j++)
        {
	  if(j < (int)tsize[mat])
	    {
              fscanf(fp1, "%lf", &(tarr[j + mat*nT]));
	    }else
	    {
	      tarr[j + mat*nT] = 0.0;
	    }
        }


      // next is the pressure, loop over temperature, loop over density
      for(j = 0; j < nT; j++)
        {
          for(i = 0; i < nRho; i++)
    	    {
	      if(j < (int)tsize[mat] && i < (int)dsize[mat])
		{
 	          fscanf(fp1, "%lf", &(parr[nT*i + j + mat*nRho*nT]));
		}else
		{
	          parr[nT*i + j + mat*nRho*nT] = 0.0;
		}
	    }  // end d loop
        }      // end t loop


      // next is the energy, loop over temperature, loop over density
      for(j = 0; j < nT; j++)
        {
          for(i = 0; i < nRho; i++)
	    {
	      if(j < (int)tsize[mat] && i < (int)dsize[mat])
	        {
	      fscanf(fp1, "%lf", &(earr[nT*i + j + mat*nRho*nT]));
		}else
		{
		  earr[nT*i + j + mat*nRho*nT] = 0.0;
		}
	    }  // end d loop
        }      // end t loop


      // last is the entropy, loop over temperature, loop over density
      for(j = 0; j < nT; j++)
        {
          for(i = 0; i < nRho; i++)
	    {
	      if(j < (int)tsize[mat] && i < (int)dsize[mat])
		{
	          fscanf(fp1, "%lf", &(sarr[nT*i + j + mat*nRho*nT]));
		}else
		{
		  sarr[nT*i + j + mat*nRho*nT] = 0.0;
		}
	    }  // end d loop
        }      // end t loop


      fclose(fp1);

    }

  return;

}




