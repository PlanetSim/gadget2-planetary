#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"
#include "eos.h"


void pressure(struct planet_data *p)
{

  int i;
  double v2;

  for(i=1; i <= p->Ntot; i++)
    {
      p->pres[i] = (float)eos_table_interp(p->rho[i], p->s[i], p->id[i], EOS_PRESSURE, EOS_EXTRAP);
      p->pres[i] /= 1.0e10;  // convert pressure to GPa
      p->T[i] = (float)eos_table_interp(p->rho[i], p->s[i], p->id[i], EOS_TEMPERATURE, EOS_EXTRAP);
      p->u[i] = (float)eos_table_interp(p->rho[i], p->s[i], p->id[i], EOS_INT_ENERGY, EOS_EXTRAP);
      //v2 = p->vel[i][1]*p->vel[i][1]+p->vel[i][2]*p->vel[i][2]+p->vel[i][3]*p->vel[i][3];
      //printf("%d %g\n", p->id[i], v2);
    }


}
