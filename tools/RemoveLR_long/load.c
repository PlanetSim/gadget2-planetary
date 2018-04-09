#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"
#include "nrsrc/nrutil.h"


void load_particles( struct particle_data *p, struct io_header *header ) {

  FILE *fd;
  int i, blksize;

#define SKIP fread(&blksize, sizeof(int), 1, fd);

  if( !(fd = fopen(infile, "r")) ) {    
    printf( "Could not find file '%s'\n", infile);
    exit(13);
  }

  SKIP;
  fread(header, sizeof(struct io_header), 1, fd);
  SKIP;

  p->Ntot = header->npart[0];
  p->pos  = matrix(1, p->Ntot, 1, 3);
  p->vel  = matrix(1, p->Ntot, 1, 3);
  p->accel  = matrix(1, p->Ntot, 1, 3);
  p->id   = ivector(1, p->Ntot);
  p->bnd  = ivector(1, p->Ntot);
  p->m    = vector(1, p->Ntot);
  p->s    = vector(1, p->Ntot);
  p->rho  = vector(1, p->Ntot);
  p->hsml = vector(1, p->Ntot);
  p->pot  = vector(1, p->Ntot);
  p->Pr = vector(1, p->Ntot);
  p->Te = vector(1, p->Ntot);
  p->U = vector(1, p->Ntot);
  p->cs = vector(1, p->Ntot);
  p->dt = vector(1, p->Ntot);
  
  SKIP;
  fread(&p->pos[1][1], sizeof(float), 3 * p->Ntot, fd);
  SKIP;

  SKIP;
  fread(&p->vel[1][1], sizeof(float), 3 * p->Ntot, fd);
  SKIP;

  SKIP;
  fread(&p->id[1], sizeof(int), p->Ntot, fd);
  SKIP;

  SKIP;
  fread(&p->m[1], sizeof(float), p->Ntot, fd);
  SKIP;

  SKIP;
  fread(&p->s[1], sizeof(float), p->Ntot, fd);
  SKIP;

  SKIP;
  fread(&p->rho[1], sizeof(float), p->Ntot, fd);
  SKIP;

  SKIP;
  fread(&p->hsml[1], sizeof(float), p->Ntot, fd);
  SKIP;

  SKIP;
  fread(&p->pot[1], sizeof(float), p->Ntot, fd);
  SKIP;

  SKIP;
  fread(&p->Pr[1], sizeof(float), p->Ntot, fd);
  SKIP;

  SKIP;
  fread(&p->Te[1], sizeof(float), p->Ntot, fd);
  SKIP;

  SKIP;
  fread(&p->U[1], sizeof(float), p->Ntot, fd);
  SKIP;

  SKIP;
  fread(&p->cs[1], sizeof(float), p->Ntot, fd);
  SKIP;

  SKIP;
  fread(&p->accel[1][1], sizeof(float), 3 * p->Ntot, fd);
  SKIP;

  SKIP;
  fread(&p->dt[1], sizeof(float), p->Ntot, fd);
  SKIP;

  fclose(fd);

  for(i = 1, p->Mtot = 0; i <= p->Ntot; i++)
    p->Mtot += p->m[i];

}
