#include <stdio.h>
#include <math.h>
#include <stdlib.h>


#include "globalvars.h"
#include "prototypes.h"

void printU( double * );

void InitPlanet(double *dsize, double *tsize)
     // Function to initialize the self-gravity planet
{


  // 1D density and temperature arrays
  double *darr = malloc(numMaterials * nRho * sizeof(double));             // g/cm^3
  double *tarr = malloc(numMaterials * nT * sizeof(double));               // K

  // 2D pressure, energy, and entropy arrays
  double *parr = malloc(numMaterials * nRho * nT * sizeof(double));        // GPa
  double *earr = malloc(numMaterials * nRho * nT * sizeof(double));        // MJ/kg
  double *sarr = malloc(numMaterials * nRho * nT * sizeof(double));        // MJ/kg/K

  // Planet value arrays, radius, density, and temperature
  double *radarr  = malloc(nr * sizeof(double));                           // cm
  double *rhoarr  = malloc(nr * sizeof(double));                           // g/cm^3
  double *temparr = malloc(nr * sizeof(double));                           // K

  // Planet's delta m(r), mass(r), g(r), P(r)
  double *dmassarr = malloc(nr * sizeof(double));
  double *massarr  = malloc(nr * sizeof(double));                          // g
  double *gravarr  = malloc(nr * sizeof(double));                          // cm/s^2
  double *presarr  = malloc(nr * sizeof(double));                          // Pa

  // Planet's internal energy(r), entropy(r)
  double *engarr = malloc(nr * sizeof(double));                            // erg/g
  double *entarr = malloc(nr * sizeof(double));                            // erg/g/K

  // array to store the interpolated density values
  double *rhonewarr = malloc(nr * sizeof(double));

  double dr;                   // thickness of radial shell
  double diff, meandiffrho;    // difference between new interpolated density and old density
  int i, j, mat = 0;
  int count = 0;

  // Read in the EOS table
  Readtable(darr, tarr, parr, earr, sarr, dsize, tsize);


  for(i=0; i < nr; i++)
  {
      radarr[i] = Rbound[numMaterials-1] / ((double) (nr-1)) * (double) i;       // radarr[0] = 0, radarr[nr-1] = R of outer layer
  }

  // set up initial values for planet's density
  for(i = 0; i < nr; i++)
    {
	    
      if(mat == 0)
        {
          if(!(radarr[i] <= Rbound[mat]))          // this radius is not within the current layer
	    {
	      mat++;
	    }
        }else
        {
          if(!radarr[i] > Rbound[mat-1] && radarr[i] <= Rbound[mat])       // this radius is not within the current layer
	    {
	      mat++;
	    }
	}

      rhoarr[i] = rhoguess[mat];
      
    }

  // set up the temperature profile
  for(i = 0; i < nr; i++)
    {
      if(isotherm)
        {
          temparr[i] = isoT;
        }else{
	  if(i == 0) Tprofile(radarr, temparr);
        }
    }


  // initialize boundary values of mass, grav, and pressure
  massarr[0]  = 0.0;
  gravarr[0]  = 0.0;
  presarr[nr-1] = Psurf;

  // calculate mass(r) and grav(r) from the center out
  for(i = 1; i < nr; i++)
    {
      dr = radarr[i] - radarr[i-1];
      dmassarr[i] = dr * rhoarr[i] * 4 * PI * radarr[i] * radarr[i];
      massarr[i] = massarr[i-1] + dmassarr[i];
      gravarr[i] = G * massarr[i] / (radarr[i] * radarr[i]);
    }

  // calculate pressure from the surface in
  for(i = nr-2; i >= 0; i--)
    {
      dr = radarr[i+1] - radarr[i];
      presarr[i] = presarr[i+1] + 0.1*(rhoarr[i] * gravarr[i] * dr);    // multiply second term by 0.1 to convert to Pa
    }

  do
    { 
      mat = 0;
      meandiffrho = 0.0;
      for(i = 0; i < nr; i++)
        {
	  if(mat == 0)
	    {
	      if(!(radarr[i] <= Rbound[mat]))     // move on to the next material if I'm past Rbound for this material
                {
		  mat++;
                }
	    }else
	    {
	      if(!(radarr[i] >= Rbound[mat-1] && radarr[i] <= Rbound[mat]))  // move on to the next mat if I'm past Rbound
	        {
		    mat++;
		}
	    }

          rhonewarr[i] = Interpolate(temparr[i], presarr[i], darr, tarr, parr, dsize[mat], tsize[mat], mat);

          // calculate the mean difference between current density and new density
          diff = fabs(rhoarr[i] - rhonewarr[i]);
          meandiffrho += diff;
          rhoarr[i] = rhonewarr[i];

        }
    
      //JMAA 7/2011 - output density profiles if it's time to or if this is the first/last loop
      if( PROFILES ) {
	if( count % OUTPUTEVERY == 0 || meandiffrho <= dconv || count == (maxitd-1) || count == 0 ) {
	  
	  FILE * file;
	  char filename[200];
	  int En;
	  
	  sprintf( filename, "%s/%s%d%s", RHODIR, RHOFILEBASE, count, ".txt" );
	  if( !(file = fopen( filename, "w" )) ) {
	    printf( "Could not open file %s.\n", filename );
	    exit(0);
	  }
	  
	  for( En = 0; En < nr; En++ ) {
	    
	    fprintf( file, "%f%\t%f\n", radarr[En], rhoarr[En] );

	  }
		 
	  fclose(file);
	 
	}

      }

      count += 1;

      // calculate mass(r) and grav(r) from the center out
      for(i = 1; i < nr; i++)
        {
          dr = radarr[i] - radarr[i-1];
          dmassarr[i] = dr * rhoarr[i] * 4 * PI * radarr[i] * radarr[i];
          massarr[i] = massarr[i-1] + dmassarr[i];
          gravarr[i] = G * massarr[i] / (radarr[i] * radarr[i]);
        }

      // calculate pressure from the surface in
      for(i = nr-2; i >= 0; i--)
        {
          dr = radarr[i+1] - radarr[i];
          presarr[i] = presarr[i+1] + 0.1*(rhoarr[i] * gravarr[i] * dr);     // the factor of 0.1 is to convert the term to Pa
        }


    } while(meandiffrho > dconv && count < maxitd);



  // calculate entropy and internal energy profiles
  mat = 0;
  for(i = 0; i < nr; i++)
    {
	if(mat == 0)
	  {
	    if(!(radarr[i] <= Rbound[mat]))  // move on to the next material if I'm past Rbound
	      {
		mat++;
	      }
	  }else
	  {
	    if(!(radarr[i] >= Rbound[mat-1] && radarr[i] <= Rbound[mat]))  // move on to the next material if I'm past Rbound
	      {
	        mat++;
	      }
	  }

	CalcEnt(&(entarr[i]), &(engarr[i]), rhoarr[i], temparr[i], darr, tarr, sarr, earr, dsize[mat], tsize[mat], mat);
    }

  if(numMaterials > 1)
  {
      mat = 0;
      for(i=1; i < nr; i++)
      {
	  if(radarr[i] > Rbound[mat])
	  {
	    printf("Mass of material %d: %.4e g (%.4f Earth Masses, %.2f%)\n", mat+1, massarr[i-1], massarr[i-1]/5.9742e27, massarr[i-1]/massarr[nr-1]*100.);
	      j=i;
	      mat++;
	  } else if(radarr[i] == Rbound[numMaterials-1])
	  {
	    printf("Mass of material %d: %.4e g (%.4f Earth Masses)\n", mat+1, massarr[i]-massarr[j], (massarr[i]-massarr[j])/5.9742e27);
	  }
      }

      printf("Total Mass:         %.4e g (%.4f Earth Masses)\n", massarr[nr-1], massarr[nr-1]/5.9742e27);
  }




  // JMAA 8/2011 - This HACKEDITS code that prevents entropy from blowing-up at the planet surface (possibly due to zero-P boundary condition there)
  if( HACKEDITS ) {

    int En = 0;
    for( En = 0; En < nr; En++ ) {
      if( (entarr[En] > HACKPARAM1) && (En != 0) ) {
	entarr[En] = entarr[En-1];
      }
    }

  }
  // End HACKEDITS


  // JMAA 7/2011 - Make an entropy profile
  if( PROFILES ) {
    write_s_profile( nr, radarr, entarr );
    write_t_profile( nr, radarr, temparr );
  }


  // JMAA 7/2011 - Used for troubleshooting
  /*
  int index = 50;
  printf( "----------------------------------------------\n" );
  printf( "Particle data: \n" );
  printf( "Radius: %e\tEntropy: %e\tDensity: %f\tTemperature: %f\n",
           radarr[index], entarr[index], rhoarr[index], temparr[index] );
  printf( "----------------------------------------------\n" );
  */


  // turn the created profiles into a set of SPH particles
  MakeParticles(radarr, massarr, entarr, engarr);


  // JMAA 7/2011 - print a file of internal energies
  //  printU( engarr );

  free(darr);
  free(tarr);
  free(parr);
  free(earr);
  free(sarr);
  free(radarr);
  free(rhoarr);
  free(temparr);
  free(dmassarr);
  free(massarr);
  free(gravarr);
  free(presarr);
  free(engarr);
  free(entarr);


  return;

}


/*
 * Added by JMAA 7/2011	
 */
void printU( double *U ) {
  
  FILE *fh;

  if( !(fh = fopen("internalenergies.txt", "w")) ) {
    printf("Could not open file\n");
    exit(0);
  }

  int i= 0;
  for( i = 0; i < nr; i++ ) {
    fprintf( fh, "%f\n", U[i] );
  }

  fclose( fh );

}
