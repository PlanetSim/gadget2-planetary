#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"


void bound(struct planet_data *p)
{

    int i, idmin;
    double potmin;
    double v2, rad, ke, pe;
    double oldm, bndm, bndm_fe, bndm_si;
    double xcm, ycm, zcm, vxcm, vycm, vzcm;
    double xcm2, ycm2, zcm2, vxcm2, vycm2, vzcm2;

    xcm=p->pos[1][1];
    ycm=p->pos[1][2];
    zcm=p->pos[1][3];
    vxcm=p->vel[1][1];
    vycm=p->vel[1][2];
    vzcm=p->vel[1][3];
    potmin=p->pot[1];
    bndm=p->m[1];
    idmin=p->id[1];

    bndm_fe = 0.0;
    bndm_si = 0.0;
    // find the particle closest to the potential minimum
    for(i = 1; i <= p->Ntot; i++)
    {
	p->bnd[i] = 0;

	if(p->pot[i] < potmin)
	{
	    xcm=p->pos[i][1];
	    ycm=p->pos[i][2];
	    zcm=p->pos[i][3];
	    vxcm=p->vel[i][1];
	    vycm=p->vel[i][2];
	    vzcm=p->vel[i][3];
	    potmin=p->pot[i];	    
	    bndm=p->m[i];
	    idmin=p->id[i];
	}
    }


    oldm = bndm/10.0;

    
    while(fabs(oldm-bndm) > tol)
    {
	oldm = bndm;

	for(i = 1; i <= p->Ntot; i++)
	{

	  v2 = (p->vel[i][1]-vxcm)*(p->vel[i][1]-vxcm) + (p->vel[i][2]-vycm)*(p->vel[i][2]-vycm) + (p->vel[i][3]-vzcm)*(p->vel[i][3]-vzcm);
	  rad = sqrt((p->pos[i][1]-xcm)*(p->pos[i][1]-xcm) + (p->pos[i][2]-ycm)*(p->pos[i][2]-ycm) + (p->pos[i][3]-zcm)*(p->pos[i][3]-zcm));

	  ke = 0.5*p->m[i]*v2;
	  pe = -G*bndm*p->m[i]/rad;
	    
	  if(ke+pe < 0.0)  // particle is bound
	    {
	      p->bnd[i] = 1;
	    }else
	    {
	      p->bnd[i] = 0;
	    }
	}
	
	xcm = ycm = zcm = 0.0;
	vxcm = vycm = vzcm = 0.0;
	bndm = 0.0;

	for(i = 1; i <= p->Ntot; i++)
	  {
	    if(p->bnd[i] == 1)
	      {
		bndm += p->m[i];
		xcm  += p->pos[i][1]*p->m[i];
		ycm  += p->pos[i][2]*p->m[i];
		zcm  += p->pos[i][3]*p->m[i];
		vxcm += p->vel[i][1]*p->m[i];
		vycm += p->vel[i][2]*p->m[i];
		vzcm += p->vel[i][3]*p->m[i];
	      }
	  }

	xcm  /= bndm;
	ycm  /= bndm;
	zcm  /= bndm;
	vxcm /= bndm;
	vycm /= bndm;
	vzcm /= bndm;

    }


    Xe = xcm;
    Ye = ycm;
    Ze = zcm;

    return;
    
}
