#include "globalvars.h"


char snapbase[25];    // base name for the snapshot files
char fout[25];        // output file

double Xcm;           // center of mass coordinates
double Ycm;
double Zcm;
double Vxcm;
double Vycm;
double Vzcm;
double Xe;            // center of earth coordinates
double Ye;
double Ze;

double gamma1;   // mass ratio
double b;        // impact parameter

int n1;          // number of particles in body 1


double f;      // oblateness
double Mpe;    // mass of protoearth
double Mpe_fe; // mass of iron in protoearth
double Mpe_si; // mass of silicate in protoearth
double Lpe;    // angular momentum of protoearth
double Md;     // mass of the disk
double Ld;     // angular momentum of the disk
double Mfe;    // mass of iron in the disk
double Mimp;   // mass from impactor in disk
double Mesc;   // mass escaping
double Lesc;   // angular momentum of the escaping particles


struct planet_data planet[1];
struct io_header header[1];

