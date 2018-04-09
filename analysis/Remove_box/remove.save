#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"


void bound(struct planet_data *p)
{

  int i;

  count=0;
  for(i = 1; i <= p->Ntot; i++)
    {
      if(p->pos[i][1] > xmin && p->pos[i][1] < xmax)
	{
	  if(p->pos[i][2] > ymin && p->pos[i][2] < ymax)
	    {
	      if(p->pos[i][3] > zmin && p->pos[i][3] < zmax)
		{
		  count++;
		  p->id[i] = 0;
		}
	    }
	}
    }


    return;
    
}
