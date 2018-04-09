#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"


void bound( struct planet_data *p ) {

  int i;
  double r;
  double totmass = 0;

  count=0;
  for(i = 1; i <= p->Ntot; i++) {
      
    //checks for position exceeding maximum allowed radius
    r = sqrt(p->pos[i][1]*p->pos[i][1]+p->pos[i][2]*p->pos[i][2]+p->pos[i][3]*p->pos[i][3]);
    if( r > rad ) {
      p->id[i] = 0;
      totmass += p->m[i];
      count++;
    }

  }

  printf( "Mass to be removed: %e g\n\t%e %\n", totmass, 100*totmass/p->Mtot );

  return;
    
}
