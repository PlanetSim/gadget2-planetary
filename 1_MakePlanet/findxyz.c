#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"
#include "prototypes.h"



void Findxyz(PARTICLE *p)
{

  double theta, phi;
  double rad = p->rad;


  // generate a random phi between 0 and 2PI
  // and a random theta between 0 and PI
  phi = drand48() * 2.0 * PI;
  theta = acos(drand48() * 2.0  - 1.0);

  p->pos[0] = rad * sin(theta) * cos(phi);    // x
  p->pos[1] = rad * sin(theta) * sin(phi);    // y
  p->pos[2] = rad * cos(theta);               // z

}

