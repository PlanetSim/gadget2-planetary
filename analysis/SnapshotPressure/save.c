#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"


void save_pressure(struct planet_data *p1, struct io_header *header)
{

  FILE *fd;
  int i, blklen;
  struct io_header new_header;


#define BLKLEN fwrite(&blklen, sizeof(blklen), 1, fd);

  if(!(fd = fopen(fout, "w")))
    {
      fprintf(stderr, "Can't write to file '%s'\n", fout);
      exit(13);
    }

  if(opt == 3) header->flag_entropy_instead_u = 0;

  blklen = sizeof(struct io_header);
  BLKLEN;
  fwrite(&header[0], sizeof(struct io_header), 1, fd);
  BLKLEN;


  blklen = 3 * (p1->Ntot) * sizeof(float);
  BLKLEN;
  fwrite(&p1->pos[1][1], sizeof(float), 3 * p1->Ntot, fd);
  BLKLEN;

  blklen = 3 * (p1->Ntot) * sizeof(float);
  BLKLEN;
  fwrite(&p1->vel[1][1], sizeof(float), 3 * p1->Ntot, fd);
  BLKLEN;
  
  blklen = (p1->Ntot) * sizeof(int);
  BLKLEN;
  fwrite(&p1->id[1], sizeof(int), p1->Ntot, fd);
  BLKLEN;
  
  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  fwrite(&p1->m[1], sizeof(float), p1->Ntot, fd);
  BLKLEN;
  
  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  if(opt == 1)
    {
      fwrite(&p1->pres[1], sizeof(float), p1->Ntot, fd);
    }else if(opt == 2)
    {
      fwrite(&p1->T[1], sizeof(float), p1->Ntot, fd);
    }else if(opt == 3)
    {
      fwrite(&p1->u[1], sizeof(float), p1->Ntot, fd);
    } else
    {
      fwrite(&p1->s[1], sizeof(float), p1->Ntot, fd);
    }
  BLKLEN;

  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  fwrite(&p1->rho[1], sizeof(float), p1->Ntot, fd);
  BLKLEN;

  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  fwrite(&p1->hsml[1], sizeof(float), p1->Ntot, fd);
  BLKLEN;
  
  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  fwrite(&p1->pres[1], sizeof(float), p1->Ntot, fd);
  BLKLEN;

  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  fwrite(&p1->T[1], sizeof(float), p1->Ntot, fd);
  BLKLEN;
  
  fclose(fd);

}
