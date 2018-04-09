#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "nrsrc/nrutil.h"

#include "globalvars.h"


void load_particles( struct planet_data *p, struct io_header *header ) {

  FILE *fd;
  int i, blksize;
  static int first=1;

#define SKIP fread(&blksize, sizeof(int), 1, fd);

  if(!(fd = fopen(snapname, "r"))) {    
    fprintf(stderr, "Could not find file '%s'\n", snapname);
    exit(13);
  }


  SKIP;
  fread(header, sizeof(struct io_header), 1, fd);
  SKIP;


  // allocate memory
  if( first ) {
    first=0;
    p->Ntot = header->npart[0];
    
    p->pos = matrix(1, p->Ntot, 1, 3);
    p->vel = matrix(1, p->Ntot, 1, 3);
    p->id  = ivector(1, p->Ntot);
    p->m   = vector(1, p->Ntot);
    p->s   = vector(1, p->Ntot);
  }

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

  fclose(fd);

  for(i = 1, p->Mtot = 0; i <= p->Ntot; i++)
    p->Mtot += p->m[i];

}
