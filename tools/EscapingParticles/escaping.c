#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>

#include "globvars.h"

/**********************************************************
 Escaping
 Takes a Gadget2 snapshot file and outputs an ascii list of
 all those particles which (to first order) will escape to
 infinity
***********************************************************/



int main( int argc, char **argv ) {

  char infile[200];
  char outfile[200];
  int i, j;
  double E_k; // kinetic energy per unit mass
  double allmass = 0; // cumulative total mass
  double escmass = 0; // cumulative escaping mass
  double v2; //squared velocity
  double xdotv; //scalar product of position and velocity vectors
  unsigned long * indextable;
  int * escaping;

  if( argc != 3 ) {      
    printf("\n>./Escaping <snapshot file> <output file>\n\n");
    exit(0);
  }

  strcpy(infile, argv[1]);
  strcpy(outfile, argv[2]);

  //load this snapshot
  printf( "Loading snapshot: %s\n", infile );
  load_snapshot( infile, _NOFILES );

  //allocate memory
  escaping = malloc( npart * sizeof(int) );
  for( i = 1; i <= npart; i++ ) escaping[i] = -1;
  
  //loop through particles
  for( i = 1, j = 0; i <= npart; i++ ) {
    v2 = ( P[i].Vel[0] * P[i].Vel[0] +
	   P[i].Vel[1] * P[i].Vel[1] +
	   P[i].Vel[2] * P[i].Vel[2] );
    xdotv = ( P[i].Pos[0] * P[i].Vel[0] +
	      P[i].Pos[1] * P[i].Vel[1] +
	      P[i].Pos[2] * P[i].Vel[2] );
    E_k = 0.5 * v2;

    if( (E_k > -P[i].Potential) && (xdotv >= 0) ) {
      escaping[j] = P[i].Id;
      escmass += P[i].Mass;
      j++;
    }
    allmass += P[i].Mass;

  }

  printf( "Saving output: %s\n", outfile );
  save_output( outfile, escaping );

  printf( "Escaped mass: ( %e of %e )g\n\t%.2f% \n\t%d particles\n\n",
	  escmass, allmass, (escmass/allmass)*100, j );

  return(0);

}



void save_output( char * outputfile, int * ids ) {

  FILE *fp;
  int i = 0;

  if( !(fp = fopen( outputfile, "w")) ) {
    printf( "Error opening output file: %s.\n\n", outputfile );
    exit(0);
  }

  // write lines
  for( i = 1; (i <= npart && ids[i] != -1); i ++ ) {
    fprintf( fp, "%d\n", ids[i] );
  }

  fclose( fp );

}
