#include <stdio.h>
#include <math.h>
#include <stdlib.h>


#include "globalvars.h"
#include "prototypes.h"


void MakeParticles(double *radarr, double *massarr, double *entarr, double *engarr)
     // Turn the planet's profile into a set of SPH particles
{


  PARTICLE *particles = malloc(nSPH * sizeof(PARTICLE));

  int *numpart  = malloc(numMaterials * sizeof(int));             // number of particles of each type
  double *mpart = malloc(numMaterials * sizeof(double));          // total mass in each type of particle

  double dmass[nr];                      // mass of each radial shell
  int numr[nr];                          // number of particles at each radius
  int i,j,k,n;
  int count = 0;
  int mat = 0;

  //JMAA 8/2011 - part of fix for particle masses - see README
  double mass_of_one_particle = massarr[nr-1]/((double)nSPH);

  srand48(PI);                 // initialize the random number generator for assigning positions


  for(i=0; i<numMaterials; i++)
    {
      numpart[i] = 0;
      mpart[i] = 0.0;
    }

  // determine the number of particles in each radial shell
  for(i = 0; i < nr; i++)
    {
      if(i==0)
	{
	  dmass[i] = massarr[i];
	  numr[i]  = 0;
	}else{
	  dmass[i] = massarr[i] - massarr[i-1];
	}

      // the number of particles in each radial shell is the total number
      // of particles times the fraction of the total mass in that shell
      numr[i] = (int) ((dmass[i] / massarr[nr-1]) * nSPH);
      count += numr[i];

      if(mat == 0)
        {
	  if(!(radarr[i] <= Rbound[mat]))
	    {
	      mat++;
	    }
        }else
        {
	  if(!(radarr[i] >= Rbound[mat-1] && radarr[i] <= Rbound[mat]))
	    {
	      mat++;
	    }
        }

      mpart[mat] += dmass[i];

    }



  /* JMAA 8/2011 - This method can cause huge problems at large resolution - see README
   * It has therefore been replaced with the following method, which also prints out
   * a warning message that alerts users to the possible discrepancy.
  // count may be less than nSPH because of truncation
  // add one particle to each shell until count = nSPH
  i = 1;
  while(count < nSPH)
    {
      numr[i] += 1;
      count++;
      i++;
      if(i==nr) i=1;
    }
  * The following new method adds the missing particles from the outside in, rather than
  * the inside out. The problem with the previous method being that it caused SPH
  * density estimations to massively overestimate central density - which in turn 
  * messed up other thermodynamic variables since density is used as an independent
  * parameter.
  * This improvement adds from the outside (where additional particles make little
  * difference due to the huge size of the shells) in, preventing any from being added to
  * the central half of the radial bins.
  * An even better solution would be to add them proporionally to their current number
  * densities - but that was more complicated that I felt was neccesary.
  */
  if( nSPH > count )
    printf( "WARNING: %d SPH particles will be added arbitrarily without recourse to the desired density profile - if this number is too high it might mess up SPH density calculations. Consider reducing the number of radial bins used to reduce the problem.\n", nSPH-count );
  i = nr-1;
  while( count < nSPH ) {
    numr[i] += 1;
    count ++;
    i --;
    if( i < (int)(nr/2) ) i = nr-1;
  }


  k = 0;    // k is the index of radial bin
  n = 0;    // n is the number of particles in the current bin
  mat = 0;

  // initialize all of the particles
  for(i = 0; i < nSPH; i++)
    {


      // initialize the velocities to zero
      for(j = 0; j < NDIM; j++)
	{
	  particles[i].vel[j] = 0.0;
	}

      if(n < numr[k])          // need more particles at the present radius
	{
	  particles[i].rad = radarr[k];
	  n++;
	}else{                 // move on to the next radius
	  k++;
	  particles[i].rad = radarr[k];
	  n = 1;
	}

      if(mat == 0)
      {
	  if(!(particles[i].rad <= Rbound[mat]))
	  {
	      mat++;
	  }
      }else
      {
	  if(!(particles[i].rad >= Rbound[mat-1] && particles[i].rad <= Rbound[mat]))
	  {
	      mat++;
	  }
      }

      numpart[mat] += 1;


      // randomly place the particle on a sphere of radius particles[i].rad
      Findxyz(&(particles[i]));


      // assign the particle the appropriate entropy and internal energy
      particles[i].s = entarr[k];
      particles[i].u = engarr[k];

      //JMAA 7/2011 - Added for troubleshooting
      /*
      if( particles[i].rad > 3.2e8 && particles[i].rad < 3.3e8 ) {
	printf("Particle data: \n");
	printf("Entropy: %f\t\n", particles[i].s );
      }
      */

      
    }


  // JMAA 8/2011 - writes the total planet internal energy to the standard output
  print_total_u( particles, mass_of_one_particle );


  WriteICs(particles, numpart, (float)mass_of_one_particle);


  free(particles);

}
             
