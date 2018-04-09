#include <stdio.h>
#include <math.h>
#include <stdlib.h>


#include "globalvars.h"
#include "prototypes.h"



void WriteICs(PARTICLE *p, int *numpart, float mass_of_one_particle)
     // Writes the initial conditions file for Gadget.  For more information on the format
     // see the Gadget users guide.
     // All SPH particles are set to type 0
{

      struct io_header
	// 256 byte header, the char at the end is unused
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
        } header1;


      FILE *fd;
      int i, id;
      float xyz[3];
      int blklen;
      int mat;


        #define BLKLEN fwrite(&blklen, sizeof(blklen), 1, fd)

      if(!(fd = fopen(fout, "w")))
	{
	  printf("error opening file %s\n", fout);
	  exit(0);
	}


      printf("Saving initial conditions to file '%s'\n\n", fout);

      header1.npart[0] = header1.npartTotal[0] = nSPH;
      header1.npart[1] = header1.npartTotal[1] = 0;
      header1.npart[2] = header1.npartTotal[2] = 0;
      header1.npart[3] = header1.npartTotal[3] = 0;
      header1.npart[4] = header1.npartTotal[4] = 0;
      header1.npart[5] = header1.npartTotal[5] = 0;


      for(i=0; i<6; i++)
	{
	  header1.mass[i] = 0.0;
	  header1.nallhw[i] = 0;
	}

      header1.time = 0.0;
      header1.redshift = 0.0;
      header1.flag_sfr = 0;
      header1.flag_feedback = 0;
      header1.flag_cooling = 0;
      header1.num_files = 0;
      header1.BoxSize = 0.0;
      header1.Omega0 = 0.0;
      header1.OmegaLambda = 0.0;
      header1.HubbleParam = 0.0;
      header1.flag_age = 0;
      header1.flag_metals = 0;
      header1.flag_entr_ics = 1;

      // write the header
      blklen = sizeof(header1);
      BLKLEN;
      fwrite(&header1, sizeof(header1), 1, fd);
      BLKLEN;



      // write the particle positions
      blklen = 3 * (nSPH) * sizeof(float);
      BLKLEN;

      for(i=0; i < nSPH; i++)
        {
	  xyz[0] = p[i].pos[0];
	  xyz[1] = p[i].pos[1];
	  xyz[2] = p[i].pos[2];
	  fwrite(xyz, sizeof(float), 3, fd);
        }

      BLKLEN;



      // write the particle velocities
      blklen = 3 * (nSPH) * sizeof(float);
      BLKLEN;

      for(i=0; i < nSPH; i++)
      {
	  xyz[0] = p[i].vel[0];
	  xyz[1] = p[i].vel[1];
	  xyz[2] = p[i].vel[2];
	  fwrite(xyz, sizeof(float), 3, fd);
      }

      BLKLEN;



      // write the particle IDs
      blklen = (nSPH) * sizeof(int);
      BLKLEN;

      for(mat = 0; mat < numMaterials; mat++)
        {
	  for(i=1; i <= numpart[mat]; i++)
	    {
	      id = i + mat*ID_SKIP;
	      fwrite(&id, sizeof(int), 1, fd);
	    }
        }

      BLKLEN;



      // write the particle masses
      blklen = (nSPH) * sizeof(float);
      BLKLEN;

      // JMAA 8/2011 - previously were set based on number of particles of each material. Has been corrected so all materials have same mass particles
      for(i=0; i < nSPH; i++)
	{
	  fwrite(&mass_of_one_particle, sizeof(float), 1, fd);
	}

      BLKLEN;


      // write the entropy
      blklen = (nSPH) * sizeof(float);
      BLKLEN;

      for(i=0; i < nSPH; i++)
        {
	  xyz[0] = p[i].s;
	  fwrite(xyz, sizeof(float), 1, fd);
        }

      BLKLEN;


      fclose(fd);

      return;

}

