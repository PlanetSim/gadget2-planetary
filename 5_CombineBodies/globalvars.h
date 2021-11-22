
extern char body1[200];
extern char body2[200];
extern char fout[200];
extern double R1;
extern double R2;
extern double relV;
extern double b;
extern double sep;
extern double Rcrust1;
extern double Rcrust2;
extern int ID_SKIP;

extern struct planet_data {
  float **pos, **vel, *m, *s, *rho, *hsml, *pot;
  double Mtot;
  int *id;
  int Ntot;
} planet[2];



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
  int flag_doubleprecision;
  char fill[256 - 6*4 - 6*8 - 2*8 - 2*4 - 6*4 - 2*4 - 4*8 - 2*4 - 6*4 - 2*4];
} header[2];



void readparam( char * );
void load_particles(char *, struct planet_data *, struct io_header *);
void move_body(struct planet_data *, struct planet_data *);
void save_combined(struct planet_data *, struct planet_data *);
void identify_body(struct planet_data *, struct planet_data *);
void identify_crust(struct planet_data *, double);

