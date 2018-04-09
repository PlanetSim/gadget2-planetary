#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "globalvars.h"


/*
 * BoundMass
 * Program originally by RAM to calculate the bound mass after a planetary 
 * collision from a Gadget output. Outputs a table of particle ids & a 1 or 
 * 0 based on whether they're considered bound or not. Commented and edited 
 * by JMAA
 */


int main( int argc, char *argv[] ) {

  struct io_header H;
  struct particle_data P;

  if( argc != 2 ) {
    printf( "Usage: ./BoundMass <parameter file>\n\n" );
    exit(0);
  }
  readparam( argv[1] );

  load_particles( &P, &H );
  
  bound( &P );

  save_output( &P );

  return 0;


}
