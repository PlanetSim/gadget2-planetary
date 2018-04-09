#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "globalvars.h"
#include "prototypes.h"



double Interpolate(double T, double P, double *darr, double *tarr, double *parr, int dsize, int tsize, int mat)
     // Interpolate to find the density given the pressure and the temperature
     // Take the pressure array which is a function of temperature and density
     // Interpolate the pressure to find P(rho) at the given T
     // Flip P(rho) to Rho(P) and interpolate to find the density given P
{

  // array to store the temperature interpolated pressure array P(rho)
  double *pintarr = malloc(dsize * sizeof(double));

  double deltaP, deltaT, deltarho;
  double rho;
  int i = 0;
  int j;


  // convert P to GPa because this is what is stored in parr
  P /= 1.0e9;

  // find the index of the smallest temperature in the array that is larger than T
  while(i < tsize && T > tarr[i + mat*nT])
    {
      i++;
    }

  if(i == 0)
    // the smallest temperature in the array was larger than T, use P(T0, rho)
    {
      for(j = 0; j < dsize; j++)
	{
	  pintarr[j] = parr[i + j*nT + mat*nRho*nT];       // assign pintarr the T=T0 value
	}
    }else if(i == tsize)
     // the largest temperature in the array was smaller than T, use P(Tmax, rho)
    {
      for(j = 0; j < dsize; j++)
	{
	  pintarr[j] = parr[(i-1) + j*nT + mat*nRho*nT];   // assign pintarr the T=Tmax value
	}
    }else
      // Found two temperatures to interpolate between
      {
	for(j = 0; j < dsize; j++)
	  {
	    // find the pressure at T[i] - pressure at T[i-1] for each rho
	    deltaP = parr[i + j*nT + mat*nRho*nT] - parr[(i-1) + j*nT + mat*nRho*nT];
	    deltaT = tarr[i + mat*nT] - tarr[i-1 + mat*nT];
	    // add the slope * change in temperature to the lower pressure
	    pintarr[j] = parr[(i-1) + j*nT + mat*nRho*nT] + (deltaP/deltaT) * (T - tarr[i-1 + mat*nT]);

	  }
      }

  i = 0;

  // find the index of the smallest pressure in pintarr that is larger than P
  while(i < dsize && P > pintarr[i])
    {
      i++;
    }


  if(i == 0)
    // the smallest pressure is larger than P, use rho(0)
    {
      rho = darr[i + mat*nRho];
    }else if(i == dsize)
	// the largest pressure is smaller than P, use rho(dsize-1)
    {
      rho = darr[i-1 + mat*nRho];
    }else
      // Found two pressures to interpolate between
      {
	deltaP = pintarr[i] - pintarr[i-1];
	deltarho = darr[i + mat*nRho] - darr[i-1 + mat*nRho];
	// add the slope * change in pressure to the lower density
	rho = darr[i-1 + mat*nRho] + (deltarho/deltaP) * (P - pintarr[i-1]);
      }



  free(pintarr);


  return rho;

}

