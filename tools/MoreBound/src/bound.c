#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "globalvars.h"

#define IS_UNBOUND 0

int calculate_material(int id);
double calculate_potential(struct particle_data *p, int index, int remnant);

typedef struct _CentreOfMass {
  double x, y, z, vx, vy, vz;
} CentreOfMass;

void update_centre_of_mass(CentreOfMass *com, struct particle_data *p,
                           size_t i) {
  com->x = p->pos[i][1] * p->m[i];
  com->y = p->pos[i][2] * p->m[i];
  com->z = p->pos[i][3] * p->m[i];
  com->vx = p->vel[i][1] * p->m[i];
  com->vy = p->vel[i][2] * p->m[i];
  com->vz = p->vel[i][3] * p->m[i];
}

size_t find_particle_potential_min(struct particle_data *p, int remnant) {
  size_t selected = 1;
  double potmin = calculate_potential(p, selected, remnant);

  for (size_t i = 2; i <= p->Ntot; ++i) {
    if (p->bnd[i] != IS_UNBOUND) {
      // ignore if particle is already bound to something
      continue;
    }

    double pot = calculate_potential(p, i, remnant);
    if (pot < potmin) {
      potmin = pot;
      selected = i;
    }
  }

  return selected;
}

float square_relative_velocity(float vel[3], CentreOfMass com) {
  float v1 = (vel[1] - com.vx);
  float v2 = (vel[2] - com.vy);
  float v3 = (vel[3] - com.vz);
  return (v1 * v1) + (v2 * v2) + (v3 * v3);
}

float square_distance(float pos[3], CentreOfMass com) {
  float x = (pos[1] - com.x);
  float y = (pos[2] - com.y);
  float z = (pos[3] - com.z);
  return (x * x) + (y * y) + (z * z);
}

size_t find_and_update_bound(struct particle_data *p, CentreOfMass data,
                             double *total_bound_mass, size_t remnant) {
  size_t num_bound = 0;
  double bound_mass = *total_bound_mass;
  for (size_t i = 1; i <= p->Ntot; ++i) {
    if (p->bnd[i] != IS_UNBOUND) {
      continue;
    }

    float rel_velocity = square_relative_velocity(p->vel[i], data);
    float distance = sqrt(square_distance(p->pos[i], data));
    // kinetic & potential energy in this COM frame
    float ke = 0.5 * p->m[i] * rel_velocity;
    float pe = -G * bound_mass * p->m[i] / distance;

    if (ke + pe < 0) {
      // particle is bound
      p->bnd[i] = remnant;
      update_centre_of_mass(&data, p, i);
      num_bound += 1;
      bound_mass += p->m[i];
    }
  }
  *total_bound_mass = bound_mass;
  return num_bound;
}

/*
 * Finds particle closest to potential minimum, then uses this as a seed for
 * the iterative procedure.
 * Given a selection of particles known to be bound, find all particles
 * bound w.r.t. them & add these to the list of bound particles.
 * Repeat until the bound mass change <= tol between iterations or until
 * maxit is exceeded
 */
