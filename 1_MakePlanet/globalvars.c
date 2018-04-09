#include "globalvars.h"

char pfile[200];

// code related constants
double dconv;        // density convergence criterion (in g/cm^3)
int  maxitd;         // maximum number of iterations for density profile
int  nr;             // number of radii points


// planet related constants
double Psurf;         // surface pressure (in Pa)
double isoT;          // temperature for isothermal body
int nRho;             // number of density points in the largest table
int nT;               // number of temperature points in the largest table


int isotherm;         // flag for isothermal body
int numMaterials;     // number of materials
double *Rbound;       // array for holding radii of boundaries between materials
double *rhoguess;     // array for holding the initial rho guess for each material layer
char ttable[200];      // table for temperature profile
char tableName[200];   // base name for material EOS tables
char fout[200];        // output file
int nSPH;             // number of particles
int ID_SKIP;          // gap between particle IDs for different materials
