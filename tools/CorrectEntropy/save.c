 #include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"


void save_snapshot( struct planet_data *p1, struct io_header *header ) {

  FILE *fd;
  int blklen;

#define BLKLEN fwrite(&blklen, sizeof(blklen), 1, fd);

  if( !(fd = fopen(fout, "w")) ) {
    fprintf(stderr, "Can't write to file '%s'\n", fout);
    exit(13);
  }

  printf("Writing file '%s'\n\n", fout);

  header->time = 0.0;
  header->flag_entr_ics = 1;
  header->flag_doubleprecision = 0;

  //Output header
  printf( "Header:\n" );
  printf( "Npart: 0: %d; 1: %d; 2: %d; 3: %d; 4: %d; 5: %d\n",
	  header->npart[0], header->npart[1], header->npart[2], header->npart[3], header->npart[4], header->npart[5] );
  printf( "NpartTotal: 0: %d; 1: %d; 2: %d; 3: %d; 4: %d; 5: %d\n",
	  header->npartTotal[0], header->npartTotal[1], header->npartTotal[2], header->npartTotal[3], header->npartTotal[4], header->npartTotal[5] );
  printf( "NAllHW: 0: %d; 1: %d; 2: %d; 3: %d; 4: %d; 5: %d\n",
	  header->nallhw[0], header->nallhw[1], header->nallhw[2], header->nallhw[3], header->nallhw[4], header->nallhw[5] );

  
  //write the header
  blklen = sizeof(struct io_header);
  BLKLEN;
  fwrite( header, sizeof(struct io_header), 1, fd );
  BLKLEN;

  blklen = 3 * (p1->Ntot) * sizeof(float);
  BLKLEN;
  fwrite( &p1->pos[1][1], sizeof(float), 3 * p1->Ntot, fd );
  BLKLEN;

  blklen = 3 * (p1->Ntot) * sizeof(float);
  BLKLEN;
  fwrite( &p1->vel[1][1], sizeof(float), 3 * p1->Ntot, fd );
  BLKLEN;
  
  blklen = (p1->Ntot) * sizeof(int);
  BLKLEN;
  fwrite( &p1->id[1], sizeof(int), p1->Ntot, fd );
  BLKLEN;
  
  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  fwrite( &p1->m[1], sizeof(float), p1->Ntot, fd );
  BLKLEN;
  
  blklen = (p1->Ntot) * sizeof(float);
  BLKLEN;
  fwrite( &p1->s[1], sizeof(float), 1, fd );
  BLKLEN;

  fclose(fd);

}
