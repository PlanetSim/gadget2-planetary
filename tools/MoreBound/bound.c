#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"


int calculate_material( int id );
double calculate_potential( struct particle_data *p, int index, int remnant );


/*
 * Finds particle closest to potential minimum, then uses this as a seed for
 * the iterative procedure.
 * Given a selection of particles known to be bound, find all particles
 * bound w.r.t. them & add these to the list of bound particles.
 * Repeat until the bound mass change <= tol between iterations or until 
 * maxit is exceeded
 */
void bound( struct particle_data *p ) {

  int i,j, count;
  double potmin;
  double v2, rad, ke, pe; //particle w.r.t. CO-bound-M data
  double oldm, bndm, bndm_fe, bndm_si; //data from last iteration
  double x, y, z, vx, vy, vz; //centre-of-bound-mass data
  double xm, ym, zm, vxm, vym, vzm; //as above, weigthed by mass
  double bndm_cr, totm;
  int remnant = 1; //ordinal representing the remnant that we're finding
  int nbound; // number of particles bound to the current remnant
  double pot; // potential of the particle discounting all particles already considered to be bound
  int seedindex;
  short int first = 1;

  //set all particles as unbound
  for( i = 1; i <= p->Ntot; i++ ) {
    p->bnd[i] = 0;
  }

  //loop through remnants
  do {

    //reset bound mass data
    xm = ym = zm = x = y = z = 0.0;
    vxm = vym = vzm = vx = vy = vz = 0.0;
    bndm = 0;
    nbound = 1;
    potmin = 0;

    // find the particle closest to the potential minimum
    first = 1;
    for( i = 1; i <= p->Ntot; i++ ) {
      
      if( p->bnd[i] != 0 ) {
	continue; //ignore if this particle's already bound to something
      }

      pot = calculate_potential( p, i, remnant );

      //for a lower-potential particle, use this as the seed instead (or if it's the first valid particle)
      if( (first == 1) || ( pot < potmin) ) { //p->pot[i]
	xm = p->pos[i][1]*p->m[i];
	ym = p->pos[i][2]*p->m[i];
	zm = p->pos[i][3]*p->m[i];
	vxm = p->vel[i][1]*p->m[i];
	vym = p->vel[i][2]*p->m[i];
	vzm = p->vel[i][3]*p->m[i];
	potmin = pot;
	bndm = p->m[i];
	seedindex = i;
	if( first == 1 ) first = 0;
      }
//      if(remnant>1) printf("Pot: %g, Potmin: %g\n",pot,potmin);
    }


    p->bnd[seedindex] = remnant; // set the seed as bound

    //set so that the iteration can at least start
    oldm = bndm/10.0;

    //iterate
    count = 0;
    while( (fabs(oldm-bndm)/oldm > tol) && (count < maxit) ) {
      oldm = bndm;

      x = xm/bndm;
      y = ym/bndm;
      z = zm/bndm;
      vx = vxm/bndm;
      vy = vym/bndm;
      vz = vzm/bndm;

      //search throuh particles, finding ones bound w.r.t. the seed
      for( i = 1; i <= p->Ntot; i++ ) {
      
	if( p->bnd[i] != 0 ) continue; //ignore if already bound to something

	//squared relative velocity to centre-of-bound-mass
	v2 = (p->vel[i][1]-vx)*(p->vel[i][1]-vx) + (p->vel[i][2]-vy)*(p->vel[i][2]-vy) + (p->vel[i][3]-vz)*(p->vel[i][3]-vz);
	//distance from centre of bound mass
	rad = sqrt((p->pos[i][1]-x)*(p->pos[i][1]-x) + (p->pos[i][2]-y)*(p->pos[i][2]-y) + (p->pos[i][3]-z)*(p->pos[i][3]-z));

	//kinetic & potential energy in this COM frame
	ke = 0.5*p->m[i]*v2;
	pe = -G*bndm*p->m[i]/rad;
	    
	if( ke+pe < 0.0 ) {  // particle is bound
	  p->bnd[i] = remnant;
	  bndm += p->m[i];
	  xm  += p->pos[i][1]*p->m[i];
	  ym  += p->pos[i][2]*p->m[i];
	  zm  += p->pos[i][3]*p->m[i];
	  vxm += p->vel[i][1]*p->m[i];
	  vym += p->vel[i][2]*p->m[i];
	  vzm += p->vel[i][3]*p->m[i];
	  nbound += 1; 
	}

      }
	
      count++;

    }

    //print status of iteration
    printf( "REMNANT %d: Iteration completed in %d/%d steps. Mass of %gg.\nFinal fractional difference in bound mass: %g\nTollerance value: %g\nNumber of particles: %d\n\n", remnant, count, maxit, bndm, fabs(bndm-oldm)/oldm, tol, nbound );

    remnant++;

  } while ( nbound >= minparts || remnant<4 );


  /* Check correct order and rearrange if required */
  double masses[remnant+1];
  int reorder=0;
  double temp;

  for(j=1;j<remnant;j++){
    totm=0;
    for( i = 1; i <= p->Ntot; i++ ) {
      if ( p->bnd[i] == j )
        totm += p->m[i];
    }
    masses[j] = totm;
  }
  do{
      reorder = 0;
      for(j=2;j<remnant;j++){
        if( masses[j] > masses[j-1] ){
            for( i = 1; i <= p->Ntot; i++ ) {
              if ( p->bnd[i] == j-1 )
                p->bnd[i] = j;
              else if ( p->bnd[i] == j )
                p->bnd[i] = j-1;
            }
            reorder = 1;
            temp=masses[j-1];
            masses[j-1]=masses[j];
            masses[j]=temp;
            printf("%d %d swapped\n",j-1,j);
        }
      }
  } while (reorder == 1);


  /* Find the bound masses of mantle (silicate) and core (iron materials)
   * based on the idskip and iron being material 1, silicates being material 2
   */
  for( j=1; j<remnant; j++ ){
    bndm_fe=0;
    bndm_si=0;
    bndm_cr=0;
    bndm=0;
    totm=0;
    for( i = 1; i <= p->Ntot; i++ ) {
      if ( p->bnd[i] == j ) {
        if( calculate_material(p->id[i]) == 1 ) {
	  bndm_fe += p->m[i];
        } else if( calculate_material(p->id[i]) == 2 ) {
	  bndm_si += p->m[i];
        } else if( calculate_material(p->id[i]) == 3 ) {
	  bndm_cr += p->m[i];
        }
	else printf("Material error: %d\n",i);
	bndm += p->m[i];
      }
      totm += p->m[i];
    }

    printf("REMNANT %d: Total bound mass: %g g (%g), Silicates: %g g (%g%%), Iron: %g g (%g%%), Crust: %g g (%g%%)\n", j, bndm, bndm/totm, bndm_si, bndm_si/bndm*100., bndm_fe, bndm_fe/bndm*100., bndm_cr, bndm_cr/bndm*100.);
  }



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



/*
 * Calculates the potential of a particle w.r.t. all particles /not/ bound by a larger
 * remnant
 */
double calculate_potential( struct particle_data *p, int index, int remnant ) {

  double potential = 0;
  double sep;
  int i;

  if( remnant == 1 ) {   //for the largest remnant this is just the already-known potential
    return p->pot[index];
  }

  //loop through particles, adding in the potentials
  for( i = 1; i <= p->Ntot; i++ ) {
    if( (p->bnd[i] < remnant && p->bnd[i] != 0) || i == index ) { //we can ignore this particle
      continue;
    }
    sep = sqrt( (p->pos[i][1] - p->pos[index][1])*(p->pos[i][1] - p->pos[index][1]) +
                (p->pos[i][2] - p->pos[index][2])*(p->pos[i][2] - p->pos[index][2]) +
                (p->pos[i][3] - p->pos[index][3])*(p->pos[i][3] - p->pos[index][3]) );

    if(sep>10.e5)
      potential -= G*p->m[i] / sep;
  }

//  if(remnant>1)
//	printf("Pot: %g\n",potential);

  return potential;

}
