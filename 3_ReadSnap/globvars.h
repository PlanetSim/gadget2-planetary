

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

  float  Rho, U, Temp, Ne;
  float  Nh, Mfs, Mclouds, hsml, sfr, meanz, TpU;
  float  Metallicity;
  float  totrad, totshock, totfb;
  int    ParentID;
  float  OrigMass, OrigHsml;
  float  Potential;

  float P, T, IntE, cs;	/* PJC 14/12/15: Pressure, Temperature and actual Internal energy (will come from EOS) */

} *P;

extern int *Id;

extern double  Time, Redshift;
extern int     SnapInfo;
extern int     SnapFile;



int load_snapshot(char *fname, int files, int quiteon);
int allocate_memory(void);
int reordering(void);
void pressure();



