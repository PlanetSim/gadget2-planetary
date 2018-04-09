#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"


void save_combined(struct planet_data *p1, struct planet_data *p2)
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

  printf("Writing data to file '%s'\n", fout);
  printf("Mass of body 1: %g\n", p1->Mtot);
  printf("Mass of body 2: %g\n", p2->Mtot);
  printf("Relative velocity: %g, impact parameter: %g\n\n", relV, b);


  new_header = header[0];

  new_header.npart[0] += header[1].npart[0];
  new_header.npartTotal[0] += header[1].npartTotal[0];
  new_header.time = 0.0;
  new_header.flag_entr_ics = 1;
  /*new_header.flag_doubleprecision = 0;*/

  for(i = 1; i <= p2->Ntot; i++)
    p2->id[i] += p1->Ntot;


  if(Rcrust1)
	identify_crust(p1,Rcrust1);
  if(Rcrust2)
	identify_crust(p2,Rcrust2);

  identify_body(p1, p2);	/* shift IDs */

  printf("Final ID: %d\n", p2->id[p2->Ntot]);

  blklen = sizeof(struct io_header);
  BLKLEN;
  fwrite(&new_header, sizeof(struct io_header), 1, fd);
  BLKLEN;


  blklen = 3 * (p1->Ntot + p2->Ntot) * sizeof(float);
  BLKLEN;
  fwrite(&p1->pos[1][1], sizeof(float), 3 * p1->Ntot, fd);
  fwrite(&p2->pos[1][1], sizeof(float), 3 * p2->Ntot, fd);
  BLKLEN;

  blklen = 3 * (p1->Ntot + p2->Ntot) * sizeof(float);
  BLKLEN;
  fwrite(&p1->vel[1][1], sizeof(float), 3 * p1->Ntot, fd);
  fwrite(&p2->vel[1][1], sizeof(float), 3 * p2->Ntot, fd);
  BLKLEN;

  blklen = (p1->Ntot + p2->Ntot) * sizeof(int);
  BLKLEN;
  fwrite(&p1->id[1], sizeof(int), p1->Ntot, fd);
  fwrite(&p2->id[1], sizeof(int), p2->Ntot, fd);
  BLKLEN;

  blklen = (p1->Ntot + p2->Ntot) * sizeof(float);
  BLKLEN;
  fwrite(&p1->m[1], sizeof(float), p1->Ntot, fd);
  fwrite(&p2->m[1], sizeof(float), p2->Ntot, fd);
  BLKLEN;

  blklen = (p1->Ntot + p2->Ntot) * sizeof(float);
  BLKLEN;
  fwrite(&p1->s[1], sizeof(float), p1->Ntot, fd);
  fwrite(&p2->s[1], sizeof(float), p2->Ntot, fd);
  BLKLEN;

  blklen = (p1->Ntot + p2->Ntot) * sizeof(float);
  BLKLEN;
  fwrite(&p1->rho[1], sizeof(float), p1->Ntot, fd);
  fwrite(&p2->rho[1], sizeof(float), p2->Ntot, fd);
  BLKLEN;

  blklen = (p1->Ntot + p2->Ntot) * sizeof(float);
  BLKLEN;
  fwrite(&p1->hsml[1], sizeof(float), p1->Ntot, fd);
  fwrite(&p2->hsml[1], sizeof(float), p2->Ntot, fd);
  BLKLEN;

  blklen = (p1->Ntot + p2->Ntot) * sizeof(float);
  BLKLEN;
  fwrite(&p1->pot[1], sizeof(float), p1->Ntot, fd);
  fwrite(&p2->pot[1], sizeof(float), p2->Ntot, fd);
  BLKLEN;

  fclose(fd);

}
