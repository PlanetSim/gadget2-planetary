#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "globvars.h"



  

/*
 * Uses pre-loaded EOS data to get the value of one of the depenent variables given an S & a Rho
 * eos_thermo_flag sets which dependent variable you want to output - see globvars.h for flags
 */
double eos_table_interp( double density, double entropy, struct eos_table *eostable, short eos_thermo_flag ) {

  static short eos_init_flag = 0;
  static long d0 = -1, s0 = -1;
  double q11, q12, q21, q22, q31, q32;
  double dd21, ds21, dd31, ds31;
  int i;
  double x, qqq;



  /* Check if eos_thermo_flag valid (i.e. in range) */
  if( (eos_thermo_flag < 0) || (eos_thermo_flag >= EOS_NUM_THERMO) ) {
    fprintf( stderr, "ERROR in eos_table_interp: Invalid eos_thermo_flag.\n" );
    exit(7); 
  }
  
  //reset given density & entropy if out of range
  if( density < eostable->density[0] ) {
    density = eostable->density[0];
  }
  if( density > eostable->density[eostable->ND-1] ) {
    density = eostable->density[eostable->ND-1];
  }
  if( entropy < eostable->entropy[0] ) {
    entropy = eostable->entropy[0];
  }
  if( entropy > eostable->entropy[eostable->NS-1] ) {
    entropy = eostable->entropy[eostable->NS-1];
  }


  /* Search tables for density & entropy bound indecies  */
  hunt( eostable->density, eostable->ND, density, &d0 );
  hunt( eostable->entropy, eostable->NS, entropy, &s0 );

  /* Check for extrapolation. */
  if( (d0 < 0) || (d0 > eostable->ND-2) || (s0 < 0) || (s0 > eostable->NS-2) ) {
    if( d0 < 0 )
      d0 = 0;
    else if( d0 > eostable->ND-2 )
      d0 = eostable->ND-2;
    if( s0 < 0 )
      s0 = 0;
    else if( s0 > eostable->NS-2 )
      s0 = eostable->NS-2;
  }


  /* Do linear interpolation to find the desired quantity */
  q11  = *(eostable->thermo_quantities[eos_thermo_flag]+(d0+0)+(s0+0)*(eostable->ND));
  q12  = *(eostable->thermo_quantities[eos_thermo_flag]+(d0+1)+(s0+0)*(eostable->ND));
  q21  = *(eostable->thermo_quantities[eos_thermo_flag]+(d0+0)+(s0+1)*(eostable->ND));
  q22  = *(eostable->thermo_quantities[eos_thermo_flag]+(d0+1)+(s0+1)*(eostable->ND));
  dd21 = eostable->density[d0+1]-eostable->density[d0];
  ds21 = eostable->entropy[s0+1]-eostable->entropy[s0];
  dd31 = density - eostable->density[d0];
  ds31 = entropy - eostable->entropy[s0];
  q31  = q11 + dd31*(q12-q11)/dd21;
  q32  = q21 + dd31*(q22-q21)/dd21;
  qqq  = q31 + ds31*(q32-q31)/ds21;


  return qqq;

}



/*
 * Import an EOS table
 */
