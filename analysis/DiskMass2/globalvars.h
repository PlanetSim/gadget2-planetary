
#define pfile  "dmparam.txt"  // parameter file
#define PI      3.14159265
#define G       6.673e-8
#define K       0.335       // gyration constant
#define _Rho    5.5         // mean earth density
#define tol     1.0e-4
#define maxit   20
#define Mearth  5.9742e27
#define Rearth  6.3781e8
#define Lem     3.53e41      // Earth-Moon system L in cgs
#define Mlun    7.3477e25


extern char snapbase[25];
extern char fout[25];
extern double Xcm;
extern double Ycm;
extern double Zcm;
extern double Vxcm;
extern double Vycm;
extern double Vzcm;
extern double Xe;
extern double Ye;
extern double Ze;

extern double gamma1;
extern double b;

extern int n1;

extern double f;
extern double Mpe;
extern double Mpe_fe;
extern double Mpe_si;
extern double Lpe;
extern double Md;
extern double Ld;
extern double Mfe;
extern double Mimp;
extern double Mesc;
extern double Lesc;


extern struct planet_data {
  float **pos, **vel, *m, *s, *rho, *hsml, *pres, *T, *pot;
  double Mtot;
  int *id, *bnd;
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



void readparam(void);
void load_particles(struct planet_data *, struct io_header *);
void findCM(struct planet_data *);
void changeCoords(struct planet_data *);
void findDiskMass(struct planet_data *);
void save_snapshot(struct planet_data *, struct io_header *);
void bound(struct planet_data *);
