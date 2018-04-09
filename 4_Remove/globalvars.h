
#define PI      3.14159265
#define G       6.673e-8
#define K       0.335       // gyration constant
#define _Rho    5.5         // mean earth density
#define tol     1.0e-5
#define maxit   20
#define Mearth  5.9742e27
#define Rearth  6.3781e8
#define Lem     3.53e41      // Earth-Moon system L in cgs
#define Mlun    7.3477e25


extern char snapbase[200];
extern char fout[200];

extern double rad;


extern int count;

extern double f;
extern double Mpe;
extern double Lpe;
extern double Md;
extern double Ld;
extern double Mfe;
extern double Mimp;
extern double Mesc;
extern double Lesc;


extern struct planet_data {
  float **pos, **vel, *m, *s, *rho, *hsml, *pot;
  double Mtot;
  int *id;
  int *bnd;
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
  int flag_entr_ics;
  char fill[256 - 6*4 - 6*8 - 2*8 - 2*4 - 6*4 - 2*4 - 4*8 - 2*4 - 6*4 - 1*4];
} header[1];



void readparam( char * );
void load_particles(struct planet_data *, struct io_header *);
void bound(struct planet_data *);
void save_snapshot(struct planet_data *, struct io_header *);
