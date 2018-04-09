#include <stdio.h>
#include <stdlib.h>

#include "globalvars.h"



int main( int argc, char **argv )
{

  if( argc != 2 ) {
    printf("\n>./CombineBodies <parameter file>\n\n");
    exit(0);
  }

  srand48(3.14159);
  readparam( argv[1] );
 
  load_particles(body1, &planet[0], &header[0]);
  load_particles(body2, &planet[1], &header[1]);

  move_body(&planet[0], &planet[1]);


  save_combined(&planet[0], &planet[1]);

}
