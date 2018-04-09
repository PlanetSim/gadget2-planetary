
#define pfile  "parameters.txt"  // parameter file


extern char snapbase[25];
extern char fout[25];
extern int  opt;

extern struct planet_data {
  float **pos, **vel, *m, *s, *rho, *hsml, *pres, *T, *u;
  double Mtot;
  int *id;
  int Ntot;
} planet[1];



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
  int flag_entropy_instead_u;
  char fill[256 - 6*4 - 6*8 - 2*8 - 2*4 - 6*4 - 2*4 - 4*8 - 2*4 - 6*4 - 1*4];
} header[1];



void readparam(void);
void load_particles(int, struct planet_data *, struct io_header *);
void pressure(struct planet_data *);
void save_pressure(struct planet_data *, struct io_header *);
