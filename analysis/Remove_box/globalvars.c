#include "globalvars.h"


char snapbase[25];    // base name for the snapshot files
char fout[25];        // output file

double xmin;
double xmax;
double ymin;
double ymax;
double zmin;
double zmax;

int count;

double f;    // oblateness
double Mpe;  // mass of protoearth
double Lpe;  // angular momentum of protoearth
double Md;   // mass of the disk
double Ld;   // angular momentum of the disk
double Mfe;  // mass of iron in the disk
double Mimp; // mass from impactor in disk
double Mesc; // mass escaping
double Lesc; // angular momentum of the escaping particles


struct planet_data planet[1];
struct io_header header[1];

