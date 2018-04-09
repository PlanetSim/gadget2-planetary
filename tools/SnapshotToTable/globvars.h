//1 if there's acceleration/potential/etc. in the snapshots, 0 otherwise
#define _HASACC 1
#define _HASPOT 1
#define _HASSTEP 1
#define _HASU 1
//number of files per snapshot (not tested for >1)
#define _NOFILES 1
//must match that defined when creating the initial conditions and in eos.h of Gadget
#define _IDSKIP 200000000
//Number of materials in the body
#define _NOMATERIALS 2
//Base name for the EOS tables e.g. for "material" it will look for material1.txt etc.
#define eostablebase "material"


extern struct io_header_1
{
  int      npart[6];
  double   mass[6];
  double   time;
  double   redshift;
  int      flag_sfr;
  int      flag_feedbacktp;
  int      npartTotal[6];
  int      flag_cooling;
  int      num_files;
  double   BoxSize;
  double   Omega0;
  double   OmegaLambda;
  double   HubbleParam; 
  int      flag_stellarage;
  int      flag_metals;
  int      nallhw[6];
  int      flag_entr_ics;
  char     fill[256- 6*4- 6*8- 2*8- 2*4- 6*4- 2*4 - 4*8 - 2*4 - 6*4 - 1*4];  /* fills to 256 Bytes */
} header1;



extern int     NumPart, Ngas;

extern struct particle_data 
{
  float  Pos[3];
  float  Vel[3];
  float  Mass;
  int    Type;
  int    Id;
  float  Rho, U, Ne, hsml;
  float  Potential;
  float  Acc[3];
  float  S;
  float  TimeStep;
} *P;


/* ---------- for eos_table_interp */
// Number of dependent variables
#define EOS_NUM_THERMO    4
// Flags denoting each dependent variable
#define EOS_PRESSURE      0
#define EOS_TEMPERATURE   1
#define EOS_INT_ENERGY    2
#define EOS_SOUND_SPEED   3
// The first id number
#define EOS_ID_START      1
struct eos_table {
  int ND, NS, NN;
  double *density, *entropy, *thermo_quantities[EOS_NUM_THERMO];
};


extern double Time, Redshift;

int load_snapshot( char *fname, int files );
int allocate_memory(void);
double eos_table_interp( double, double, struct eos_table*, short eos_thermo_flag );
struct eos_table *eos_table_import( char *filename );
void hunt(double *xx, long n, double x, long *jlo);
