/*
 ** ssio.c -- DCR 98-09-16
 ** ======
 ** Solar System data I/O routines.
 **
 ** DCR 6/13/08: added "reduced" ss format support.
 **
 ** 10/30/13: merged with Bristol version supporting MPI files.
 **
 ** PJC 27/07/15: If editing, ensure src/ss/ssio.c and src/pkdgrav/ssio.c are the same.
 */

#include <string.h>
#include <assert.h>
#include "ssio.h"

#ifdef SSIO_USE_MPI
MPI_Comm process_comm;
static int ssio_initd = 0;

typedef int (*mpi_file_func_t)(MPI_File, void *, int, MPI_Datatype, MPI_Status *);
mpi_file_func_t WRITE_FUNC = MPI_File_write_shared;
mpi_file_func_t READ_FUNC = MPI_File_read_shared;
#endif /* SSIO_USE_MPI */

static const char *ssioBasename(const char *path)
{
	char *p;

	assert(path != NULL);
	p = strrchr(path,'/');
	if (p) return p + 1;
	else return path;
}

int ssioNewExt(const char *infile,const char *inext,
			   char *outfile,const char *outext)
{
	const char *basename;
	char *c;
	size_t n;

	assert(infile != NULL && inext != NULL && outfile != NULL && outext != NULL);
	basename = ssioBasename(infile);
	if ((c = strrchr(basename,'.')) && strstr(c,inext))
		n = c - basename;
	else
		n = strlen(basename);
	if (n + strlen(outext) >= (size_t) MAXPATHLEN)
		return 1;
	(void) strncpy(outfile,basename,n); /* not null terminated */
	(void) strcpy(outfile + n,outext);
	return 0;
}

#ifdef SSIO_USE_MPI

/*
 *  Because sometimes only one process at a time wants to read or write to
 *  the file and other times many do, just open a single file in each process.
 *  This is kind of against the whole point of MPI files, but any other way to
 *  do it would require a load of hacky stuff.
 */
void ssioInitialise(void)
{
    if (ssio_initd)
    {
        return;
    }

    int ret;
    // start MPI if it isn't started already (for writing ssic.ss)
    MPI_Initialized(&ret);
    if (!ret)
    {
        MPI_Init(NULL, NULL);
    }

    // all processes originally in one group
    MPI_Group original_group;

    // extract original group from comm world
    MPI_Comm_group(MPI_COMM_WORLD, &original_group);

    int num = 1;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int* group_ranks = (int *) malloc(sizeof(int));
    group_ranks[0] = rank;

    MPI_Group loc_group;

    // all processes with a certain hash will now join the same group
    MPI_Group_incl(original_group,
                   num,
                   &group_ranks[0],
                   &loc_group);
    // create the comm which will be used within the group
    MPI_Comm_create(MPI_COMM_WORLD,
                    loc_group,
                    &process_comm);

    MPI_Group_free(&loc_group);
    free(group_ranks);

    // still need
    //MPI_Comm_free(&process_comm);

    ssio_initd = 1;
}

/*
 *  Set all processes to access the file at the same time.
 *  Has to be called before it's opened or weirdness happens
 */
void ssioSetGroupFile(void)
{
    /*
    MPI_Info info;
    MPI_Info_create(&info);
    MPI_File_get_info(ssio->mfile, &info);

    static char key[MPI_MAX_INFO_KEY];
    static char value[MPI_MAX_INFO_VAL + 1];

    int nkeys, flag;
    MPI_Info_get_nkeys(info, &nkeys);

    int ii;
    fprintf(stdout, "%d keys\n", nkeys);
    for (ii = 0; ii < nkeys; ii++)
    {
        MPI_Info_get_nthkey(info, ii, key);

        MPI_Info_get(info, key, MPI_MAX_INFO_KEY, value, &flag);

        fprintf(stdout, "key = '%s'; value = '%s'\n", key, value);
    }
    */

    // just for initialising mpi
    if (!ssio_initd)
    {
        ssioInitialise();
    }

    // set it to be MPI_COMM_WORLD again
    MPI_Comm_free(&process_comm);
    MPI_Comm_dup(MPI_COMM_WORLD, &process_comm);
}

/*
 *  Opposite of above
 */
void ssioSetSingleFile (void)
{
    MPI_Comm_free(&process_comm);
    ssio_initd = 0;
    ssioInitialise();
}

#endif /* SSIO_USE_MPI */

