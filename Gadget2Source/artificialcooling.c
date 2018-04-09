#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "allvars.h"
#include "proto.h"
#include "eos.h"

/*! \file artificialcooling.c
 *  \brief Routine to artificially cool SPH particles. JMAA 8/2011
 */


/*! Function artifically cools all SPH particles by removing both kinetic & thermal energy.
 *  Kinetic energy is taken by multiplying all velocities by a restitution factor.
 *  Thermal energy is removed by reducing entropy by a factor of the difference from some 
 *  ideal value given in the parameter file. Each particle loses a proportion defined by 
 *  its own internal energy w.r.t. the total.
 *  This involves totaling the internal energy of all SPH particles. Note that this process,
 *  for very high resolution runs, may run into overflow problems due to dumb way in which
 *  the values are summed. If this becomes a problem then it would be possible to change this
 *  sum to something more sensible (recursively summing as a tree, for example)
 */
void artificialcooling( void ) {

  int i, j;
  double t0, t1;
  double Utot = 0;  // total int. erg. this timestep
  double Utotlocal = 0; // total int. erg. on this processor
  double dU = 0;    // difference from ideal
  double dS = 0;    // amount to reduce entropy by
  double T = 0;     // temperature of that particle
  int ReduceEntropies = 0;  // flag which is only set true if we're outside the tollerance
  double * intenergies = malloc( NumPart * sizeof(double) );  //stores the internal energies of sph particles so we only calculate them once

  t0 = second();

  // preliminaries for reducing entropy
  if( All.ReduceEnt == 1 ) {
    // get the tot
    for( i = 0; i < NumPart; i++ ) {
      if( P[i].Type == 0 ) { // only count SPH particles
	intenergies[i] = eos_table_interp( SphP[i].Density, SphP[i].Entropy, P[i].ID, EOS_INT_ENERGY, EOS_EXTRAP )*P[i].Mass;
	Utotlocal += intenergies[i];
      }
    }
    // now we need to reduce this across all processors
    MPI_Allreduce( &Utotlocal, &Utot, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
    // decide if we need to reduce entropy
    if( Utot > ((All.UTollFac + 1)*All.UTotIdeal) ) {
      ReduceEntropies = 1;
      dU = Utot - All.UTotIdeal;
    }
  }

  for( i = 0; i < NumPart; i++ ) {
    // Select only SPH particles
    if( P[i].Type == 0 ) {

      // reduce velocities
      if( All.ReduceVels != 0 ) {
	for( j = 0; j < 3; j++ ) {
	  P[i].Vel[j] *= All.RestitutionFac;
	}
      }

      // reduce entropies
      if( ReduceEntropies == 1 ) {
	T = eos_table_interp( SphP[i].Density, SphP[i].Entropy, P[i].ID, EOS_TEMPERATURE, EOS_EXTRAP );
	dS = (intenergies[i]/Utot) * dU * ( 1/(P[i].Mass * T) ) * All.UReductionFac;
	SphP[i].Entropy -= dS;
        printf ("Utot %6.4e  UtotIdeal %6.4e \n", Utot, All.UTotIdeal);
      }

      // set entropies
      if( All.ReduceEnt == -1 ) {
	if( P[i].ID > EOS_ID_SKIP ) {
	  SphP[i].Entropy = All.UTotIdeal; /* mantle */
        } else {
	  SphP[i].Entropy = All.UTollFac; /* core */
	}
      }

    }
  }

  t1 = second();
  All.CPU_TimeLine += timediff(t0, t1);

}
