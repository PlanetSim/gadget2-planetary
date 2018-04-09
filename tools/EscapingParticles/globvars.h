#define _G (6.67e-8)

//1 if there's acceleration/potential/etc. in the snapshots, 0 otherwise
//N.B. that U takes the dSdt field in the snapshots, and S takes the U field
#define _HASACC 1
#define _HASPOT 1
#define _HASSTEP 1
#define _HASU 1
//number of files per snapshot (not tested for >1)
#define _NOFILES 1


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
extern int     npart, Ngas;
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
extern double Time, Redshift;


int load_snapshot( char *fname, int files );
void save_output( char *, int * );
void indexx( unsigned long, double *, unsigned long * );
