#include "globalvars.h"


char body1[200];       // filename for first body
char body2[200];       // filename for second body
char fout[200];        // output file
double R1;            // radius of the first body
double R2;            // radius of the second body
double relV;          // relative velocity (in the x direction)
double b;             // impact parameter 
double sep;           // initial seperation (in the x direction)
double Rcrust1;		// radius of crust for first body
double Rcrust2;		// radius of crust for second body
int ID_SKIP;		// material id offset

struct planet_data planet[2];
struct io_header header[2];