struct eos_table *eos_table_import( char *filename )
     /* Import equation-of-state table.  File format is a serial list
	containing: ND = number of density elements, NS = number of entropy
	elements, a list of the ND density elements, a list of the NS entropy
	elements, a list of the ND*NS elements of pressure (varing density for 
	each entropy), followed by the ND*NS elements of temperature, 
	internal energy, and the sound speed.  Thus, the file should have 
	2+ND+NS+4*ND*NS elements. */
{

  struct eos_table *p;
  FILE *fp;
  int i, j;

  // allocate the memory
  if( (p = (struct eos_table *) malloc(sizeof(struct eos_table))) == NULL ) {
    fprintf( stderr, "ERROR in eos_table_import: Could not allocate space for equation-of-state table: %s\n", filename );
    return NULL;
  }

  // open the file
  if( (fp = fopen(filename,"r")) == NULL ) {
    fprintf( stderr, "ERROR in eos_table_import: Could not open equation-of-state data file: %s\n", filename );
    return NULL;
  }

  // Read in number of density & entropy points
  fscanf(fp,"%d",&p->ND);
  fscanf(fp,"%d",&p->NS);
  p->NN = (p->ND)*(p->NS);

  // allocate memory
  if( ((p->density=(double *)malloc((p->ND)*sizeof(double))) == NULL) || ((p->entropy=(double *)malloc((p->NS)*sizeof(double))) == NULL ) ) {
    fprintf(stderr,"ERROR in eos_table_import: Could not allocate space for equation-of-state table: %s\n",filename);
    return NULL;
  }
  
  //loop through the dependent variables, allocating memory for each
  for( i=0; i < EOS_NUM_THERMO; i++ ) {
    if( (p->thermo_quantities[i]=(double *)malloc((p->NN)*sizeof(double))) == NULL ) {
      fprintf(stderr, "ERROR in eos_table_import: Could not allocate space for equation-of-state table: %s\n",filename);
      return NULL;
    }
  }

  
  // Read in all of the data
  for( i=0; i < p->ND; i++ )
    fscanf(fp,"%lf",p->density+i);
  for( i=0; i < p->NS; i++ )
    fscanf(fp,"%lf",p->entropy+i);
  for( j=0; j < EOS_NUM_THERMO; j++ ) {
    for( i=0; i < (p->NN); i++ )
      fscanf(fp,"%lf",p->thermo_quantities[j]+i);
  }

  fclose(fp);
  return p;

}




void hunt(double *xx, long n, double x, long *jlo)
     /* Given an array xx[0,...,n-1] and given a value x, returns a      */
     /* value jlo such that x is between xx[jlo] and xx[jlo+1].          */
     /* xx must be monotonic, either increasing or decreasing.           */
     /* jlo=-1 or jlo=n-1 is returned to indicate that x is out of range.*/
     /* jlo on input is taken as the initial guess for jlo on output.    */
     /* Numerical Recipes in C, p. 118, modified for zero-offset arrays. */
{
  long jm, jhi, inc;
  int ascnd;

  xx--;                   /* Needed for zero-offset arrays. */
  (*jlo)++;               /* Needed for zero-offset arrays. */
  ascnd=(xx[n]>=xx[1]);   /* True if ascending order, false otherwise. */
  if (*jlo<=0||*jlo>n){   /* Input guess not useful. Go to bisection.  */
    *jlo=0;
    jhi=n+1;
  }
  else {
    inc=1;                             /* Set the hunting increment. */
    if (x >= xx[*jlo] == ascnd) {      /* Hunt up:                   */
      if (*jlo==n) return;
      jhi=(*jlo)+1;
      while (x >= xx[jhi] == ascnd) {  /* Not done hunting,          */
	*jlo=jhi;
	inc += inc;                    /* so double the increment.   */
	jhi=(*jlo)+inc;
	if (jhi>n) {                   /* Done hunting, off end of table. */
	  jhi=n+1;
	  break;
	}                              /* Try again. */
      }                                /* Done hunting, value bracketed. */
    }
    else {                             /* Hunt down:                 */
      if (*jlo==1) {
	*jlo=0;
	return;
      }
      jhi=(*jlo)--;
      while (x < xx[*jlo] == ascnd) {  /* Not done hunting,          */
	jhi=(*jlo);
	inc <<= 1;                     /* so double the increment.   */
	if (inc>=jhi) {                /* Done hunting, off end of table. */
	  *jlo=0;
	  break;
	}
	else *jlo=jhi-inc;
      }                                /* Try again. */
    }                                  /* Done hunting, value bracketed. */
  }                                    /* Hunt is done, so begin bisection. */
  while (jhi-(*jlo)!=1) {
    jm=(jhi+(*jlo))>>1;
    if (x >= xx[jm] == ascnd)
      *jlo=jm;
    else
      jhi=jm;
  }
  if (x==xx[n]) *jlo=n-1;
  if (x==xx[1]) *jlo=1;
  (*jlo)--; /* Needed for zero=offset arrays. */
  xx++;     /* Needed for zero-offset arrays. */
}

