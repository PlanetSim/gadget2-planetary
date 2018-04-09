#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"


int calculate_material( int id );


/*
 * Finds particle closest to potential minimum, then uses this as a seed for
 * the iterative procedure.
 * Given a selection of particles known to be bound, find all particles
 * bound w.r.t. them & add these to the list of bound particles.
 * Repeat until the bound mass change <= tol between iterations or until 
 * maxit is exceeded
 */
void bound( struct particle_data *p ) {

  int i, count;
  double potmin;
  double v2, rad, ke, pe; //particle w.r.t. CO-bound-M data
  double oldm, bndm, bndm_fe, bndm_si; //data from last iteration
  double xcm, ycm, zcm, vxcm, vycm, vzcm; //centre-of-bound-mass data
  double M_MERC = 3.3e26; //put bound mass in units of Merc mass
  double bndm_cr, totm;

  //Assume first particle is at the potential minimum
  xcm=p->pos[1][1];
  ycm=p->pos[1][2];
  zcm=p->pos[1][3];
  vxcm=p->vel[1][1];
  vycm=p->vel[1][2];
  vzcm=p->vel[1][3];
  potmin=p->pot[1];
  bndm=p->m[1];

  bndm_fe = 0.0;
  bndm_si = 0.0;
  bndm_cr = 0.0;
  totm = 0.0;

  // find the particle closest to the potential minimum
  for( i = 1; i <= p->Ntot; i++ ) {
    p->bnd[i] = 0;
  
    //for a lower-potential particle, use this as the seed instead
    if( p->pot[i] < potmin ) {
      xcm=p->pos[i][1];
      ycm=p->pos[i][2];
      zcm=p->pos[i][3];
      vxcm=p->vel[i][1];
      vycm=p->vel[i][2];
      vzcm=p->vel[i][3];
      potmin=p->pot[i];	    
      bndm=p->m[i];
    }
  }

  //set so that the iteration can at least start
  oldm = bndm/10.0;

  //iterate
  count = 0;
  while( (fabs(oldm-bndm)/oldm > tol) && (count < maxit) ) {
    oldm = bndm;

    //search throuh particles, finding ones bound w.r.t. the seed
    for( i = 1; i <= p->Ntot; i++ ) {
      
      //squared relative velocity to centre-of-bound-mass
      v2 = (p->vel[i][1]-vxcm)*(p->vel[i][1]-vxcm) + (p->vel[i][2]-vycm)*(p->vel[i][2]-vycm) + (p->vel[i][3]-vzcm)*(p->vel[i][3]-vzcm);
      //distance from centre of bound mass
      rad = sqrt((p->pos[i][1]-xcm)*(p->pos[i][1]-xcm) + (p->pos[i][2]-ycm)*(p->pos[i][2]-ycm) + (p->pos[i][3]-zcm)*(p->pos[i][3]-zcm));

      //kinetic & potential energy in this COM frame
      ke = 0.5*p->m[i]*v2;
      pe = -G*bndm*p->m[i]/rad;
	    
      if( ke+pe < 0.0 ) {  // particle is bound
	p->bnd[i] = 1;
      } else {
	p->bnd[i] = 0;
      }
    }
	
    //reset bound mass data
    xcm = ycm = zcm = 0.0;
    vxcm = vycm = vzcm = 0.0;
    bndm = 0.0;

    // calculate bound mass data for this iteration step
    for( i = 1; i <= p->Ntot; i++ ) {
      if(p->bnd[i] == 1) {
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

    count++;

  }

  //print status of iteration
  printf( "Iteration completed in %d/%d steps.\nFinal fractional difference in bound mass: %g\nTollerance value: %g\n\n", count, maxit, fabs(bndm-oldm)/oldm, tol );

  /* Find the bound masses of mantle (silicate) and core (iron materials)
   * based on the idskip and iron being material 1, silicates being material 2
   */
  for( i = 1; i <= p->Ntot; i++ ) {
    if ( p->bnd[i] == 1 ) {
      if( calculate_material(p->id[i]) == 1 ) {
	bndm_fe += p->m[i];
      } else if( calculate_material(p->id[i]) == 2 ) {
	bndm_si += p->m[i];
      } else if( calculate_material(p->id[i]) == 3 ) {
	bndm_cr += p->m[i];
      }
    }
    totm += p->m[i];
  }

  printf("\nTotal bound mass: %g g (%g), %g M_Merc, Silicates: %g g (%g%%), Iron: %g g (%g%%), Crust: %g g (%g%%)\n", bndm, bndm/totm, bndm/M_MERC, bndm_si, bndm_si/bndm*100., bndm_fe, bndm_fe/bndm*100., bndm_cr, bndm_cr/bndm*100.);

  return;
    
}



int calculate_material( int id ) {

  int i;

  for( i = 1; i <= nMat; i++ ) {
    if( id < i*idskip )
      break;
  }
  return i;

}
