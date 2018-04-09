#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "globalvars.h"


int main( int argc, char **argv )
{
  double fold;
  double fg, Ltot, Mm;
  int count=0;
  char pfile[200];

  if( argc != 2 ) {
    printf("\n>./Remove <parameter file>\n\n");
    exit(0);
  }

  strcpy( pfile, argv[1] );

  //read in parameters
  readparam( pfile );

  //load particles from snapshot
  load_particles(&planet[0], &header[0]);

  //if the distance of a particular particle from the origin is greater than the radius defined in the parameter file, set its id to zero
  bound(&planet[0]);

  //save new snapshot ignoring particles with id=0
  save_snapshot(&planet[0], &header[0]);
  return 0;


}
