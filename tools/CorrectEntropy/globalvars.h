// files
extern char snapname[200];
extern char fout[200];
extern char sprofilename[200];


extern struct planet_data {
  float **pos, **vel, *m, *s;
  double Mtot;
  int *id;
  int Ntot;
} planet;



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
} header;



void readparam( char * );
void load_particles( struct planet_data *, struct io_header * );
void save_snapshot( struct planet_data *, struct io_header * );
int get_s_num( void );
void load_s_profile( double *, double *, int );
void change_entropies( struct planet_data * );
double interpolate_entropy( double, double *, double *, int );
