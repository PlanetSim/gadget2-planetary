#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "globvars.h"




/* this routine loads particle data from Gadget's default
 * binary file format. (A snapshot may be distributed
 * into multiple files.
 */
int load_snapshot(char *fname, int files, int quiteon)
{
  FILE *fd;
  char   buf[200];
  int    i,j,k,dummy,ntot_withmasses;
  int    t,n,off,pc,pc_new,pc_sph;
  int    first_n, dummy_total=0;
  int    dog;

#define SKIP fread(&dummy, sizeof(dummy), 1, fd);

  for(i=0, pc=1; i<files; i++, pc=pc_new)
    {
      if(files>1)
	sprintf(buf,"%s.%d",fname,i);
      else
	sprintf(buf,"%s",fname);

      if(!(fd=fopen(buf,"r")))
	{
	  printf("can't open file `%s`\n",buf);
	  exit(0);
	}

      /*printf("reading `%s' ...\n",buf); fflush(stdout); */
      if(quiteon==0) printf("\n");
      fflush(stdout);

      SKIP;
      fread(&header1, sizeof(header1), 1, fd);
      SKIP;
      if(SnapInfo) printf("%10d\tHeader",dummy);  dummy_total+= dummy+8;

      if(files==1)
	{
	  for(k=0, NumPart=0, ntot_withmasses=0; k<6; k++)
	    NumPart+= header1.npart[k];
	  Ngas= header1.npart[0];
	}
      else
	{
	  for(k=0, NumPart=0, ntot_withmasses=0; k<6; k++)
	    NumPart+= header1.npartTotal[k];
	  Ngas= header1.npartTotal[0];
	}
      if(SnapInfo) printf(" (NumPart=%10d\tNgas=%10d)\n",NumPart,Ngas);

      for(k=0, ntot_withmasses=0; k<6; k++)
	{
	  if(header1.mass[k]==0)
	    ntot_withmasses+= header1.npart[k];
	}

      if(i==0)
	allocate_memory();

      SKIP;
      if(SnapInfo) printf("%10d\tPositions\n",dummy); dummy_total+=dummy+8;
      for(k=0,pc_new=pc;k<6;k++)
	{
	  for(n=0;n<header1.npart[k];n++)
	    {
	      fread(&P[pc_new].Pos[0], sizeof(float), 3, fd);
	      pc_new++;
	    }
	}
      SKIP;

      SKIP;
      if(SnapInfo) printf("%10d\tVelocities\n",dummy); dummy_total+=dummy+8;
      for(k=0,pc_new=pc;k<6;k++)
	{
	  for(n=0;n<header1.npart[k];n++)
	    {
	      fread(&P[pc_new].Vel[0], sizeof(float), 3, fd);
	      pc_new++;
	    }
	}
      SKIP;
    

      SKIP;
      if(SnapInfo) printf("%10d\tIDs\n",dummy); dummy_total+=dummy+8;
      for(k=0,pc_new=pc;k<6;k++)
	{
	  for(n=0;n<header1.npart[k];n++)
	    {
	      fread(&Id[pc_new], sizeof(int), 1, fd);
	      pc_new++;
	    }
	}
      SKIP;


      if(ntot_withmasses>0)
	{
	  SKIP;
	  if(SnapInfo)
	    if (quiteon==0) printf("%10d\tMasses (ntot_withmasses= %d)\n",dummy,ntot_withmasses);
	  else
	    {printf("ntot_withmasses = %d\n",ntot_withmasses); fflush(stdout);}
	  dummy_total+=dummy+8;
	}
      for(k=0, pc_new=pc; k<6; k++)
	{
	  for(n=0;n<header1.npart[k];n++)
	    {
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
      

	  /* --------- Gas Internal Energy --------- */
	  if(header1.npart[0]>0)
	    {
	     SKIP;
	     if(SnapInfo) printf("%10d\tInteral Energy (Ngas)\n",dummy); dummy_total+=dummy+8;
	     for(n=0, pc_sph=pc; n<header1.npart[0];n++)
	       {
	          fread(&P[pc_sph].U, sizeof(float), 1, fd);
	          pc_sph++;
	        }
	      SKIP;
	     }

	  /* --------- Gas Density ----------------- */
	  if(SnapFile && (header1.npart[0]>0))
	    {
		SKIP;
		if(SnapInfo) printf("%10d\tDensity (Ngas)\n",dummy); dummy_total+=dummy+8;
		for(n=0, pc_sph=pc; n<header1.npart[0];n++)
		  {
		    fread(&P[pc_sph].Rho, sizeof(float), 1, fd);
		    pc_sph++;
		  }
		SKIP;
	    }
	  else
	    {
		for(n=0, pc_sph=pc; n<header1.npart[0];n++)
		   {
			P[pc_sph].Rho= 0;
			pc_sph++;
		   }
	    }

	  /* -------- Electron Abundance --------- */
	  if(header1.flag_cooling && (header1.npart[0]>0))
	    {
	      SKIP;
	      if(SnapInfo) printf("%10d\tNe (Ngas)\n",dummy); dummy_total+=dummy+8;
	      for(n=0, pc_sph=pc; n<header1.npart[0];n++)
		{
		  fread(&P[pc_sph].Ne, sizeof(float), 1, fd);
		  pc_sph++;
		}
	      SKIP;
	    }
	  else
	    for(n=0, pc_sph=pc; n<header1.npart[0];n++)
	      {
		P[pc_sph].Ne= 1.0;
		pc_sph++;
	      }

	  /* -------- Neutral Hydrogen Density -------- */
	  if(header1.flag_cooling && (header1.npart[0]>0))
	  {
              SKIP;
	      if(SnapInfo) printf("%10d\tNH0 (Ngas)\n",dummy); dummy_total+=dummy+8;
              for(n=0, pc_sph=pc; n<header1.npart[0];n++)
                {
                  fread(&P[pc_sph].Nh, sizeof(float), 1, fd);
                  pc_sph++;
                }
              SKIP;
          }
          else
            for(n=0, pc_sph=pc; n<header1.npart[0];n++)
              {
                P[pc_sph].Nh= 0.0;
                pc_sph++;
	      }

#ifdef TJ_VERSION
	  /* --------- Formed Stellar Mass --------- */
	  if((header1.flag_sfr) && (header1.flag_stargens==0) && (header1.npart[0]>0))
	  {
              SKIP;
	      if(SnapInfo) printf("%10d\tMfs (Ngas)\n",dummy); dummy_total+=dummy+8;
              for(n=0, pc_sph=pc; n<header1.npart[0];n++)
                {
                  fread(&P[pc_sph].Mfs, sizeof(float), 1, fd);
                  pc_sph++;
                }
              SKIP;
          }
          else
            for(n=0, pc_sph=pc; n<header1.npart[0];n++)
              {
                P[pc_sph].Mfs = 0.0;
                pc_sph++;
              }

	  /* --------- Mass of Cold Clouds --------- */
	  if(header1.flag_multiphase && (header1.npart[0]>0))
	  {
              SKIP;
	      if(SnapInfo) printf("%10d\tMclouds (Ngas)\n",dummy); dummy_total+=dummy+8;
              for(n=0, pc_sph=pc; n<header1.npart[0];n++)
                {
                  fread(&P[pc_sph].Mclouds, sizeof(float), 1, fd);
                  pc_sph++;
                }
              SKIP;
          }
          else
            for(n=0, pc_sph=pc; n<header1.npart[0];n++)
              {
                P[pc_sph].Mclouds = 0.0;
                pc_sph++;
              }
#endif

	  /* --------- Gas Smoothing length --------- */
	  if(SnapFile && (header1.npart[0]>0))
	  {
              SKIP;
	      if(SnapInfo) printf("%10d\tHsml (Ngas)\n",dummy); dummy_total+=dummy+8;
              for(n=0, pc_sph=pc; n<header1.npart[0];n++)
                {
                  fread(&P[pc_sph].hsml, sizeof(float), 1, fd);
                  pc_sph++;
                }
              SKIP;
          }
          else
            for(n=0, pc_sph=pc; n<header1.npart[0];n++)
              {
                P[pc_sph].hsml = 0.0;
                pc_sph++;
              }

	  /* --------- Star Formation Rate ---------- */
	  if(header1.flag_sfr && (header1.npart[0]>0))
	  {
              SKIP;
	      if(SnapInfo) printf("%10d\tSfr (Ngas)\n",dummy); dummy_total+=dummy+8;
              for(n=0, pc_sph=pc; n<header1.npart[0];n++)
                {
                  fread(&P[pc_sph].sfr, sizeof(float), 1, fd);
                  pc_sph++;
                }
              SKIP;
          }
          else
            for(n=0, pc_sph=pc; n<header1.npart[0];n++)
              {
                P[pc_sph].sfr = 0.0;
                pc_sph++;
              }

	  /* --------- Mean formation redshift ------ */
	  if(header1.flag_stellarage)
	  {

#ifdef TJ_VERSION
	      if((header1.flag_stargens) && (header1.npart[4]>0))
#else
              if(header1.npart[4]>0)
#endif
		{
		  SKIP;
		  if(SnapInfo) printf("%10d\tStellarAge (Nstars)\n",dummy); dummy_total+=dummy+8;
		  first_n= header1.npart[0]+header1.npart[1]+header1.npart[2]+header1.npart[3]+1;
		  for(n=0, pc_sph=first_n; n<header1.npart[4];n++)
			{
			  fread(&P[pc_sph].meanz, sizeof(float), 1, fd);
			  pc_sph++;
			}
		  SKIP;
		}

#ifdef TJ_VERSION
	      if((!(header1.flag_stargens)) && (header1.npart[0]>0))
		{
              	SKIP;
	        if(SnapInfo) printf("%10d\tStellarAge (Ngas)\n",dummy); dummy_total+=dummy+8;
		for(n=0, pc_sph=pc; n<header1.npart[0];n++)
		   {
		     fread(&P[pc_sph].meanz, sizeof(float), 1, fd);
		     pc_sph++;
		   }
		SKIP;
		}
#endif
          }
          else
            for(n=0, pc_sph=pc; n<header1.npart[0];n++)
              {
                P[pc_sph].meanz = 0.0;
                pc_sph++;
              }


          /* --------- Turbulent Energy Reservior ------ */
#ifdef TJ_VERSION
          if(header1.flag_feedbacktp && (header1.npart[0]>0))
          {
              SKIP;
	      if(SnapInfo) printf("%10d\tUtp (Ngas)\n",dummy); dummy_total+=dummy+8;
              for(n=0, pc_sph=pc; n<(header1.npart[0]);n++)
                {
                  fread(&P[pc_sph].TpU, sizeof(float), 1, fd);
                  pc_sph++;
                }
              SKIP;
          }
          else
            for(n=0, pc_sph=pc; n<header1.npart[0];n++)
              {
                P[pc_sph].TpU = 0.0;
                pc_sph++;
	      }
#endif



#ifdef TJ_VERSION
          /* --------- Potential Energy ------ */
          if(header1.flag_snaphaspot)
          {
              SKIP;
	      if(SnapInfo) printf("%10d\tPotential\n",dummy); dummy_total+=dummy+8;
              for(k=0, pc_sph=pc; k<6;k++)
                {
		  for(n=0;n<header1.npart[k];n++)
		    {
                      fread(&P[pc_sph].Potential, sizeof(float), 1, fd);
                      pc_sph++;
		    }
                }
              SKIP;
          }
          else
	    for(k=0, pc_sph=pc; k<6;k++)
	      {
            	for(n=0;n<header1.npart[k];n++)
           	  {
 		    P[pc_sph].Potential = 0.0;
                    pc_sph++;
                  }
	      }
#endif


              
#ifdef TJ_VERSION
          /* --------- Metallicity - Gas ------ */
	  if(header1.flag_metals && (header1.npart[0]>0))
	  {
	      SKIP;
	      if(SnapInfo) printf("%10d\tMetallicity (Ngas)\n",dummy); dummy_total+=dummy+8;
	      for(n=0, pc_sph=pc; n<(header1.npart[0]);n++)
		{
		  fread(&P[pc_sph].Metallicity, sizeof(float), 1, fd);
		  pc_sph++;
		}
	      SKIP;
	  }
	  else
	    for(n=0, pc_sph=pc; n<NumPart;n++)
	      {
		P[pc_sph].Metallicity= 0.0;
		pc_sph++;
	      }


	  /* --------- Metallicity - Stars ------ */
	  if((header1.flag_metals) && (header1.npart[4]>0))
	    {
	      SKIP;
	      if(SnapInfo) printf("%10d\tMetallicity (Nstars)\n",dummy); dummy_total+=dummy+8;
	      first_n= header1.npart[0]+header1.npart[1]+header1.npart[2]+header1.npart[3]+1;
	      for(n=0, pc_sph=first_n; n<(header1.npart[4]);n++)
	        {
	          fread(&P[pc_sph].Metallicity, sizeof(float), 1, fd);
	          pc_sph++;
	        }
	      SKIP;
	    }
#else
          /* --------- Metallicity - Gas+Stars ------ */
          if(header1.flag_metals && ((header1.npart[0]+header1.npart[4])>0))
          {
              SKIP;
              if(SnapInfo) printf("%10d\tMetallicity (Ngas+Nstars)\n",dummy); dummy_total+=dummy+8;
              for(n=0, pc_sph=pc; n<(header1.npart[0]);n++)
                {
                  fread(&P[pc_sph].Metallicity, sizeof(float), 1, fd);
                  pc_sph++;
                }
              first_n= header1.npart[0]+header1.npart[1]+header1.npart[2]+header1.npart[3]+1;
              for(n=0, pc_sph=first_n; n<(header1.npart[4]);n++)
                {
                  fread(&P[pc_sph].Metallicity, sizeof(float), 1, fd);
                  pc_sph++;
                }
              SKIP;
          }
          else
            for(n=0, pc_sph=pc; n<NumPart;n++)
              {
                P[pc_sph].Metallicity= 0.0;
                pc_sph++;
              }
#endif


#ifdef TJ_VERSION
          /* --------- Total Radiated Energy ------ */
	  if(header1.flag_energydetail && (header1.npart[0]>0))
	  {
	    SKIP;
	    if(SnapInfo) printf("%10d\tTotal Radiated Energy (Ngas)\n",dummy); dummy_total+=dummy+8;
	    for(n=0, pc_sph=pc; n<(header1.npart[0]);n++)
	      {
		fread(&P[pc_sph].totrad, sizeof(float), 1, fd);
		pc_sph++;
	      }
	    SKIP;
	  }
	  else
	    for(n=0, pc_sph=pc; n<header1.npart[0];n++)
	      {
		P[pc_sph].totrad = 0.0;
		pc_sph++;
	      }


	  /* --------- Total Shocked Energy ------ */
	  if(header1.flag_energydetail && (header1.npart[0]>0))
	    {
	      SKIP;
	      if(SnapInfo) printf("%10d\tTotal Shocked Energy (Ngas)\n",dummy); dummy_total+=dummy+8;
	      for(n=0, pc_sph=pc; n<(header1.npart[0]);n++)
	        {
	          fread(&P[pc_sph].totshock, sizeof(float), 1, fd);
	          pc_sph++;
	        }
	      SKIP;
	    }
	  else
	    for(n=0, pc_sph=pc; n<header1.npart[0];n++)
	      {
	        P[pc_sph].totshock = 0.0;
	        pc_sph++;
	      }



	  /* --------- Total Fb Energy ------ */
	  if(header1.flag_energydetail && (header1.npart[0]>0))
	    {
	      SKIP;
	      if(SnapInfo) printf("%10d\tTotal Fb Energy (Ngas)\n",dummy); dummy_total+=dummy+8;
	      for(n=0, pc_sph=pc; n<(header1.npart[0]);n++)
	        {
	          fread(&P[pc_sph].totfb, sizeof(float), 1, fd);
		  pc_sph++;
		}
	      SKIP;
	    }
	  else
	    for(n=0, pc_sph=pc; n<header1.npart[0];n++)
	      {
	        P[pc_sph].totfb = 0.0;
	        pc_sph++;
	      }



          /* --------- Parent ID ------ */
          if((header1.flag_parentid) && (header1.npart[4]>0))
            {
              SKIP;
              if(SnapInfo) printf("%10d\tParent ID (Nstars)\n",dummy); dummy_total+=dummy+8;
	      first_n= header1.npart[0]+header1.npart[1]+header1.npart[2]+header1.npart[3]+1;
              for(n=0, pc_sph=first_n; n<(header1.npart[4]);n++)
                {
                  fread(&dog, sizeof(int), 1, fd);
		  P[pc_sph].ParentID= dog;
                  pc_sph++;
                }
              SKIP;
            }
/*
          else
            for(n=0, pc_sph=pc; n<header1.npart[0];n++)
              {
                P[pc_sph].ParentID= 0.0;
                pc_sph++;
              }
*/



          /* --------- Original Gas Mass ------ */
          if((header1.flag_starorig) && (header1.npart[4]>0))
            {
              SKIP;
              if(SnapInfo) printf("%10d\tOriginal Mass (Nstars)\n",dummy); dummy_total+=dummy+8;
	      first_n= header1.npart[0]+header1.npart[1]+header1.npart[2]+header1.npart[3]+1;
              for(n=0, pc_sph=first_n; n<(header1.npart[4]);n++)
                {
                  fread(&P[pc_sph].OrigMass, sizeof(float), 1, fd);
                  pc_sph++;
                }
              SKIP;
            }
/*
          else
            for(n=0, pc_sph=pc; n<header1.npart[4];n++)
              {
                P[pc_sph].OrigMass= 0.0;
                pc_sph++;
              }
*/



          /* --------- Original Gas Hsml ------ */
          if((header1.flag_starorig) && (header1.npart[4]>0))
            {
              SKIP;
              if(SnapInfo) printf("%10d\tOriginal Hsml (Nstars)\n",dummy); dummy_total+=dummy+8;
              first_n= header1.npart[0]+header1.npart[1]+header1.npart[2]+header1.npart[3]+1;
              for(n=0, pc_sph=first_n; n<(header1.npart[4]);n++)
                {
                  fread(&P[pc_sph].OrigHsml, sizeof(float), 1, fd);
                  pc_sph++;
                }
              SKIP;
            }
#endif

#ifndef TJ_VERSION
          /* --------- Potential Energy ------ */
          /* if(header1.flag_snaphaspot) */
          /* if(0==1) */
          if(header1.time>0)
          {   
              SKIP;
              if(SnapInfo) printf("%10d\tPotential\n",dummy); dummy_total+=dummy+8;
              for(k=0, pc_sph=pc; k<6;k++)
                { 
                  for(n=0;n<header1.npart[k];n++)
                    { 
                      fread(&P[pc_sph].Potential, sizeof(float), 1, fd);
                      pc_sph++;
                    }
                }
              SKIP;
          }
          else
            for(k=0, pc_sph=pc; k<6;k++)
              { 
                for(n=0;n<header1.npart[k];n++)
                  { 
                    P[pc_sph].Potential = 0.0;
                    pc_sph++;
                  }
              }


          /* --------- Black Hole Information ------ */
          /* if(0==1) */
          if((header1.npart[5]>0) && (header1.time>0))
	  {
              SKIP;
              if(SnapInfo) printf("%10d\tBlack Hole Mass\n",dummy); dummy_total+=dummy+8;
              for(n=0, pc_sph=pc; n<header1.npart[5];n++)
                { 
                   /* fread(&P[pc_sph].Potential, sizeof(float), 1, fd); */
                   pc_sph++;
                }
              SKIP;
              SKIP;
              if(SnapInfo) printf("%10d\tBlack Hole Mdot\n",dummy); dummy_total+=dummy+8;
              for(n=0, pc_sph=pc; n<header1.npart[5];n++)
                { 
                   /* fread(&P[pc_sph].Potential, sizeof(float), 1, fd); */
                   pc_sph++;
                }
              SKIP;
	  }
          


#endif



      fclose(fd);
    }

  if(SnapInfo)
    {
      printf("----------\n");
      printf("%10d\tTotal Bytes (included padding)\n\n",dummy_total);
    }


  Time= header1.time;
  Redshift= header1.time;
}




/* this routine allocates the memory for the 
 * particle data.
 */
int allocate_memory(void)
{
  /*printf("allocating memory...\n");*/

  if(!(P=malloc(NumPart*sizeof(struct particle_data))))
    {
      fprintf(stderr,"failed to allocate memory.\n");
      fprintf(stderr,"NumPart = %d\n",NumPart);
      fprintf(stderr,"sizeof = %d\n",sizeof(struct particle_data));
      fprintf(stderr,"\n\n");
      exit(0);
    }
  
  P--;   /* start with offset 1 */

  
  if(!(Id=malloc(NumPart*sizeof(int))))
    {
      fprintf(stderr,"failed to allocate memory.\n");
      fprintf(stderr,"NumPart = %d\n",NumPart);
      fprintf(stderr,"sizeof = %d\n",sizeof(int));
      fprintf(stderr,"\n\n");
      exit(0);
    }
  
  Id--;   /* start with offset 1 */

  /*printf("allocating memory...done\n");*/
}




/* This routine brings the particles back into
 * the order of their ID's.
 * NOTE: The routine only works if the ID's cover
 * the range from 1 to NumPart !
 * In other cases, one has to use more general
 * sorting routines.
 */
int reordering(void)
{
  int i,j;
  int idsource, idsave, dest;
  struct particle_data psave, psource;


  printf("reordering....\n");

  for(i=1; i<=NumPart; i++)
    {
      if(Id[i] != i)
	{
	  psource= P[i];
	  idsource=Id[i];
	  dest=Id[i];

	  do
	    {
	      psave= P[dest];
	      idsave=Id[dest];

	      P[dest]= psource;
	      Id[dest]= idsource;
	      
	      if(dest == i) 
		break;

	      psource= psave;
	      idsource=idsave;

	      dest=idsource;
	    }
	  while(1);
	}
    }

  printf("done.\n");

  Id++;   
  free(Id);

  printf("space for particle ID freed\n");
}






  











