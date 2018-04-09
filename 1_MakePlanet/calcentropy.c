#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "globalvars.h"
#include "prototypes.h"



void CalcEnt(double *entropy, double *energy, double rho, double T, double *darr, double *tarr, double *sarr, double *earr, int dsize, int tsize, int mat)
     // Look up the entropy and energy from the appropriate table given the density and temperature
     // Interpolate over temperature to find S(rho) and E(rho) and then interpolate over density
{

  double *sintarr = malloc(dsize * sizeof(double));
  double *eintarr = malloc(dsize * sizeof(double));

  double deltaS, deltaE, deltaT, deltarho;
  int j, i = 0;


  // find the index of the smallest temperature in the array that is larger than T
  while(i < tsize && T > tarr[i + mat*nT])
    {
      i++;
    }

  if(i == 0)
    // the smallest temperature in the array was larger than T, use S/E(T0, rho)
    {
      for(j = 0; j < dsize; j++)
	{
	  sintarr[j] = sarr[i + j*nT + mat*nRho*nT];       // assign sintarr the T=T0 value
	  eintarr[j] = earr[i + j*nT + mat*nRho*nT];       // assign eintarr the T=T0 value
	}
    }else if(i == tsize)
	// the largest temperature in the array was smaller than T, use S/E(Tmax, rho)
    {
      for(j = 0; j < dsize; j++)
        {
          sintarr[j] = sarr[(i-1) + j*nT + mat*nRho*nT];   // assign sintarr the T=Tmax value
          eintarr[j] = earr[(i-1) + j*nT + mat*nRho*nT];   // assign eintarr the T=Tmax value
	}
    }else
      // Found two temperatures to interpolate between
      {
	for(j = 0; j < dsize; j++)
	  {
	    // find the entropy/energy at T[i] - entropy/energy at T[i-1] for each rho
	    deltaS = sarr[i + j*nT + mat*nRho*nT] - sarr[(i-1) + j*nT + mat*nRho*nT];
	    deltaE = earr[i + j*nT + mat*nRho*nT] - earr[(i-1) + j*nT + mat*nRho*nT];
	    deltaT = tarr[i + mat*nT] - tarr[i-1 + mat*nT];
	    // add the slope * change in temperature to the lower pressure
	    sintarr[j] = sarr[(i-1) + j*nT + mat*nRho*nT] + (deltaS/deltaT) * (T - tarr[i-1 + mat*nT]);
	    eintarr[j] = earr[(i-1) + j*nT + mat*nRho*nT] + (deltaE/deltaT) * (T - tarr[i-1 + mat*nT]);

	  }
      }


  i = 0;

  // find the index of the smallest value in rhoarr that is larger than given rho
  while(i < dsize && rho > darr[i + mat*nRho])
    {
      i++;
    }


  if(i == 0)
    // the smallest value is larger than rho, use S/E(0)
    {
      *entropy = sarr[i + mat*nRho];
      *energy  = earr[i + mat*nRho];
    }else if(i == dsize)
	// the largest value is smaller than rho, use S/E(dsize-1)
    {
      *entropy = sarr[i-1 + mat*nRho];
      *energy  = earr[i-1 + mat*nRho];
    }else
      // Found two densities to interpolate between
      {
	deltaS = sintarr[i] - sintarr[i-1];
	deltaE = eintarr[i] - eintarr[i-1];
	deltarho = darr[i + mat*nRho] - darr[i-1 + mat*nRho];
	// add the slope * change in pressure to the lower density
	*entropy = sintarr[i-1] + (deltaS/deltarho) * (rho - darr[i-1 + mat*nRho]);
	*energy  = eintarr[i-1] + (deltaE/deltarho) * (rho - darr[i-1 + mat*nRho]);
      }

  *entropy *= 1.0e10;     // convert to erg/g/K
  *energy  *= 1.0e10;     // convert to erg/g

  free(sintarr);
  free(eintarr);

}

