#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"


void save_output( struct particle_data *p ) {

  FILE * fp;
  int i;

  if( !(fp = fopen(fout, "w")) ) {
    printf( "Unable to open file %s for writing.\n", fout );
    exit(0);
  }

  fprintf( fp, "#Id\tBound\n" );

  for( i = 1; i <= p->Ntot; i++ ) {
    fprintf( fp, "%d\t%d\n", p->id[i], p->bnd[i] );
  }

  fclose(fp);

}
