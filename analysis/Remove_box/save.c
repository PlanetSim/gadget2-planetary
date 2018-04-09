#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"


void save_snapshot(struct planet_data *p1, struct io_header *header)
{

  FILE *fd;
  int i, blklen, num;
  struct io_header new_header;


#define BLKLEN fwrite(&blklen, sizeof(blklen), 1, fd);

  if(!(fd = fopen(fout, "w")))
    {
      fprintf(stderr, "Can't write to file '%s'\n", fout);
      exit(13);
    }

  printf("Writing file '%s'\n\n", fout);


  // change number of particles in header
  new_header = header[0];
  new_header.npart[0] = p1->Ntot - count;
  new_header.npartTotal[0] = new_header.npart[0];
 
  num = p1->Ntot;
  p1->Ntot -= count;


  //write the new header
  blklen = sizeof(struct io_header);
  BLKLEN;
  fwrite(&new_header, sizeof(struct io_header), 1, fd);
  BLKLEN;

  blklen = 3 * (p1->Ntot) * sizeof(float);
  BLKLEN;
  for(i=1; i <= num; i++)
    {
      if(p1->id[i] != 0)
	fwrite(&p1->pos[i][1], sizeof(float), 3, fd);
    }
  BLKLEN;

  blklen = 3 * (p1->Ntot) * sizeof(float);
  BLKLEN;
  for(i=1; i <= num; i++)
    {
      if(p1->id[i] != 0)
	fwrite(&p1->vel[i][1], sizeof(float), 3, fd);
    }
  BLKLEN;
  
  blklen = (p1->Ntot) * sizeof(int);
  BLKLEN;
  for(i=1; i <= num; i++)
    {
      if(p1->id[i] != 0)
	fwrite(&p1->id[i], sizeof(int), 1, fd);
    }
  BLKLEN;
  
  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  for(i=1; i <= num; i++)
    {
      if(p1->id[i] != 0)
	fwrite(&p1->m[i], sizeof(float), 1, fd);
    }
  BLKLEN;
  
  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  for(i=1; i <= num; i++)
    {
      if(p1->id[i] != 0)
	fwrite(&p1->s[i], sizeof(float), 1, fd);
    }
  BLKLEN;

  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  for(i=1; i <= num; i++)
    {
      if(p1->id[i] != 0)
	fwrite(&p1->rho[i], sizeof(float), 1, fd);
    }
  BLKLEN;

  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  for(i=1; i <= num; i++)
    {
      if(p1->id[i] != 0)
	fwrite(&p1->hsml[i], sizeof(float), 1, fd);
    }
  BLKLEN;

  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  for(i=1; i <= num; i++)
    {
      if(p1->id[i] != 0)
	fwrite(&p1->pot[i], sizeof(float), 1, fd);
    }
  BLKLEN;
  
  fclose(fd);

}
