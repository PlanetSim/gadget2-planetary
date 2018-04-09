#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "globvars.h"

/**********************************************************
 PhaseDiagram
 Use the inverted EOS table to write a table of phase diagram
 data for isobars on a T-against-S plot.
***********************************************************/


int main( int argc, char **argv ) {

  char eosfilename[200];
  char tablefilename[200];
  int nRho, nS;
  double * Rho;
  double * S;
  double * P; //2d
  double * T; //2d
  double * U; //2d
  double * C; //2d
  double * isobars; //2d array holding the isobars
  double * pressures; //1d array holding the pressures relating to each isobar
  int skip, i, j, k, r;
  double pressure, deltaT, deltaP;

  if( argc != 3 ) {
    printf("\n>./PhaseDiagram <eos table filename> <output table filename>\n\n");
    exit(0);
  }

  strcpy( eosfilename, argv[1] );
  strcpy( tablefilename, argv[2] );

  printf( "Loading EOS table: %s\n", eosfilename );
  load_eos_table( eosfilename, &nRho, &nS, &Rho, &S, &P, &T, &U, &C );

  //calculate density skip
  if( _NOISOBARS == 0 ) {
    skip = 1;
  } else {
    skip = (int) (nRho/_NOISOBARS);
  }

  //allocate memory for output tables
  isobars = malloc( nS * _NOISOBARS * sizeof(double) );
  pressures = malloc( _NOISOBARS * sizeof(double) );

  printf( "Creating %d isobars from EOS table.\n", _NOISOBARS );
  //printf( "[" ); fflush(stdout);
  /* Loop through density points, each one will define the pressure for
   * the isobar via the first entropy point.
   */
  int S_zero = (int)(150);
  printf( "S_0 = %e\n", S[S_zero] );
  for( i = 0; i < _NOISOBARS; i++ ) {
 
    r = i*skip; //the rho-index for this isobar
    pressure = P[ S_zero*nRho + r ];
    pressures[i] = pressure;

    //printf( "%d,", i ); fflush(stdout);

    /* Loop through the entropy points, for each one finding the density
     * that corresponds to the desired pressure then using this to find the
     * temperature. Linear-interpolation is required since pressures
     * will not match exactly
     */
    for( j = 0; j < nS; j++ ) {

      if( j == 0 ) {
	//first one is trivial since we have an exact pressure
	isobars[ i*nS + 0 ] = T[ 0*nRho + r ];
      } else {
	//The general case

	k = 0;
	while( P[j*nRho + k] < pressure && k < nRho ) { k++; }
	if( k == nRho ) {
	  //pressure is greater than values in this line in the table
	  //therefore extrapolate
	  deltaT = T[j*nRho + k-1] - T[j*nRho + k-2];
	  deltaP = P[j*nRho + k-1] - P[j*nRho + k-2];
	  isobars[ i*nS + j ] = T[j*nRho + k-1] + (deltaT/deltaP)*(pressure - P[j*nRho + k-1]);
	} else if ( k == 0 ) {
	  //pressure is less than values in this line in the table
	  //therefore extrapolate
	  deltaT = T[j*nRho + 1] - T[j*nRho + 0];
	  deltaP = P[j*nRho + 1] - P[j*nRho + 0];
	  isobars[ i*nS + j ] = T[j*nRho + 0] - (deltaT/deltaP)*(P[j*nRho + 0] - pressure);
	} else {
	  //interpolate
	  deltaT = T[j*nRho + k] - T[j*nRho + k-1];
	  deltaP = P[j*nRho + k] - P[j*nRho + k-1];
	  isobars[ i*nS + j ] = T[j*nRho + k-1] + (deltaT/deltaP)*(pressure - P[j*nRho + k-1]);
	}

      }

    }

  }
  //printf( "]\n" );

  printf( "Saving output table to: %s\n\n", tablefilename );
  save_table( tablefilename, isobars, S, pressures, nS );

  return(0);

}



void save_table( char * file, double * isobars, double * S, double * pressures, int nS ) {

  FILE *fp;
  int i, j;
  
  if( !(fp = fopen(file, "w")) ) {
    printf( "Could not open table for writing: %s\n", file );
    exit(15);
  }

  //write header line
  fprintf( fp, "#Entropy" );
  for( i = 0; i < _NOISOBARS; i++ ) {
    fprintf( fp, "\tP%f", pressures[i] );
  }

  //write data
  for( i = 0; i < nS; i ++ ) {
    fprintf( fp, "\n%f", S[i] );
    for( j = 0; j < _NOISOBARS; j++ ) {
      fprintf( fp, "\t%f", isobars[ j*nS + i ] );
    }
  }

  fclose( fp );

}



void load_eos_table( char * file, int * nRhop, int * nSp, double ** Rhop, double ** Sp, double ** Pp, double ** Tp, double ** Up, double ** Cp ) {

  FILE *fp;
  int i;
    
  if( !(fp = fopen(file, "r")) ) {
    printf( "Could not find EOS table %s\n", file );
    exit(15);
  }

  fscanf( fp, "%d", nRhop );
  fscanf( fp, "%d", nSp );

  // allocate memory
  *Rhop = malloc( (*nRhop) * sizeof(double) );
  *Sp   = malloc( (*nSp)   * sizeof(double) );
  *Pp   = malloc( (*nSp)*(*nRhop) * sizeof(double) );
  *Tp   = malloc( (*nSp)*(*nRhop) * sizeof(double) );
  *Up   = malloc( (*nSp)*(*nRhop) * sizeof(double) );
  *Cp   = malloc( (*nSp)*(*nRhop) * sizeof(double) );

  //read in the data
  for( i = 0; i < *nRhop; i++ ) {
    fscanf( fp, "%lf", &((*Rhop)[i]) );
  }
  for( i = 0; i < *nSp; i++ ) {
    fscanf( fp, "%lf", &((*Sp)[i]) );
  }
  for( i = 0; i < (*nSp)*(*nRhop); i++ ) {
    fscanf( fp, "%lf", &((*Pp)[i]) );
  }
  for( i = 0; i < (*nSp)*(*nRhop); i++ ) {
    fscanf( fp, "%lf", &((*Tp)[i]) );
  }
  for( i = 0; i < (*nSp)*(*nRhop); i++ ) {
    fscanf( fp, "%lf", &((*Up)[i]) );
  }
  for( i = 0; i < (*nSp)*(*nRhop); i++ ) {
    fscanf( fp, "%lf", &((*Cp)[i]) );
  }

  //close the file
  fclose( fp );

}
