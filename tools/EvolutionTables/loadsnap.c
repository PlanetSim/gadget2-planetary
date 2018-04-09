#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "globvars.h"




/* this routine loads particle data from Gadget's default
 * binary file format. (A snapshot may be distributed
 * into multiple files.)
 */
int load_snapshot( char *fname, int files ) {

  FILE *fd;
  char   buf[200];
  int    i, j, k, dummy, ntot_withmasses;
  int    n, pc, pc_new, pc_sph;

#define SKIP fread(&dummy, sizeof(dummy), 1, fd);

  //Loop over files. pc seems to be some sort of particle id
  for(i=0, pc=1; i<files; i++, pc=pc_new) {

    if(files>1)
      sprintf(buf,"%s.%d",fname,i);
    else
      sprintf(buf,"%s",fname);

    //open file for reading
    if(!(fd=fopen(buf,"r"))) {
      printf("Can't open file `%s`\n",buf);
      exit(0);
    }

    fflush(stdout);


    SKIP;
    fread(&header1, sizeof(header1), 1, fd);
    SKIP;


    //get total number of particles
    if( files == 1 ) {

      for(k=0, npart=0, ntot_withmasses=0; k<6; k++)
	npart += header1.npart[k];
      Ngas= header1.npart[0];

    } else {

      for(k=0, npart=0, ntot_withmasses=0; k<6; k++)
	npart+= header1.npartTotal[k];
      Ngas= header1.npartTotal[0];

    }


    //get particles with mass
    for(k=0, ntot_withmasses=0; k<6; k++){
      if(header1.mass[k]==0)
	ntot_withmasses+= header1.npart[k];
    }

   
    //if this is the first file, allocate the memory
    if(i==0)
      allocate_memory();


    //Read position coordinates for each particle in this file
    SKIP;
    for( k=0, pc_new=pc; k < 6; k++ ) {
      for( n=0; n < header1.npart[k]; n++ ) {
	fread(&P[pc_new].Pos[0], sizeof(float), 3, fd);
	pc_new++;
      }
    }
    SKIP;

    //read velocity coordinates
    SKIP;
    for(k=0,pc_new=pc;k<6;k++) {
      for(n=0;n<header1.npart[k];n++) {
	fread(&P[pc_new].Vel[0], sizeof(float), 3, fd);
	pc_new++;
      }
    }
    SKIP;
    

    //Read Ids
    SKIP;
    for(k=0,pc_new=pc;k<6;k++) {
      for(n=0;n<header1.npart[k];n++) {
	fread(&P[pc_new].Id, sizeof(int), 1, fd);
	pc_new++;
      }
    }
    SKIP;


    if(ntot_withmasses>0)
      SKIP;


    //get types and masses
    for(k=0, pc_new=pc; k<6; k++) {
      for(n=0;n<header1.npart[k];n++) {
	
	P[pc_new].Type=k;
	if(header1.mass[k]==0)
	  fread(&P[pc_new].Mass, sizeof(float), 1, fd);
	else
	  P[pc_new].Mass= header1.mass[k];
	pc_new++;
      }
    }

    
    if(ntot_withmasses>0)
      SKIP;
      

    //Only read these values off if we have "gas" particles
    if( header1.npart[0] > 0 ) {

      /* --------- Entropy --------- */
      SKIP;
      for(n=0, pc_sph=pc; n<header1.npart[0];n++) {
	fread(&P[pc_sph].S, sizeof(float), 1, fd);
	pc_sph++;
      }
      SKIP;

      /* --------- Density ----------------- */
      SKIP;
      for(n=0, pc_sph=pc; n<header1.npart[0];n++) {
	fread(&P[pc_sph].Rho, sizeof(float), 1, fd);
	pc_sph++;
      }
      SKIP;

      /* --------- Smoothing length --------- */
      SKIP;
      for(n=0, pc_sph=pc; n<header1.npart[0];n++) {
	fread(&P[pc_sph].hsml, sizeof(float), 1, fd);
	pc_sph++;
      }
      SKIP;

      /* --------- Potential Energy ------ */
      if( _HASPOT ) {
	SKIP;
	for(k=0, pc_sph=pc; k<6;k++) {
	  for(n=0;n<header1.npart[k];n++) {
	    fread(&P[pc_sph].Potential, sizeof(float), 1, fd);
	    pc_sph++;
	  }
	}
	SKIP;
      } else {
	for(k=0, pc_sph=pc; k<6;k++) {
	  for(n=0;n<header1.npart[k];n++) {
	    P[pc_sph].Potential = 0.0;
	    pc_sph++;
	  }
	}
      }

      /* --------- Acceleration ------ */
      if( _HASACC ) {
	SKIP;
	for(k=0, pc_sph=pc; k<6;k++) {
	  for(n=0;n<header1.npart[k];n++) {
	    fread(&P[pc_sph].Acc[0], sizeof(float), 3, fd);
	    pc_sph++;
	  }
	}
	SKIP;
      } else {
	for(k=0, pc_sph=pc; k<6;k++) {
	  for(n=0;n<header1.npart[k];n++) {
	    P[pc_sph].Acc[0] = 0.0;
	    P[pc_sph].Acc[1] = 0.0;
	    P[pc_sph].Acc[2] = 0.0;
	    pc_sph++;
	  }
	}
      }

      /* --------- Internal Energy ------ */
      if( _HASU ) {
	SKIP;
	for(k=0, pc_sph=pc; k<6;k++) {
	  for(n=0;n<header1.npart[k];n++) {
	    fread(&P[pc_sph].U, sizeof(float), 1, fd);
	    pc_sph++;
	  }
	}
	SKIP;
      } else {
	for(k=0, pc_sph=pc; k<6;k++) {
	  for(n=0;n<header1.npart[k];n++) {
	    P[pc_sph].U = 0.0;
	    pc_sph++;
	  }
	}
      }      


      /* --------- Time Step ------ */
      if( _HASSTEP ) {
	SKIP;
	for(k=0, pc_sph=pc; k<6;k++) {
	  for(n=0;n<header1.npart[k];n++) {
	    fread(&P[pc_sph].TimeStep, sizeof(float), 1, fd);
	    pc_sph++;
	  }
	}
	SKIP;
      } else {
	for(k=0, pc_sph=pc; k<6;k++) {
	  for(n=0;n<header1.npart[k];n++) {
	    P[pc_sph].TimeStep = 0.0;
	    pc_sph++;
	  }
	}
      }      
      

    }
    // End just considering "gas" particles
       
    fclose(fd);
  }
  //close loop

  Time= header1.time;
  Redshift= header1.time;

}




/* this routine allocates the memory for the 
 * particle data.
 */
int allocate_memory(void) {

  if(!(P=malloc(npart*sizeof(struct particle_data)))) {
      fprintf(stderr,"failed to allocate memory.\n");
      fprintf(stderr,"npart = %d\n",npart);
      fprintf(stderr,"sizeof = %d\n",sizeof(struct particle_data));
      fprintf(stderr,"\n\n");
      exit(0);
  }
  
  P--;   /* start with offset 1 */

}






  











