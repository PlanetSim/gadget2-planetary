#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"



void findCM(struct planet_data *p)
{

    int i;
    double mtot=0.0;
    

    Xcm = 0.0;
    Ycm = 0.0;
    Zcm = 0.0;
    Vxcm = 0.0;
    Vycm = 0.0;
    Vzcm = 0.0;

    for(i=1; i <= p->Ntot; i++)
    {
	Xcm  += p->pos[i][1]*p->m[i];
	Ycm  += p->pos[i][2]*p->m[i];
	Zcm  += p->pos[i][3]*p->m[i];
	Vxcm += p->vel[i][1]*p->m[i];
	Vycm += p->vel[i][2]*p->m[i];
	Vzcm += p->vel[i][3]*p->m[i];
	mtot += p->m[i];
    }

    Xcm  /= mtot;
    Ycm  /= mtot;
    Zcm  /= mtot;
    Vxcm /= mtot;
    Vycm /= mtot;
    Vzcm /= mtot;


    return;

}



void changeCoords(struct planet_data *p)
{

    int i;

    for(i=1; i <= p->Ntot; i++)
    {
	p->pos[i][1] -= Xcm;
	p->pos[i][2] -= Ycm;
	p->pos[i][3] -= Zcm;
	p->vel[i][1] -= Vxcm;
	p->vel[i][2] -= Vycm;
	p->vel[i][3] -= Vzcm;
    }

    // change center of earth position to center of mass coordinates
    Xe -= Xcm;
    Ye -= Ycm;
    Ze -= Zcm;


    return;

}
