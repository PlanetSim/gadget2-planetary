#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globvars.h"
#include "eos.h"

/* PJC 14/12/15 U is used for entropy! */

void pressure()//struct particle_data *p)
{

  int i;
  //  double v2;

  for(i=1; i <= NumPart; i++)
    {
      P[i].P = (float)eos_table_interp(P[i].Rho, P[i].U, Id[i], EOS_PRESSURE, EOS_EXTRAP);
      P[i].P /= 1.0e10;  // convert pressure to GPa
      P[i].T = (float)eos_table_interp(P[i].Rho, P[i].U, Id[i], EOS_TEMPERATURE, EOS_EXTRAP);
      P[i].IntE = (float)eos_table_interp(P[i].Rho, P[i].U, Id[i], EOS_INT_ENERGY, EOS_EXTRAP);
      P[i].cs = (float)eos_table_interp(P[i].Rho, P[i].U, Id[i], EOS_SOUND_SPEED, EOS_EXTRAP);
      //v2 = p->vel[i][1]*p->vel[i][1]+p->vel[i][2]*p->vel[i][2]+p->vel[i][3]*p->vel[i][3];
      //printf("%d %g\n", p->id[i], v2);
    }


}
