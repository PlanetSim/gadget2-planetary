#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "globalvars.h"


int main( int argc, char **argv ) {

  char pfile[200];

  if( argc != 2 ) {
    printf("\n>./CorrectEntropy <parameter file>\n\n");
    exit(0);
  }

  strcpy( pfile, argv[1] );

  //read in parameters
  printf( "Reading in parameter file: %s\n", pfile );
  readparam( pfile );

  //load particles from snapshot
  printf( "Loading snapshot file: %s\n", snapname );
  load_particles( &planet, &header );

  //change the entropies of the particles
  change_entropies( &planet );

  //save new IC file
  save_snapshot( &planet, &header );

  return 0;

}
