#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "globalvars.h"
#include "particledata.h"

#define IS_UNBOUND 0

int calculate_material(int id);
double calculate_potential(ParticleData *pd, size_t index, int remnant);

FVec3 fvec_mult(FVec3 v, float m) {
  v.x *= m;
  v.y *= m;
  v.z *= m;
  return v;
}

typedef struct _CentreOfMass {
  FVec3 pos, vel;
} CentreOfMass;

void update_centre_of_mass(CentreOfMass *com, ParticleData *pd, size_t i) {
  com->pos = fvec_mult(pd_get_pos(pd, i), pd->mass[i]);
  com->vel = fvec_mult(pd_get_vel(pd, i), pd->mass[i]);
}

size_t find_particle_potential_min(ParticleData *pd, int remnant) {
  size_t selected = 0;
  double potmin = calculate_potential(pd, selected, remnant);

  for (int i = 1; i < pd->total_number; ++i) {
    if (pd->bnd[i] != IS_UNBOUND) {
      // ignore if particle is already bound to something
      continue;
    }

    double pot = calculate_potential(pd, i, remnant);
    if (pot < potmin) {
      potmin = pot;
      selected = i;
    }
  }

  return selected;
}

float fvec_square_distance(FVec3 vel1, FVec3 vel2) {
  float v1 = (vel1.x - vel2.x);
  float v2 = (vel1.y - vel2.y);
  float v3 = (vel1.y - vel2.z);
  return (v1 * v1) + (v2 * v2) + (v3 * v3);
}

FVec3 fvec_zero() { return (FVec3){0, 0, 0}; }

size_t find_and_update_bound(ParticleData *pd, CentreOfMass data,
                             double *total_bound_mass, int remnant) {
  size_t num_bound = 0;
  double bound_mass = *total_bound_mass;
  for (size_t i = 0; i < pd->total_number; ++i) {
    if (!(pd->bnd[i] == IS_UNBOUND)) {
      continue;
    }

    float rel_velocity = fvec_square_distance(pd_get_vel(pd, i), data.vel);
    float distance = sqrt(fvec_square_distance(pd_get_pos(pd, i), data.pos));
    // kinetic & potential energy in this COM frame
    float ke = 0.5 * pd->mass[i] * rel_velocity;
    float pe = -G * bound_mass * pd->mass[i] / distance;

    if (ke + pe < 0) {
      // particle is bound
      pd->bnd[i] = remnant;
      update_centre_of_mass(&data, pd, i);
      num_bound += 1;
      bound_mass += pd->mass[i];
    }
  }
  *total_bound_mass = bound_mass;
  return num_bound;
}

int calculate_remnant(ParticleData *pd, int remnant) {
  CentreOfMass weighted = (CentreOfMass){fvec_zero(), fvec_zero()};

  // number of particles bound to the current remnant
  int nbound = 1;

  size_t seedindex = find_particle_potential_min(pd, remnant);
  // set the seed as bound to current remnant
  pd->bnd[seedindex] = remnant;

  // update the center of mass
  update_centre_of_mass(&weighted, pd, seedindex);

  double old_bound_mass, bound_mass = pd->mass[seedindex];

  int count = 0;
  while (count < maxit) {
    old_bound_mass = bound_mass;

    double ibm = 1 / bound_mass;

    // search through particles, finding ones bound w.r.t. the seed
    CentreOfMass current = (CentreOfMass){
        fvec_mult(weighted.pos, ibm),
        fvec_mult(weighted.vel, ibm),
    };
    nbound += find_and_update_bound(pd, current, &bound_mass, remnant);
    ++count;

    double converged =
        (fabs(old_bound_mass - bound_mass) / old_bound_mass) < tol;
    if (converged)
      break;
  }

  // print status of iteration
  printf("REMNANT %d: Iteration completed in %d/%d steps. Mass of "
         "%gg.\nNumber of particles: %d\n\n",
         remnant, count, maxit, bound_mass, nbound);
  return nbound;
}

/*
 * Finds particle closest to potential minimum, then uses this as a seed for
 * the iterative procedure.
 * Given a selection of particles known to be bound, find all particles
 * bound w.r.t. them & add these to the list of bound particles.
 * Repeat until the bound mass change <= tol between iterations or until
 * maxit is exceeded
 */