void bound(struct particle_data *p) {

  int i, j, count;
  double oldm, bndm, bndm_fe, bndm_si; // data from last iteration
  CentreOfMass data;
  CentreOfMass mass_weighted;

  double bndm_cr, totm;
  int remnant = 1; // ordinal representing the remnant that we're finding
  int nbound;      // number of particles bound to the current remnant

  // set all particles as unbound
  for (i = 1; i <= p->Ntot; i++) {
    p->bnd[i] = 0;
  }

  // loop through remnants
  do {
    mass_weighted = (CentreOfMass){0, 0, 0, 0, 0, 0};
    // reset bound mass data
    nbound = 1;

    size_t seedindex = find_particle_potential_min(p, remnant);
    update_centre_of_mass(&mass_weighted, p, seedindex);
    bndm = p->m[seedindex];

    p->bnd[seedindex] = remnant; // set the seed as bound

    // set so that the iteration can at least start
    oldm = bndm / 10.0;

    // iterate
    count = 0;
    while ((fabs(oldm - bndm) / oldm > tol) && (count < maxit)) {
      oldm = bndm;

      double ibndm = 1 / bndm;

      data.x = mass_weighted.x * ibndm;
      data.y = mass_weighted.y * ibndm;
      data.z = mass_weighted.z * ibndm;
      data.vx = mass_weighted.vx * ibndm;
      data.vy = mass_weighted.vy * ibndm;
      data.vz = mass_weighted.vz * ibndm;

      // search through particles, finding ones bound w.r.t. the seed
      nbound += find_and_update_bound(p, data, &bndm, remnant);
      count++;
    }

    // print status of iteration
    printf("REMNANT %d: Iteration completed in %d/%d steps. Mass of "
           "%gg.\nFinal fractional difference in bound mass: %g\nTollerance "
           "value: %g\nNumber of particles: %d\n\n",
           remnant, count, maxit, bndm, fabs(bndm - oldm) / oldm, tol, nbound);

    remnant++;

  } while (nbound >= minparts || remnant < 4);

  /* Check correct order and rearrange if required */
  double masses[remnant + 1];
  int reorder = 0;
  double temp;

  for (j = 1; j < remnant; j++) {
    totm = 0;
    for (i = 1; i <= p->Ntot; i++) {
      if (p->bnd[i] == j)
        totm += p->m[i];
    }
    masses[j] = totm;
  }
  do {
    reorder = 0;
    for (j = 2; j < remnant; j++) {
      if (masses[j] > masses[j - 1]) {
        for (i = 1; i <= p->Ntot; i++) {
          if (p->bnd[i] == j - 1)
            p->bnd[i] = j;
          else if (p->bnd[i] == j)
            p->bnd[i] = j - 1;
        }
        reorder = 1;
        temp = masses[j - 1];
        masses[j - 1] = masses[j];
        masses[j] = temp;
        printf("%d %d swapped\n", j - 1, j);
      }
    }
  } while (reorder == 1);

  /* Find the bound masses of mantle (silicate) and core (iron materials)
   * based on the idskip and iron being material 1, silicates being material 2
   */
  for (j = 1; j < remnant; j++) {
    bndm_fe = 0;
    bndm_si = 0;
    bndm_cr = 0;
    bndm = 0;
    totm = 0;
    for (i = 1; i <= p->Ntot; i++) {
      if (p->bnd[i] == j) {
        if (calculate_material(p->id[i]) == 1) {
          bndm_fe += p->m[i];
        } else if (calculate_material(p->id[i]) == 2) {
          bndm_si += p->m[i];
        } else if (calculate_material(p->id[i]) == 3) {
          bndm_cr += p->m[i];
        } else
          printf("Material error: %d\n", i);
        bndm += p->m[i];
      }
      totm += p->m[i];
    }

    printf("REMNANT %d: Total bound mass: %g g (%g), Silicates: %g g (%g%%), "
           "Iron: %g g (%g%%), Crust: %g g (%g%%)\n",
           j, bndm, bndm / totm, bndm_si, bndm_si / bndm * 100., bndm_fe,
           bndm_fe / bndm * 100., bndm_cr, bndm_cr / bndm * 100.);
  }

  return;
}

int calculate_material(int id) {

  int i;

  for (i = 1; i <= nMat; i++) {
    if (id < i * idskip)
      break;
  }
  return i;
}

/*
 * Calculates the potential of a particle w.r.t. all particles /not/ bound by a
 * larger remnant
 */
double calculate_potential(struct particle_data *p, int index, int remnant) {

  double potential = 0;
  double sep;
  int i;

  if (remnant ==
      1) { // for the largest remnant this is just the already-known potential
    return p->pot[index];
  }

  // loop through particles, adding in the potentials
  for (i = 1; i <= p->Ntot; i++) {
    if ((p->bnd[i] < remnant && p->bnd[i] != 0) ||
        i == index) { // we can ignore this particle
      continue;
    }
    sep = sqrt(
        (p->pos[i][1] - p->pos[index][1]) * (p->pos[i][1] - p->pos[index][1]) +
        (p->pos[i][2] - p->pos[index][2]) * (p->pos[i][2] - p->pos[index][2]) +
        (p->pos[i][3] - p->pos[index][3]) * (p->pos[i][3] - p->pos[index][3]));

    if (sep > 10.e5)
      potential -= G * p->m[i] / sep;
  }

  //  if(remnant>1)
  //	printf("Pot: %g\n",potential);

  return potential;
}
