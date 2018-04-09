#ifndef SSIO_HINCLUDED
#define SSIO_HINCLUDED

/*
 ** ssio.h -- DCR 98-09-16
 ** ======
 ** Header file specific to Solar System data I/O routines.
 **
 ** DCR 6/13/08: added "reduced" ss format support.
 **
 ** 10/30/13: merged with Bristol version supporting MPI files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>	/* for MAXPATHLEN */
#include <rpc/rpc.h>	/* for XDR routines */

#ifndef MAXPATHLEN
# define MAXPATHLEN 256
#endif

#ifndef N_DIM
# define N_DIM 3
#endif

#define SSIO_MAGIC_STANDARD (-1) /* for backwards compatability */
#define SSIO_MAGIC_REDUCED  (-10101) /* arbitrary */

/*
 ** Following structures intended for use with xdr I/O routines.  Note
 ** that internal representations may differ from format written to
 ** disk.  As a result, for now use hardwired sizes for seeking.  Is
 ** there an xdr routine to figure this out automatically?...
 */

#ifdef SSIO_USE_MPI
typedef int32_t ptcl_idx_t;
#else
typedef int ptcl_idx_t; /* for compatability with xdr routines */
#endif

typedef struct ss_head {
  double time;
  ptcl_idx_t n_data;
  ptcl_idx_t iMagicNumber; /* for distinguishing between formats */
} SSHEAD;

#define SSHEAD_SIZE (sizeof(double) + 2*sizeof(ptcl_idx_t))

typedef struct ss_data {
	double mass;
	double radius;
	double pos[N_DIM];
	double vel[N_DIM];
	double spin[N_DIM];
	ptcl_idx_t color;
	ptcl_idx_t org_idx;
	} SSDATA;

#define SSDATA_SIZE (2*sizeof(double) + 3*N_DIM*sizeof(double) + 2*sizeof(ptcl_idx_t))

typedef struct ssr_data {
  float fMass;
  float fRadius;
  float vPos[N_DIM];
  ptcl_idx_t iColor;
  ptcl_idx_t iOrgIdx;
  } SSRDATA;

#define SSRDATA_SIZE (2*sizeof(float) + N_DIM*sizeof(float) + 2*sizeof(ptcl_idx_t))

#ifdef SSIO_USE_MPI

#include "mpi.h"

// MAX_OPEN only used by genchuck.c -- clean this up?
#define MAX_OPEN INT_MAX

// particles to buffer at a time - say 100000 per process as a maximum
#define MAX_BUF_PARTICLES 100000

typedef struct ssio {
	FILE *fp;
	XDR xdrs;
    MPI_File mfile;
    int fmode;

    size_t max_buf_particles;

    size_t total_to_buffer;
    size_t total_read;
    size_t extra;

    size_t particles_written;
    size_t particles_read;

    char * file_buf;
    char * cur_buf_ptr;
} SSIO;

void ssioInitialise (void);
void ssioSetGroupFile (void);
void ssioSetSingleFile(void);
int ssioSetPos(SSIO *ssio, const MPI_Offset pos);

#else

typedef struct ssio {
	FILE *fp;
	XDR xdrs;
} SSIO;

int ssioSetPos(SSIO *ssio, const size_t pos);
#endif

#define SSIO_READ	0
#define SSIO_WRITE	1
#define SSIO_UPDATE	2

#define SS_EXT ".ss"

int ssioNewExt(const char *infile, const char *inext,
			   char *outfile, const char *outext);
int ssioOpen(const char *filename, SSIO *ssio, u_int mode);
int ssioHead(SSIO *ssio, SSHEAD *head);
int ssioData(SSIO *ssio, SSDATA *data);
int ssioDataReduced(SSIO *ssio,SSRDATA *data);
int ssioClose(SSIO *ssio);
void ssioRewind(SSIO *ssio);
void ssioStandardToReduced(const SSDATA *ds,SSRDATA *dr);
void ssioReducedToStandard(const SSRDATA *dr,SSDATA *ds);

#endif
