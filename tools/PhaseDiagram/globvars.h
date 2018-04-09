//number of files per snapshot (not tested for >1)
#define _NOFILES 1
//number of isobars to be allocated (0 means all of them)
#define _NOISOBARS 120
//maximum pressure of isobars
#define _MAXPRESSURE 5.1e11


void save_table( char *, double *, double *, double *, int, int );
void load_eos_table( char *, int *, int *, double **, double **, double **, double **, double **, double ** );
