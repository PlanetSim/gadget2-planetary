
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "globalvars.h"



// particle structure definition
typedef struct {
  double rad;              // particle radius from center
  double pos[NDIM];        // particle position (x,y,z)
  double vel[NDIM];        // particle velocity (vx, vy, vz)
  int ID;                  // particle ID
  double s;                // particle entropy per unit mass
  double u;                // particle internal energy per unit mass
}PARTICLE;



void Readtable(double *, double *, double *, double *, double *, double *, double *);
void InitPlanet(double *, double *);
void Tprofile(double *, double *);
double Interpolate(double, double, double *, double *, double *, int, int, int);
void CalcEnt(double *, double *, double, double, double *, double *, double *, double *, int, int, int);
void MakeParticles(double *, double *, double *, double *);
void Findxyz(PARTICLE *);
void WriteICs(PARTICLE *, int *, float);
void write_s_profile( int, double *, double * );
void write_t_profile( int, double *, double * );
void print_total_u( PARTICLE *, double );
void readparam(void);






