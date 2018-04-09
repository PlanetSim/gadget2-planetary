#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "eos.h"
#include "globalvars.h"


struct eos_table {
  int ND, NS, NN;
  double *density, *entropy, *thermo_quantities[EOS_NUM_THERMO];
};

static struct eos_table *eos_table_import(char *filename);
static void locate(double *xx, long n, double x, long *j);
static void hunt(double *xx, long n, double x, long *jlo);
  
double eos_table_interp(double density, double entropy, int particle_id, short eos_thermo_flag, short eos_extrap_flag)
{
  static struct eos_table *materials[EOS_NUM_MATERIALS];
  static short eos_init_flag = 0;
  static long d0 = -1, s0 = -1;
  double q11, q12, q21, q22, q31, q32;
  double lq11, lq12, lq21, lq22, lq31, lq32;
  double dd21, ds21, dd31, ds31;
  double ldd21, lds21, ldd31, lds31;
  int i;
  short eos_material_flag;
  double x, qqq, lqqq, qqqo;
  char filename[100];


  /* Allocate space for tables and import equation-of-state data. */
  if (!eos_init_flag) {
    eos_init_flag = 1;
    for (i=0;i<EOS_NUM_MATERIALS;i++) {
     sprintf(filename,"%s%02d%s",EOS_MATERIAL_ROOT,i+1,EOS_MATERIAL_SUFFIX);

      if (((materials[i] = (struct eos_table*) malloc(sizeof(struct eos_table))) == NULL)){ 
	fprintf(stderr, "ERROR in eos_table_interp: Could not import equation-of-state table: %s\n", filename);
	exit(7);
      }
      if((materials[i] = eos_table_import(filename)) == NULL){
	fprintf(stderr, "ERROR in eos_table_interp: Could not import equation-of-state table: %s\n", filename);
        exit(7);
      }
    }
  }


  /* Compute material type from particle id. */
  modf(((double)(particle_id-EOS_ID_START))/EOS_ID_SKIP,&x);
  eos_material_flag = (long)(x+0.5);
  if ((eos_material_flag<0)||(eos_material_flag>=EOS_NUM_MATERIALS+EOS_NUM_GASES)) {
    fprintf(stderr, "ERROR in eos-table_interp: Invalid particle id.\n");
   exit(7); 
  }

  /* Check if eos_thermo_flag is in range. */
  if ((eos_thermo_flag<0)||(eos_thermo_flag>=EOS_NUM_THERMO)) {
    fprintf(stderr, "ERROR in eos_table_interp: Invalid eos_thermo_flag.\n");
    exit(7); 
  }
  
  if( eos_material_flag >= EOS_NUM_MATERIALS ) {
    /* Is a gas, use analytical equations instead of tables */
    if ( eos_thermo_flag == EOS_PRESSURE ) {
      qqq = entropy * pow(density, GAMMA);
    }
    else if ( eos_thermo_flag == EOS_TEMPERATURE ) {
      qqq = pow(density, GAMMA_MINUS1) / GAMMA_MINUS1; /* ??? */
    }
    else if ( eos_thermo_flag == EOS_INT_ENERGY ) {
      qqq = entropy / (GAMMA_MINUS1) * pow(density, GAMMA_MINUS1);
    }
    else if ( eos_thermo_flag == EOS_SOUND_SPEED ) {
      qqq = sqrt(GAMMA * (entropy * pow(density, GAMMA)) / density);
    }
  }
  else {
    if(density < materials[eos_material_flag]->density[0])
      {
	density = materials[eos_material_flag]->density[0];
      }
    if(density > materials[eos_material_flag]->density[materials[eos_material_flag]->ND-1])
      {
	density = materials[eos_material_flag]->density[materials[eos_material_flag]->ND-1];
      }
    if(entropy < materials[eos_material_flag]->entropy[0])
      {
	entropy = materials[eos_material_flag]->entropy[0];
      }
    if(entropy > materials[eos_material_flag]->entropy[materials[eos_material_flag]->NS-1])
      {
	entropy = materials[eos_material_flag]->entropy[materials[eos_material_flag]->NS-1];
      }


    /* Search tables. */
    hunt(materials[eos_material_flag]->density,materials[eos_material_flag]->ND,density,&d0);
    hunt(materials[eos_material_flag]->entropy,materials[eos_material_flag]->NS,entropy,&s0);

    /* Check for extrapolation. */
    if ((d0<0)||(d0>materials[eos_material_flag]->ND-2)||(s0<0)||(s0>materials[eos_material_flag]->NS-2)) {
      if (!eos_extrap_flag) {
	fprintf(stderr,"ERROR in eos_table_interp: No extrapolation allowed!\n");
	exit(7);
      }
      else { /* Allow extrapolation. */
	if (d0<0)
	  d0=0;
	else if (d0>materials[eos_material_flag]->ND-2)
	  d0=materials[eos_material_flag]->ND-2;
	if (s0<0)
	  s0=0;
	else if (s0>materials[eos_material_flag]->NS-2)
	  s0=materials[eos_material_flag]->NS-2;
      }
    }

    /* Do linear interpolation. */
    q11  = *(materials[eos_material_flag]->thermo_quantities[eos_thermo_flag]+(d0+0)+(s0+0)*(materials[eos_material_flag]->ND));
    q12  = *(materials[eos_material_flag]->thermo_quantities[eos_thermo_flag]+(d0+1)+(s0+0)*(materials[eos_material_flag]->ND));
    q21  = *(materials[eos_material_flag]->thermo_quantities[eos_thermo_flag]+(d0+0)+(s0+1)*(materials[eos_material_flag]->ND));
    q22  = *(materials[eos_material_flag]->thermo_quantities[eos_thermo_flag]+(d0+1)+(s0+1)*(materials[eos_material_flag]->ND));

    dd21 = materials[eos_material_flag]->density[d0+1]-materials[eos_material_flag]->density[d0];
    ds21 = materials[eos_material_flag]->entropy[s0+1]-materials[eos_material_flag]->entropy[s0];
    dd31 = density - materials[eos_material_flag]->density[d0];
    ds31 = entropy - materials[eos_material_flag]->entropy[s0];
    q31  = q11 + dd31*(q12-q11)/dd21;
    q32  = q21 + dd31*(q22-q21)/dd21;
    qqqo  = q31 + ds31*(q32-q31)/ds21;

    /* Do log interpolation */
    lq11  = log10(q11);
    lq12  = log10(q12);
    lq21  = log10(q21);
    lq22  = log10(q22);
    ldd21 = log10(materials[eos_material_flag]->density[d0+1])-log10(materials[eos_material_flag]->density[d0]);
    lds21 = log10(materials[eos_material_flag]->entropy[s0+1])-log10(materials[eos_material_flag]->entropy[s0]);
    ldd31 = log10(density) - log10(materials[eos_material_flag]->density[d0]);
    lds31 = log10(entropy) - log10(materials[eos_material_flag]->entropy[s0]);

    lq31  = lq11 + ldd31*(lq12-lq11)/ldd21;
    lq32  = lq21 + ldd31*(lq22-lq21)/ldd21;
    lqqq  = lq31 + lds31*(lq32-lq31)/lds21;
    qqq  = pow(10.0,lqqq);

    /* Use linear for higher densities */
    if(density>2.0) { 
      qqq=qqqo;
    }

  }
  return qqq;
}


