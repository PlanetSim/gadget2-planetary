/*
** move.c:
**	Note that this code mixes 1-indexing for the particle arrays
**	(thanks to nrutil) with standard 0-indexing for the com 
**	calculation.
**
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"


double * get_com( struct planet_data * );


void move_body(struct planet_data *p1, struct planet_data *p2)
{

  double shift, rad;
  double theta, phi;
  double frac;
  double * com1, * com2;
  int i;

  printf( "Target " );
  com1 = get_com(p1);
  printf( "Projectile " );
  com2 = get_com(p2);

  shift = (R1 + R2) * b;


  for(i = 1; i <= p1->Ntot; i++)  {
    frac = p2->Mtot/(p1->Mtot+p2->Mtot);
    p1->pos[i][1] -= com1[0];
    p1->pos[i][2] -= com1[1];
    p1->pos[i][3] -= com1[2];
    p1->pos[i][1] -= sep*frac;
    p1->pos[i][2] -= shift*frac;
    p1->vel[i][1] += relV*frac;
  }
  for(i = 1; i <= p2->Ntot; i++) {
    frac = p1->Mtot/(p1->Mtot+p2->Mtot);
    p2->pos[i][1] -= com1[0];
    p2->pos[i][2] -= com1[1];
    p2->pos[i][3] -= com1[2];
    p2->pos[i][1] += sep*frac;
    p2->pos[i][2] += shift*frac;
    p2->vel[i][1] -= relV*frac;
  }

}




double * get_com( struct planet_data * p ) {

  int i;
  double * tpm = malloc( 3*sizeof(double) );
  double * com = malloc( 3*sizeof(double) );
  for( i = 1; i <= p->Ntot; i++ ) {
    if( i == 1 ) {
      tpm[0] = tpm[1] = tpm[2] = 0;
    }
    tpm[0] += p->pos[i][1]*p->m[i];
    tpm[1] += p->pos[i][2]*p->m[i];
    tpm[2] += p->pos[i][3]*p->m[i];
  }
  com[0] = tpm[0]/p->Mtot;
  com[1] = tpm[1]/p->Mtot;
  com[2] = tpm[2]/p->Mtot;
  free( tpm );
  printf( "Centre of mass: ( %e, %e, %e )\n", com[0], com[1], com[2] );
  return com;

}
