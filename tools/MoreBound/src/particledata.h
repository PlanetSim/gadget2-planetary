#ifndef _PARTICLE_DATA_H
#define _PARTICLE_DATA_H

#include "globalvars.h"

typedef struct _FVec3 {
  float x, y, z;
} FVec3;

float fvec_square_distance(FVec3 vel1, FVec3 vel2);

typedef struct _ParticleData {
  int *id;
  int *bnd;
  float *pos, *vel, *mass, *s, *rho, *hsml, *potential;

  double total_mass;
  int total_number;
} ParticleData;

int pd_init(ParticleData *pd, size_t N);
void pd_free(ParticleData *pd);
int pd_read(ParticleData *pd, IOHeader *header);

FVec3 pd_get_vel(ParticleData *pd, size_t i);
FVec3 pd_get_pos(ParticleData *pd, size_t i);
void pd_set_vel(ParticleData *pd, size_t i, FVec3 vel);
void pd_set_pos(ParticleData *pd, size_t i, FVec3 pos);

void calculate_binding(ParticleData *pd);
#endif
