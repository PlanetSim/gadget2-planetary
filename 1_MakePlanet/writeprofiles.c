#include <stdio.h>
#include <stdlib.h>


#include "globalvars.h"
#include "prototypes.h"

/* writeprofiles.c
 * Code to output radial profiles from before initial conditions files are written.
 * These are useful for troubleshooting as well as providing information such as
 * the total internal energy of the body
 * JMAA 8/2011
 */


/* write_s_profile
 * Function that outputs a file containing the entropy profile of the planet
 * Location of the file set by options in globalvars.h
 */
void write_s_profile( int nr, double * radarr, double * entarr ) {

  FILE *file;
  int i = 0;
  char filename[200];

  sprintf( filename, "%s/%s.txt", PDIR, SFILENAME );
  if( !(file = fopen( filename, "w" )) ) {
    printf( "Could not open file %s.\n", filename );
    exit(0);
  }  
  for( i = 0; i < nr; i++ ) { 
    fprintf( file, "%f\t%f\n", radarr[i], entarr[i] );
  }
  fclose(file);

}


/* write_t_profile
 * Function that outputs a temperature profile
 * Location and filename set in globalvars.h
 */
void write_t_profile( int nr, double * radarr, double * temparr ) {

  FILE *file;
  int i = 0;
  char filename[200];

  sprintf( filename, "%s/%s.txt", PDIR, TFILENAME );
  if( !(file = fopen( filename, "w" )) ) {
    printf( "Could not open file %s.\n", filename );
    exit(0);
  }  
  for( i = 0; i < nr; i++ ) { 
    fprintf( file, "%f\t%f\n", radarr[i], temparr[i] );
  }
  fclose(file);

}



/* print_total_u
 * Prints the total internal energy of the planet (that is,
 * real internal energy, not specific internal energy) that
 * is to be made. This is done to the stdout.
 * N.B. this assumes all particles have identical masses
 */
void print_total_u( PARTICLE * particles, double mass ) {

  int i;
  double total = 0.0;

  for( i = 0; i < nSPH; i ++ ) {
    total += particles[i].u;
  }

  total *= mass;

  printf( "Total internal energy of planet: %e erg\n", total );

}
