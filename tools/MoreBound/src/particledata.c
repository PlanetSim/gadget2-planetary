#include "particledata.h"

#include <stdio.h>
#include <stdlib.h>

int pd_init(ParticleData *pd, size_t N) {
  int *ibuffer = (int *)malloc(sizeof(int) * 2 * N);
  if (!ibuffer) {
    return -1;
  }

  float *fbuffer = (float *)malloc(sizeof(float) * 11 * N);
  if (!fbuffer) {
    free(ibuffer);
    return -2;
  }

  pd->id = ibuffer;
  pd->bnd = &ibuffer[N];

  pd->pos = fbuffer;
  pd->vel = &fbuffer[3 * N];
  pd->mass = &fbuffer[6 * N];
  pd->s = &fbuffer[7 * N];
  pd->rho = &fbuffer[8 * N];
  pd->hsml = &fbuffer[9 * N];
  pd->potential = &fbuffer[10 * N];

  pd->total_mass = 0;
  pd->total_number = 0;
  return 0;
}

void pd_free(ParticleData *pd) {
  free(pd->id);
  free(pd->pos);
}

FVec3 pd_get_vel(const ParticleData *pd, size_t i) {
  FVec3 v;
  size_t offset = i * 3;
  v.x = pd->vel[offset];
  v.y = pd->vel[offset + 1];
  v.z = pd->vel[offset + 2];
  return v;
}

FVec3 pd_get_pos(const ParticleData *pd, size_t i) {
  FVec3 v;
  size_t offset = i * 3;
  v.x = pd->pos[offset];
  v.y = pd->pos[offset + 1];
  v.z = pd->pos[offset + 2];
  return v;
}

void pd_set_pos(ParticleData *pd, size_t i, FVec3 pos) {
  size_t offset = i * 3;
  pd->pos[offset] = pos.x;
  pd->pos[offset + 1] = pos.y;
  pd->pos[offset + 2] = pos.z;
}

void pd_set_vel(ParticleData *pd, size_t i, FVec3 vel) {
  size_t offset = i * 3;
  pd->vel[offset] = vel.x;
  pd->vel[offset + 1] = vel.y;
  pd->vel[offset + 2] = vel.z;
}

int pd_read(ParticleData *pd, IOHeader *header) {
  FILE *fd = fopen(infile, "r");
  int status = 0;

  if (!fd) {
    printf("Could not find file '%s'\n", infile);
    status = -1;
    goto EXIT;
  }

  fseek(fd, sizeof(int), SEEK_CUR);
  fread(header, sizeof(IOHeader), 1, fd);

  size_t N = header->npart[0];

  if (pd_init(pd, N) < 0) {
    printf("Could not allocate particle data\n");
    status = -2;
    goto EXIT;
  }

  fseek(fd, 2 * sizeof(int), SEEK_CUR);
  fread(pd->pos, sizeof(float), 3 * N, fd);

  fseek(fd, 2 * sizeof(int), SEEK_CUR);
  fread(pd->vel, sizeof(float), 3 * N, fd);

  fseek(fd, 2 * sizeof(int), SEEK_CUR);
  fread(pd->id, sizeof(float), N, fd);

  fseek(fd, 2 * sizeof(int), SEEK_CUR);
  fread(pd->mass, sizeof(float), N, fd);

  fseek(fd, 2 * sizeof(int), SEEK_CUR);
  fread(pd->s, sizeof(float), N, fd);

  fseek(fd, 2 * sizeof(int), SEEK_CUR);
  fread(pd->rho, sizeof(float), N, fd);

  fseek(fd, 2 * sizeof(int), SEEK_CUR);
  fread(pd->hsml, sizeof(float), N, fd);

  fseek(fd, 2 * sizeof(int), SEEK_CUR);
  fread(pd->potential, sizeof(float), N, fd);

  pd->total_number = N;
  pd->total_mass = 0;

  for (size_t i = 0; i < N; ++i) {
    pd->total_mass += pd->mass[i];
  }

EXIT:
  fclose(fd);
  return status;
}
