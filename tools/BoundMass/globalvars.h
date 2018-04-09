#define G       6.673e-8

//Parameters to be read in
extern char infile[200];
extern char fout[200];
extern double tol;
extern int maxit;
extern int nMat;
extern int idskip;


extern struct particle_data {
  float **pos, **vel, *m, *s, *rho, *hsml, *pot;
  double Mtot;
  int *id;
  int *bnd;
  int Ntot;
} P;

extern struct io_header
{
  int npart[6];
  double mass[6];
  double time;
  double redshift;
  int flag_sfr;
  int flag_feedback;
  int npartTotal[6];
  int flag_cooling;
  int num_files;
  double BoxSize;
  double Omega0;
  double OmegaLambda;
  double HubbleParam;
  int flag_age;
  int flag_metals;
  int nallhw[6];
  int flag_entr_ics;
  char fill[256 - 6*4 - 6*8 - 2*8 - 2*4 - 6*4 - 2*4 - 4*8 - 2*4 - 6*4 - 1*4];
} H;

void readparam( char * );
void load_particles( struct particle_data *, struct io_header * );
void bound( struct particle_data * );
void save_output( struct particle_data * );

