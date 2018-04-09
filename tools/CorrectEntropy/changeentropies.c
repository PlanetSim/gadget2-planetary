#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "globalvars.h"


void change_entropies( struct planet_data *p ) {

  int i;
  double r;
  double * radarr;
  double * entarr;
  int num;

  printf( "Loading entropy profile: %s\n", sprofilename );
  num = get_s_num();
  radarr = (double *) malloc( num * sizeof(double) );
  entarr = (double *) malloc( num * sizeof(double) );
  load_s_profile( radarr, entarr, num );

  printf( "Changing the entropy.\n" );
  for( i = 1; i <= p->Ntot; i++ ) {
      
    r = sqrt( p->pos[i][1]*p->pos[i][1] + 
	      p->pos[i][2]*p->pos[i][2] + 
	      p->pos[i][3]*p->pos[i][3] );
    p->s[i] = interpolate_entropy( r, radarr, entarr, num );
    
  }

  return;
    
}


int get_s_num() {

  int num;
  FILE *fp;
  if( !(fp = fopen(sprofilename, "r")) )  {
    printf( "Could not find the profile table %s\n", sprofilename );
    exit(13);
  }
  fscanf( fp, "%d", &num );
  fclose(fp);
  return num;

}


void load_s_profile( double * rad, double * ent, int num ) {

  FILE *fp1;
  int i;

  if( !(fp1 = fopen(sprofilename, "r")) )  {
    printf( "Could not find the profile table %s\n", sprofilename );
    exit(13);
  }
  // dispose of length line
  fscanf(fp1, "%d", &i);

  for( i = 0; i < num; i++ ) {
    fscanf( fp1, "%lf%lf", &(rad[i]), &(ent[i]) );
  }

  fclose(fp1);

}



double interpolate_entropy( double r, double * radarr, double * entarr, int num ) {

  int i = 0;
  double S = 0;
  double deltaS, deltaR;

  // find the index of the smallest R in rvalues that is larger than r
  while( (i < num) && (r > radarr[i]) ) {
    i++;
  }

  if( i == 0 ) {

    // r is smaller than the smallest rvalue, extrapolate
    S = entarr[i] - (entarr[i+1] - entarr[i])/(radarr[i+1] - radarr[i]) * r;
 
  } else if( r > radarr[i] ) {

    // r is larger than the largest rvalue, extrapolate
    S = entarr[i] + (entarr[i] - entarr[i-1])/(radarr[i] - radarr[i-1]) * r;

  } else {

    // there are two points to interpolate between
    deltaS = entarr[i] - entarr[i-1];
    deltaR = radarr[i] - radarr[i-1];
    S = entarr[i] + (deltaS/deltaR) * (r - radarr[i]);

  }

  return S;

}