int ssioOpen(const char *filename, SSIO *ssio, const u_int mode)
{
	assert(filename != NULL && ssio != NULL);
	assert(mode == SSIO_READ || mode == SSIO_WRITE || mode == SSIO_UPDATE);

#ifdef SSIO_USE_MPI

    // need this check for ss2bt, ssa, etc
    if (!ssio_initd)
    {
        ssioInitialise();
    }

    int open_mode;
    if (SSIO_READ == mode)
    {
        open_mode = MPI_MODE_RDONLY;
    }
    else if (SSIO_WRITE == mode)
    {
        open_mode = MPI_MODE_WRONLY | MPI_MODE_CREATE;
    }
    else //if (SSIO_UPDATE == mode)
    {
        open_mode = MPI_MODE_RDWR;
    }

    // save for ssioData later use
    ssio->fmode = mode;

    int err;
    err = MPI_File_open(process_comm,
                        (char *)filename,
                        open_mode,
                        MPI_INFO_NULL,
                        &ssio->mfile);

    // if anything went wrong
    if (err)
    {
        return 1;
    }

    if (SSIO_READ == mode)
    {
        size_t particle_div = 1;

        // get file size and correct for the header being there
        MPI_Offset filesize;
        err |= MPI_File_get_size(ssio->mfile,
                                 &filesize);

        int n_ranks, rank;
        err |= MPI_Comm_rank(process_comm,
                             &rank);
        err |= MPI_Comm_size(process_comm,
                             &n_ranks);

        // how many particles each process will use (nLocal in pkd.c?)
        const size_t total_particles = ((filesize - SSHEAD_SIZE)/SSDATA_SIZE);
        const size_t local_particles = total_particles/n_ranks;

        ssio->total_to_buffer = local_particles;
        ssio->total_read = 0;
        ssio->extra = total_particles % local_particles;

        // check if there are less particles per process than max buffer size
        if (MAX_BUF_PARTICLES > local_particles)
        {
            // just load the max amount in each process
            ssio->max_buf_particles = local_particles;
        }
        // check if it can be nicely divided up
        else
        {
            // divide until its less than buffer size
            while (local_particles/particle_div > MAX_BUF_PARTICLES
            // or its dividing it can't divide it any more
            && particle_div < local_particles/2
            // or it's divided it equally
            && local_particles % particle_div)
            {
                particle_div++;
            }

            ssio->max_buf_particles = local_particles/particle_div;
        }

        // didn't divide evenly
        if (local_particles % particle_div)
        {
            if (!rank)
            {
                fprintf(stderr, "%d processes\n", n_ranks);
                fprintf(stderr, "%zu total particles\n", total_particles);
                //fprintf(stderr, "%lld local particles\n", local_particles);
                fprintf(stderr, "%f per process\n", total_particles/(float)n_ranks);

                // FIXME do something here instead of erroring out
                fprintf(stderr, "Couldn't divide particles nicely between processes\n");
                fprintf(stderr, "Change max buf particles\n");
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            else
            {
                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
        //fprintf(stderr, "%lld particles per process\n", local_particles);
        //fprintf(stderr, "%lld particles in one read\n", ssio->max_buf_particles);

        if (ssio->extra && !rank)
        {
            fprintf(stdout, "Prepared to read an extra %lu in a process\n", ssio->extra);
        }
    }
    else
    {
        ssio->max_buf_particles = MAX_BUF_PARTICLES;
    }

    ssio->particles_written = 0;
    ssio->particles_read = ssio->max_buf_particles;
    ssio->file_buf = (char *) malloc(ssio->max_buf_particles*SSDATA_SIZE);
	assert(ssio->file_buf != NULL);
    ssio->cur_buf_ptr = ssio->file_buf;

#else

    // original xdr
	const char type[][3] = {"r","w","r+"};
	const enum xdr_op op[] = {XDR_DECODE,XDR_ENCODE,XDR_ENCODE};
	if (!(ssio->fp = fopen(filename,type[mode])))
		return 1;
	xdrstdio_create(&ssio->xdrs,ssio->fp,op[mode]);

#endif /* SSIO_USE_MPI */

	return 0;
}

int ssioHead(SSIO *ssio,SSHEAD *head)
{
	assert(ssio != NULL && head != NULL);

#ifdef SSIO_USE_MPI

    if (SSIO_READ == ssio->fmode)
    {
        READ_FUNC(ssio->mfile, &head->time, sizeof(SSHEAD), MPI_CHAR, MPI_STATUS_IGNORE);
    }
    else //if (SSIO_WRITE == ssio->fmode)
    {
        WRITE_FUNC(ssio->mfile, &head->time, sizeof(SSHEAD), MPI_CHAR, MPI_STATUS_IGNORE);
    }

#else

	if (!xdr_double(&ssio->xdrs,&head->time)) return 1;
	if (!xdr_int(&ssio->xdrs,&head->n_data)) return 1;
	if (!xdr_int(&ssio->xdrs,&head->iMagicNumber)) return 1;
	if (head->iMagicNumber != SSIO_MAGIC_STANDARD &&
		head->iMagicNumber != SSIO_MAGIC_REDUCED) {
		(void) fprintf(stderr,"Invalid SSIO magic number (%i).\n",head->iMagicNumber);
		return 1;
		}

#endif /* SSIO_USE_MPI */

	return 0;
}

int ssioData(SSIO *ssio, SSDATA *data)
{
	assert(ssio != NULL && data != NULL);

#ifdef SSIO_USE_MPI

    /*
     *  Buffer a certain amount before reading or writing it to the file.
     */

    if (SSIO_READ == ssio->fmode)
    {
        /*
         *  FIXME
         *  Need to read a number of particles that is exactly divisible by
         *  the number allocated to each task or else each task will read a
         *  chunk including some which should be read by other tasks, but is
         *  then discarded
         */
        if (ssio->max_buf_particles == ssio->particles_read)
        {
            // reset
            ssio->particles_read = 0;
            ssio->cur_buf_ptr = ssio->file_buf;

            // if the max equal divison has been read, only buffer 1 at a time
            if (ssio->total_read == ssio->total_to_buffer)
            {
                ssio->max_buf_particles = 1;
            }
            /*
            if (ssio->max_buf_particles == 1)
            {
                fprintf(stdout, "Reading 1 extra...\n");
            }
            */
            ssio->total_read += ssio->max_buf_particles;

            READ_FUNC(ssio->mfile,
                      ssio->file_buf,
                      ssio->max_buf_particles*SSDATA_SIZE,
                      MPI_BYTE,
                      MPI_STATUS_IGNORE);
        }

        memcpy(&data->mass,
               ssio->cur_buf_ptr,
               2*sizeof(double));
        memcpy(data->pos,
               ssio->cur_buf_ptr + 2*sizeof(double),
               N_DIM*sizeof(double));
        memcpy(data->vel,
               ssio->cur_buf_ptr + 2*sizeof(double) + N_DIM*sizeof(double),
               N_DIM*sizeof(double));
        memcpy(data->spin,
               ssio->cur_buf_ptr + 2*sizeof(double) + 2*N_DIM*sizeof(double),
               N_DIM*sizeof(double));
        memcpy(&data->color,
               ssio->cur_buf_ptr + 2*sizeof(double) + 3*N_DIM*sizeof(double),
               2*sizeof(ptcl_idx_t));

        ssio->particles_read += 1;
        ssio->cur_buf_ptr += SSDATA_SIZE;
    }
    else //if (SSIO_WRITE == ssio->fmode)
    {
        memcpy(ssio->cur_buf_ptr,
               &data->mass,
               2*sizeof(double));
        memcpy(ssio->cur_buf_ptr + 2*sizeof(double),
               data->pos,
               N_DIM*sizeof(double));
        memcpy(ssio->cur_buf_ptr + 2*sizeof(double) + N_DIM*sizeof(double),
               data->vel,
               N_DIM*sizeof(double));
        memcpy(ssio->cur_buf_ptr + 2*sizeof(double) + 2*N_DIM*sizeof(double),
               data->spin,
               N_DIM*sizeof(double));
        memcpy(ssio->cur_buf_ptr + 2*sizeof(double) + 3*N_DIM*sizeof(double),
               &data->color,
               2*sizeof(ptcl_idx_t));
        
        ssio->particles_written += 1;
        ssio->cur_buf_ptr += SSDATA_SIZE;

        if (ssio->max_buf_particles == ssio->particles_written)
        {
            // reset
            ssio->particles_written = 0;
            ssio->cur_buf_ptr = ssio->file_buf;

            // write block
            WRITE_FUNC(ssio->mfile,
                       ssio->file_buf,
                       ssio->max_buf_particles*SSDATA_SIZE,
                       MPI_BYTE,
                       MPI_STATUS_IGNORE);
        }
    }

#else

	int i;
    
	if (!xdr_double(&ssio->xdrs,&data->mass)) return 1;
	if (!xdr_double(&ssio->xdrs,&data->radius)) return 1;
	for (i=0;i<N_DIM;i++)
		if (!xdr_double(&ssio->xdrs,&data->pos[i])) return 1;
	for (i=0;i<N_DIM;i++)
		if (!xdr_double(&ssio->xdrs,&data->vel[i])) return 1;
	for (i=0;i<N_DIM;i++)
		if (!xdr_double(&ssio->xdrs,&data->spin[i])) return 1;
	if (!xdr_int(&ssio->xdrs,&data->color)) return 1;
	if (!xdr_int(&ssio->xdrs,&data->org_idx)) return 1;

#endif /* SSIO_USE_MPI */

	return 0;
}

int ssioClose(SSIO *ssio)
{
	assert(ssio != NULL);

#ifdef SSIO_USE_MPI

    if ((SSIO_WRITE == ssio->fmode || SSIO_UPDATE == ssio->fmode)
		&& ssio->particles_written)
    {
        // write any that didn't get in to the buffer
        WRITE_FUNC(ssio->mfile,
                   ssio->file_buf,
                   ssio->particles_written*SSDATA_SIZE,
                   MPI_BYTE,
                   MPI_STATUS_IGNORE);
    }

    MPI_File_close(&ssio->mfile);
    free(ssio->file_buf);

#else

	xdr_destroy(&ssio->xdrs);
	if (fclose(ssio->fp)) return 1;

#endif /* SSIO_USE_MPI */

	return 0;
	}

#ifdef SSIO_USE_MPI
int ssioSetPos(SSIO *ssio, const MPI_Offset pos)
{
	assert(ssio != NULL);
    // don't seek past header
    assert(pos == SSHEAD_SIZE);
    MPI_File_seek_shared(ssio->mfile, pos, MPI_SEEK_SET);
    return 0;
}
#else
int ssioSetPos(SSIO *ssio, const size_t pos)
{
	assert (ssio != NULL);
    assert (pos < UINT_MAX);
	if (!xdr_setpos(&ssio->xdrs,pos)) return 1;
    return 0;
}
#endif /* SSIO_USE_MPI */

void ssioRewind(SSIO *ssio)
{
	assert(ssio != NULL);

#ifdef SSIO_USE_MPI

    MPI_File_seek_shared(ssio->mfile, 0, MPI_SEEK_SET);

#else

	rewind(ssio->fp);

#endif /* SSIO_USE_MPI */

}

int ssioDataReduced(SSIO *ssio,SSRDATA *data)
{
#ifdef SSIO_USE_MPI
	assert(0); /*DEBUG! reduced format not supported with MPI files yet */
#endif

	int i;

	assert(ssio != NULL && data != NULL);
	if (!xdr_float(&ssio->xdrs,&data->fMass)) return 1;
	if (!xdr_float(&ssio->xdrs,&data->fRadius)) return 1;
	for (i=0;i<N_DIM;i++)
		if (!xdr_float(&ssio->xdrs,&data->vPos[i])) return 1;
	if (!xdr_int(&ssio->xdrs,&data->iColor)) return 1;
	if (!xdr_int(&ssio->xdrs,&data->iOrgIdx)) return 1;
	return 0;
	}

void ssioStandardToReduced(const SSDATA *ds,SSRDATA *dr)
{
	int i;

	assert(ds != NULL && dr != NULL);
	dr->fMass = ds->mass;
	dr->fRadius = ds->radius;
	for (i=0;i<N_DIM;i++)
		dr->vPos[i] = ds->pos[i];
	dr->iColor = ds->color;
	dr->iOrgIdx = ds->org_idx;
	}

void ssioReducedToStandard(const SSRDATA *dr,SSDATA *ds)
{
	int i;

	assert(dr != NULL && ds != NULL);
	ds->mass = dr->fMass;
	ds->radius = dr->fRadius;
	for (i=0;i<N_DIM;i++) {
		ds->pos[i] = dr->vPos[i];
		ds->vel[i] = ds->spin[i] = 0.0;
		}
	ds->color = dr->iColor;
	ds->org_idx = dr->iOrgIdx;
	}

/* ssio.c */