void calculate_binding(ParticleData *pd) {
  size_t j, count;
  double bndm_fe, bndm_si; // data from last iteration
  double bndm_cr, totm;

  // set all particles as unbound
  for (size_t i = 0; i < pd->total_number; ++i) {
    pd->bnd[i] = 0;
  }
    
  int total_bound = 0;
  // ordinal representing the remnant that we're finding
  for (int remnant = 1; remnant < 4 || total_bound >= minparts; ++remnant) {
    total_bound += calculate_remnant(pd, remnant);
  }

  /* Check correct order and rearrange if required */
  /* double masses[remnant + 1]; */
  /* int reorder = 0; */
  /* double temp; */

  /* for (j = 1; j < remnant; j++) { */
  /*   totm = 0; */
  /*   for (i = 1; i <= p->Ntot; i++) { */
  /*     if (p->bnd[i] == j) */
  /*       totm += p->m[i]; */
  /*   } */
  /*   masses[j] = totm; */
  /* } */
  /* do { */
  /*   reorder = 0; */
  /*   for (j = 2; j < remnant; j++) { */
  /*     if (masses[j] > masses[j - 1]) { */
  /*       for (i = 1; i <= p->Ntot; i++) { */
  /*         if (p->bnd[i] == j - 1) */
  /*           p->bnd[i] = j; */
  /*         else if (p->bnd[i] == j) */
  /*           p->bnd[i] = j - 1; */
  /*       } */
  /*       reorder = 1; */
  /*       temp = masses[j - 1]; */
  /*       masses[j - 1] = masses[j]; */
  /*       masses[j] = temp; */
  /*       printf("%d %d swapped\n", j - 1, j); */
  /*     } */
  /*   } */
  /* } while (reorder == 1); */

  /* Find the bound masses of mantle (silicate) and core (iron materials)
   * based on the idskip and iron being material 1, silicates being material 2
   */
  /* for (j = 1; j < remnant; j++) { */
  /*   bndm_fe = 0; */
  /*   bndm_si = 0; */
  /*   bndm_cr = 0; */
  /*   bndm = 0; */
  /*   totm = 0; */
  /*   for (i = 1; i <= p->Ntot; i++) { */
  /*     if (p->bnd[i] == j) { */
  /*       if (calculate_material(p->id[i]) == 1) { */
  /*         bndm_fe += p->m[i]; */
  /*       } else if (calculate_material(p->id[i]) == 2) { */
  /*         bndm_si += p->m[i]; */
  /*       } else if (calculate_material(p->id[i]) == 3) { */
  /*         bndm_cr += p->m[i]; */
  /*       } else */
  /*         printf("Material error: %d\n", i); */
  /*       bndm += p->m[i]; */
  /*     } */
  /*     totm += p->m[i]; */
  /*   } */

  /*   printf("REMNANT %d: Total bound mass: %g g (%g), Silicates: %g g (%g%%),
   * " */
  /*          "Iron: %g g (%g%%), Crust: %g g (%g%%)\n", */
  /*          j, bndm, bndm / totm, bndm_si, bndm_si / bndm * 100., bndm_fe, */
  /*          bndm_fe / bndm * 100., bndm_cr, bndm_cr / bndm * 100.); */
  /* } */
  return;
}

int calculate_material(int id) {
  for (int i = 1; i <= nMat; i++) {
    if (id < i * idskip)
      return i;
  }
  return nMat;
}

/*
 * Calculates the potential of a particle w.r.t. all particles /not/ bound by a
 * larger remnant
 */
double calculate_potential(ParticleData *pd, size_t index, int remnant) {
  // for the largest remnant this is just the already-known potential
  if (remnant == 1) {
    return pd->potential[index];
  }

  double potential = 0;
  // loop through particles, adding in the potentials
  FVec3 reference = pd_get_pos(pd, index);
  for (size_t i = 0; i < pd->total_number; ++i) {
    if (!(pd->bnd[i] == IS_UNBOUND) && pd->bnd[i] < remnant) {
        continue;
    }
    if (i == index) {
        continue;
    }
    
    float seperation = fvec_square_distance(reference, pd_get_pos(pd, i));
    // TODO: is this right? only update potential if they are really far away?
    if (seperation > 10.e5) {
      potential -= G * pd->mass[i] / seperation;
    }
  }

  return potential;
}
