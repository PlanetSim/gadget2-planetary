#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"

double * get_com( struct planet_data * );

void identify_body(struct planet_data *p1, struct planet_data *p2)
{

  int i, body_skip;
  
  body_skip=ID_SKIP/2;

  //printf("%d %d %d in planet 1, 2\n", body_skip, p1->Ntot, p2->Ntot);
  
  if ( p1->Ntot + p2->Ntot <= body_skip )
    for(i = 1; i <= p2->Ntot; i++)
      p2->id[i] += body_skip;
  //printf("Final ID: %d\n", p2->id[i-1]);

}

void identify_crust(struct planet_data *p, double rcrust) {
  
  int i;
  double rad;
  double * com1;
  
  com1 = get_com(p);
  
  for(i=1; i <= p->Ntot; i++) {
    rad = sqrt((p->pos[i][1]-com1[0])*(p->pos[i][1]-com1[0]) + (p->pos[i][2]-com1[1])*(p->pos[i][2]-com1[1]) + (p->pos[i][3]-com1[2])*(p->pos[i][3]-com1[2]) );
    if ( rad >= rcrust && p->id[i] > ID_SKIP ) /* if particle is beyond inner crust radius, and is already mantle, bump to next material */
      p->id[i] += ID_SKIP;
  }
  //printf("Final ID: %d\n", p->id[i-1]);
  free(com1);
  
}