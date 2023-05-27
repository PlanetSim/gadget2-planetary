#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#include "globalvars.h"
#include "particledata.h"

#define N_THREADS 12

#define IS_UNBOUND == 0
#define IS_BOUND != 0

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
    if (pd->bnd[i] IS_BOUND && pd->bnd[i] < remnant) {
      continue;
    }
    if (i == index) {
      continue;
    }

    float seperation =
        fvec_square_distance(reference, pd_get_pos(pd, i));
    // TODO: is this right? only update potential if they are really far away?
    if (seperation > 1e10) {
      potential -= G * pd->mass[i] / sqrtf(seperation);
    }
  }

  return potential;
}

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

typedef struct _MinPotentialThreadStorage {
    ParticleData *pd;
    int remnant;
    double minimum_potential;
    size_t index;
    size_t start;
    size_t end;
} MinPotentialThreadStorage;

int worker_find_min_potential(void *arg) {
  MinPotentialThreadStorage *ts = (MinPotentialThreadStorage*) arg;
  ParticleData *pd = ts->pd;
  int remnant = ts->remnant;

  size_t selected = 0;
  double potmin = calculate_potential(pd, selected, remnant);

  for (size_t i = ts->start; i < ts->end; ++i) {
    if (pd->bnd[i] IS_BOUND) {
      // ignore if particle is already bound to something
      continue;
    }

    double pot = calculate_potential(pd, i, remnant);
    if (pot < potmin) {
      potmin = pot;
      selected = i;
    }
  }

  ts->minimum_potential = potmin;
  ts->index = selected;
  return 0;
}

size_t find_particle_potential_min(ParticleData *pd, int remnant) {
    thrd_t threads[N_THREADS - 1];
    MinPotentialThreadStorage storage[N_THREADS - 1];
    const size_t workload = pd->total_number / N_THREADS;
    
    size_t i;
    // launch all worker threads
    for (i = 0; i < N_THREADS - 1; ++i) {
        size_t start = i * workload;
        storage[i] = (MinPotentialThreadStorage) {
            pd, remnant, 0, 0, start, start + workload,
        };
        thrd_create(&threads[i], worker_find_min_potential, &storage[i]);
    }

    MinPotentialThreadStorage current = (MinPotentialThreadStorage) {
        pd, remnant, 0, 0, (N_THREADS - 1) * workload, pd->total_number
    };
    // launch current thread
    worker_find_min_potential((void *) &current);

    // join threads and aggregate results
    for (i = 0; i < N_THREADS - 1; ++i) {
        thrd_join(threads[i], NULL);
        // if a thread found somethin better, update the current
        if (current.minimum_potential > storage[i].minimum_potential) {
            current.minimum_potential = storage[i].minimum_potential;
            current.index = storage[i].index;
        }
    }
    return current.index;
}

float fvec_square_distance(FVec3 vel1, FVec3 vel2) {
  float v1 = (vel1.x - vel2.x);
  float v2 = (vel1.y - vel2.y);
  float v3 = (vel1.z - vel2.z);
  return (v1 * v1) + (v2 * v2) + (v3 * v3);
}

FVec3 fvec_zero() { return (FVec3){0, 0, 0}; }

size_t find_and_update_bound(ParticleData *pd, CentreOfMass data,
                             double *total_bound_mass, int remnant) {

  size_t num_bound = 0;
  double mass_total = *total_bound_mass;

  for (size_t i = 0; i < pd->total_number; ++i) {
    if (pd->bnd[i] IS_BOUND) {
      continue;
    }

    float rel_velocity = fvec_square_distance(pd_get_vel(pd, i), data.vel);
    float distance = sqrt(fvec_square_distance(pd_get_pos(pd, i), data.pos));

    // kinetic & potential energy in this COM frame
    float ke = 0.5 * pd->mass[i] * rel_velocity;
    float pe = -G * mass_total * pd->mass[i] / distance;

    if (ke + pe < 0) {
      // particle is bound
      pd->bnd[i] = remnant;
      update_centre_of_mass(&data, pd, i);
      num_bound += 1;
      mass_total += pd->mass[i];
    }
  }

  *total_bound_mass = mass_total;
  return num_bound;
}

int calculate_remnant(ParticleData *pd, int remnant) {
  CentreOfMass weighted = (CentreOfMass){fvec_zero(), fvec_zero()};

  // number of particles bound to the current remnant
  int nbound = 1;
    
  printf("Finding potential minimum\n");
  size_t seedindex = find_particle_potential_min(pd, remnant);
  printf("Seedindex found %ld\n", seedindex);

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
  return;
}