struct eos_table *eos_table_import(char *filename)
     /* Import equation-of-state table.  File format is a serial list
	containing: ND = number of density elements, NS = number of entropy
	elements, a list of the ND density elements, a list of the NS entropy
	elements, a list of the ND*NS elements of pressure (row-wise; that
	is, varying density first), followed by the ND*NS elements of
	temperature, internal energy, and the sound speed.  Thus, the file
	should have 2+ND+NS+4*ND*NS elements. */
{
  struct eos_table *p;
  FILE *fp;
  int i, j;



  if ((p = (struct eos_table *) malloc(sizeof(struct eos_table)))==NULL) {
    fprintf(stderr, "ERROR in eos_table_import: Could not allocate space for equation-of-state table: %s\n",filename);
    return NULL;
  }



  if ((fp = fopen(filename,"r"))==NULL) {
    fprintf(stderr, "ERROR in eos_table_import: Could not open equation-of-state data file: %s\n",filename);
    return NULL;
  }



  fscanf(fp,"%d",&p->ND);
  fscanf(fp,"%d",&p->NS);
  p->NN = (p->ND)*(p->NS);
  if (((p->density=(double *)malloc((p->ND)*sizeof(double)))==NULL)||((p->entropy=(double *)malloc((p->NS)*sizeof(double)))==NULL)) {
    fprintf(stderr,"ERROR in eos_table_import: Could not allocate space for equation-of-state table: %s\n",filename);
    return NULL;
  }
  
  for (i=0;i<EOS_NUM_THERMO;i++) {
    if ((p->thermo_quantities[i]=(double *)malloc((p->NN)*sizeof(double)))==NULL) {
      fprintf(stderr, "ERROR in eos_table_import: Could not allocate space for equation-of-state table: %s\n",filename);
      return NULL;
    }
  }



  for (i=0;i<p->ND;i++) fscanf(fp,"%lf",p->density+i);
  for (i=0;i<p->NS;i++) fscanf(fp,"%lf",p->entropy+i);
  for (j=0;j<EOS_NUM_THERMO;j++) for (i=0;i<(p->NN);i++) fscanf(fp,"%lf",p->thermo_quantities[j]+i);



  fclose(fp);
  return p;
}


void locate(double *xx, long n, double x, long *j)
     /* Given an array xx[0,...,n-1] and given a value x, returns a      */
     /* value j such that x is between xx[j] and xx[j+1].                */
     /* xx must be monotonic, either increasing or decreasing.           */
     /* j=-1 or j=n-1 is returned to indicate that x is out of range.    */
     /* Numerical Recipes in C, p. 117, modified for zero-offset arrays. */
{
  long ju, jm, jl;
  int ascnd;

  xx--;                 /* Needed for zero-offset arrays. */
  jl=0;                 /* Initialize lower  */
  ju=n+1;               /* and upper limits. */
  ascnd=(xx[n]>=xx[1]);
  while (ju-jl>1) {     /* If we are not yet done, */
    jm=(ju+jl)>>1;      /* compute a midpoint,     */
    if (x >= xx[jm] == ascnd)
      jl=jm;            /* and replace either the lower limit */
    else
      ju=jm;            /* or the upper limit, as appropriate.*/
  }                     /* Repeat until the test condition is satisfied. */
  if (x==xx[1]) *j=1;   /* Then set the output */
  else if (x==xx[n]) *j=n-1;
  else *j=jl;
  (*j)--; /* Needed for zero-offset arrays. */
  xx++;   /* Needed for zero-offset arrays. */
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

