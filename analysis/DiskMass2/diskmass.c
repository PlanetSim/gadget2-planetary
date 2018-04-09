#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"


void findDiskMass(struct planet_data *p)
{

    double a, b;
    double omega, omega2, period, rad, OmegaMean=0.0;
    double x, y, v2;
    double E;
    double ex, ey, ez, rdotv;
    double semimaj, semimin, ecc, periap;
    double l, mu;
    int i, count=0;


    a = 3.0*Mpe/(4.0*PI*_Rho*(1.-f));
    a = pow(a, 1./3.);
    b = a*(1.-f);


    Lpe = 0.0;
    Mpe_fe = 0.0;
    Mpe_si = 0.0;
    Md  = 0.0;
    Ld  = 0.0;
    Mfe  = 0.0;
    Mimp = 0.0;
    Mesc = 0.0;
    Lesc = 0.0;


    for(i = 1; i <= p->Ntot; i++)
    {
	x = p->pos[i][1]-Xe;
	y = p->pos[i][2]-Ye;
	rad = sqrt(x*x+y*y);

	// omega = (r x v)/|r|^2 = (x*vy-y*vx)/rad^2
	omega = (x*p->vel[i][2]- y*p->vel[i][1])/(rad*rad);


	if(rad < a)
	{
	    //p->id[i]=1;
	    Lpe += p->m[i] * (x*p->vel[i][2] - y*p->vel[i][1]);
	    if(p->id[i] < 200000000) Mpe_fe += p->m[i];
	    if(p->id[i] > 200000000) Mpe_si += p->m[i];
	    OmegaMean += omega;
	    count++;
	} else{
	    v2 = p->vel[i][1]*p->vel[i][1]+p->vel[i][2]*p->vel[i][2]+p->vel[i][3]*p->vel[i][3];
	    rad = sqrt(x*x+y*y+(p->pos[i][3]-Ze)*(p->pos[i][3]-Ze));
	    E  = 0.5*v2 - (G*Mpe)/rad;

	    if(E > 0.0)  // unbound particle
	    {
		Mesc += p->m[i];
		Lesc += (double)p->m[i] * (p->pos[i][1]*p->vel[i][2] - p->pos[i][2]*p->vel[i][1]);
		//p->id[i]=2;
	    }else{       // bound particle, calculate orbital elements
		semimaj = -G*Mpe/(2.0*E);
		rdotv = p->pos[i][1]*p->vel[i][1]+p->pos[i][2]*p->vel[i][2]+p->pos[i][3]*p->vel[i][3];
		rad = sqrt(p->pos[i][1]*p->pos[i][1]+p->pos[i][2]*p->pos[i][2]+p->pos[i][3]*p->pos[i][3]);
		ex = v2*p->pos[i][1]/(G*Mpe) - rdotv*p->vel[i][1]/(G*Mpe) - p->pos[i][1]/rad;
		ey = v2*p->pos[i][2]/(G*Mpe) - rdotv*p->vel[i][2]/(G*Mpe) - p->pos[i][2]/rad;
		ez = v2*p->pos[i][3]/(G*Mpe) - rdotv*p->vel[i][3]/(G*Mpe) - p->pos[i][3]/rad;
		ecc = sqrt(ex*ex+ey*ey+ez*ez);
		periap  = semimaj*(1.0-ecc);
		if(periap > a)
		{
		  //p->id[i]=3;
		    Md += p->m[i];
		    Ld += p->m[i] * (p->pos[i][1]*p->vel[i][2] - p->pos[i][2]*p->vel[i][1]);
		    if(p->id[i] < 200000000) Mfe += p->m[i];                    // iron particle
		    if(p->id[i] > 200000000+n1) Mimp += p->m[i];                // particle from impactor
		    if(p->id[i] < 200000000 && p->id[i] > n1) Mimp += p->m[i];
		}else{
		  //p->id[i]=1;
		  if(p->id[i] < 200000000) Mpe_fe += p->m[i];
		  if(p->id[i] > 200000000) Mpe_si += p->m[i];
		}
	    }
	}

    }

    OmegaMean /= (double)count;
    Mpe = p->Mtot - Mesc - Md;
    f = 2.5*(OmegaMean*OmegaMean*a*a*a/(G*Mpe))/(1 + (2.5-15.*K/4.0)*(2.5-15.*K/4.0));


    printf("\nf = %.2g Mpe = %g a = %g km\n", f, Mpe/Mearth, a/1.0e5);
    printf("Silicates: %g (%g %), Iron: %g (%g %)\n", Mpe_si/Mearth, Mpe_si/Mpe*100., Mpe_fe/Mearth, Mpe_fe/Mpe*100.);
    printf("Rotation period: %g hours\n", 2*PI/OmegaMean/3600.);


    return;
    
}
