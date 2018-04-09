#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"
#include "prototypes.h"




void Tprofile(double *rad, double *T)
{


  FILE *fp1;
  int num;      // number of lines in the table
  int i,j = 0;
  double *rvalues, *tvalues;
  double deltaT, deltaR;


  if(!(fp1 = fopen(ttable, "r")))
    {
      printf("Could not find the temperature profile table %s\n", ttable);
      exit(13);
    }
  fscanf(fp1, "%d", &num);

  rvalues = (double *) malloc(num * sizeof(double));
  tvalues = (double *) malloc(num * sizeof(double));


  for(i = 0; i < num; i++)
    {
      fscanf(fp1, "%lf%lf", &(rvalues[i]), &(tvalues[i]));
    }


  for(i = 0; i < nr; i++)
  {
      // find the index of the smallest R in rvalues that is larger than rad[i]
      while(j < num && rad[i] > rvalues[j])
        {
	  j++;
	}

      if(j == 0)
	// rad[i] is smaller than the smallest rvalue, extrapolate
        {
	  T[i] = tvalues[j] - (tvalues[j+1] - tvalues[j])/(rvalues[j+1] - rvalues[j]) * rad[i];
	} else if(rad[i] > rvalues[j])
	// rad[i] is larger than the largest rvalue, extrapolate
        {
	    T[i] = tvalues[j] + (tvalues[j] - tvalues[j-1])/(rvalues[j] - rvalues[j-1]) * rad[i];
	} else
	    // there are two points to interpolate between
	{
	  deltaT = tvalues[j] - tvalues[j-1];
	  deltaR = rvalues[j] - rvalues[j-1];
	  T[i] = tvalues[j] + (deltaT/deltaR) * (rad[i] - rvalues[j]);
	}

  }


  fclose(fp1);

  return;

}
