#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"


void save_snapshot(struct particle_data *p1, struct io_header *header)
{

  FILE *fd;
  int i,j, blklen, num;
  struct io_header new_header;



  recenter(p1);


  count=0;

  for(j=1;j<=p1->Ntot;j++)
    if(p1->bnd[j]==1)
      count++;


#define BLKLEN fwrite(&blklen, sizeof(blklen), 1, fd);

  strcat(fout,"_ejecta");

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
      if(p1->bnd[i] != 1)
	fwrite(&p1->pos[i][1], sizeof(float), 3, fd);
    }
  BLKLEN;

  blklen = 3 * (p1->Ntot) * sizeof(float);
  BLKLEN;
  for(i=1; i <= num; i++)
    {
      if(p1->bnd[i] != 1)
	fwrite(&p1->vel[i][1], sizeof(float), 3, fd);
    }
  BLKLEN;
  
  blklen = (p1->Ntot) * sizeof(int);
  BLKLEN;
  for(i=1; i <= num; i++)
    {
      if(p1->bnd[i] != 1)
	fwrite(&p1->id[i], sizeof(int), 1, fd);
    }
  BLKLEN;
  
  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  for(i=1; i <= num; i++)
    {
      if(p1->bnd[i] != 1)
	fwrite(&p1->m[i], sizeof(float), 1, fd);
    }
  BLKLEN;
  
  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  for(i=1; i <= num; i++)
    {
      if(p1->bnd[i] != 1)
	fwrite(&p1->s[i], sizeof(float), 1, fd);
    }
  BLKLEN;

  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  for(i=1; i <= num; i++)
    {
      if(p1->bnd[i] != 02)
	fwrite(&p1->rho[i], sizeof(float), 1, fd);
    }
  BLKLEN;

  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  for(i=1; i <= num; i++)
    {
      if(p1->bnd[i] != 1)
	fwrite(&p1->hsml[i], sizeof(float), 1, fd);
    }
  BLKLEN;

  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  for(i=1; i <= num; i++)
    {
      if(p1->bnd[i] != 1)
	fwrite(&p1->pot[i], sizeof(float), 1, fd);
    }
  BLKLEN;
  
  fclose(fd);

}



void  recenter(struct  particle_data *p1){

  double xm,ym,zm,vxm,vym,vzm,mlr,xc,yc,zc,vxc,vyc,vzc;
  int i;

  for(i=1;i<=p1->Ntot;i++){
    if(p1->bnd[i]==1){
      xm += p1->pos[i][1]*p1->m[i];
      ym += p1->pos[i][2]*p1->m[i];
      zm += p1->pos[i][3]*p1->m[i];
      vxm += p1->vel[i][1]*p1->m[i];
      vym += p1->vel[i][2]*p1->m[i];
      vzm += p1->vel[i][3]*p1->m[i];
      mlr += p1->m[i];
    }
  }

  xc=xm/mlr;
  yc=ym/mlr;
  zc=zm/mlr;
  vxc=vxm/mlr;
  vyc=vym/mlr;
  vzc=vzm/mlr;

  for(i=1;i<=p1->Ntot;i++){
    p1->pos[i][1]=p1->pos[i][1]-xc;
    p1->pos[i][2]=p1->pos[i][2]-yc;
    p1->pos[i][3]=p1->pos[i][3]-zc;
    p1->vel[i][1]=p1->vel[i][1]-vxc;
    p1->vel[i][2]=p1->vel[i][2]-vyc;
    p1->vel[i][3]=p1->vel[i][3]-vzc;
  }

}








