#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"


void bound(struct planet_data *p)
{

  int i;
  double xcm,ycm,zcm;
  double vxcm,vycm,vzcm;
  double omx, omy, omz;
  double mtot, r2, omega, T;
  double Lz, Iz;


  xcm=ycm=zcm=0.0;
  vxcm=vycm=vzcm=0.0;
  omx=omy=omz=0.0;
  mtot=0.0;
  Lz=Iz=0.0;
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
		  p->idsave[i] = p->id[i];
		  p->id[i] = 0;
		  xcm  += p->pos[i][1]*p->m[i];
		  ycm  += p->pos[i][2]*p->m[i];
		  zcm  += p->pos[i][3]*p->m[i];
		  vxcm += p->vel[i][1]*p->m[i];
		  vycm += p->vel[i][2]*p->m[i];
		  vzcm += p->vel[i][3]*p->m[i];
		  mtot += p->m[i];
		}
	    }
	}
    }

  xcm /= mtot;
  ycm /= mtot;
  zcm /= mtot;
  vxcm /= mtot;
  vycm /= mtot;
  vzcm /= mtot;

  for(i = 1; i <= p->Ntot; i++)
    {
      if(p->id[i] == 0)
      {
	// go into CM frame
	p->pos[i][1] -= xcm;
	p->pos[i][2] -= ycm;
	p->pos[i][3] -= zcm;
	p->vel[i][1] -= vxcm;
	p->vel[i][2] -= vycm;
	p->vel[i][3] -= vzcm;

	
	// calculate omegaX, omegaY, omegaZ
	r2  = p->pos[i][2]*p->pos[i][2]+p->pos[i][3]*p->pos[i][3];
	omx += (p->pos[i][2]*p->vel[i][3] - p->pos[i][3]*p->vel[i][2])/r2;
	r2  = p->pos[i][1]*p->pos[i][1]+p->pos[i][3]*p->pos[i][3];
	omy += (p->pos[i][3]*p->vel[i][1] - p->pos[i][1]*p->vel[i][3])/r2;
	r2  = p->pos[i][1]*p->pos[i][1]+p->pos[i][2]*p->pos[i][2];
	omz += (p->pos[i][1]*p->vel[i][2] - p->pos[i][2]*p->vel[i][1])/r2;


	// calculate Lz
	Lz += p->m[i] * (p->pos[i][1]*p->vel[i][2] - p->pos[i][2]*p->vel[i][1]);
	Iz += p->m[i] * (p->pos[i][1]*p->pos[i][1] + p->pos[i][2]*p->pos[i][2]);
      }
    }
    
  omx /= (double)count;
  omy /= (double)count;
  omz /= (double)count;
  omega = sqrt(omx*omx+omy*omy+omz*omz);
  //T = 2*PI / omega;
  T = 2*PI / omz;
  T /= 3600.;
  //printf("spin period: %g hours\n", T);
  

  T = 2*PI * Iz/Lz;
  T /= 3600.;

  printf("spin period: %g hours\n", T);
  exit(13);


  return;
    
}
