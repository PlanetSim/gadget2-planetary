#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "globvars.h"


int ShowMass;
int ShowId;
int ShowPot;
int ShowHeader;
int Reorder;
int PrintDefault;
int Printxy;
int Printxz;
int Printyz;
int PrintListR;
int PrintGas;
int PrintMFS;
int PrintRHO;
int PrintSFR;
int PrintGMass;
int PrintTotalMass;
int PrintTotalPot;
int PrintMetals;
int PrintEnergyDetail;



int unit_conversion(void);
int write_std_to_file(void);
int write_header(void);
int print_xy(void);
int print_xz(void);
int print_yz(void);
int list_r(void);
int print_gas(void);
int print_mfs(void);
int print_rho(void);
int print_sfr(void);
int print_gmass(void);
int print_totalmass(void);
int print_totalpotential(void);
int print_metals(void);
int print_energydetail(void);



/* Here we load a snapshot file. It can be distributed
 * onto several files (for files>1).
 * The particles are brought back into the order
 * implied by their ID's.
 * A unit conversion routine is called to do unit
 * conversion, and to evaluate the gas temperature.
 */
int main(int argc, char **argv)
{
  char input_fname[200];
  int  type, files, i, ok;
  char *p;

  if(argc<2)
    {
      printf("\n>ReadIC <IC filename> <[parameters]>\n\n");
      printf("-info   prints byte information for all fields present\n");
      printf("-xz     x vs. z for all particles into file xz.txt\n");
      printf("-yz     y vs. z for all particles into file yz.txt\n");
      printf("-r      r for all particles into files, r_gas.txt, r_disk.txt, r_halo.txt\n");
      printf("-gas    prints only the gas information\n");
      printf("-mfs    formed stellar mass is printed into file mfs.txt\n");
      printf("-rho    gas density is printed for each particle to rho.txt\n");
      printf("-sfr    current star formation rate is printed to sfr.txt (bad name for this)\n");
      printf("-gm\n");
      printf("-h      only the header information is printed to the screen\n");
      printf("-m      prints the mass of a particle when displaying other information\n");
      printf("-id     prints the ID for each particle.\n");
      printf("-pot    prints the Potential for every particle.\n");
      printf("-tpot   prints the total Potential Energy for every group.\n");
      printf("-tm     prints the total mass of each type\n");
      printf("-metals prints metallicity of gas and star particles\n");
      printf("-ed     printf the energy detail\n");
      printf("\n\n");
      exit(0);
    }

  files=1;                               /* number of files per snapshot */
  ShowMass=0;
  ShowId=0;
  ShowPot=0;
  Reorder=0;
  PrintDefault=0;
  SnapFile=0;

  strcpy(input_fname, argv[1]);

  /* problem, what if we give a path rather then just filename 
  if(!strncmp(input_fname, "snapshot", 8))  SnapFile = 1; */
  if(strstr(input_fname,"snapshot")!=0)  SnapFile = 1;

  if(argc==2) PrintDefault = 1;

  for(i=2; i<argc; i++)
  {
	ok = 0;
	if(!(strcmp(argv[i],"-info"))) ok = SnapInfo = 1;
	if(!(strcmp(argv[i],"-xy")))  ok = Printxy = 1;
	if(!(strcmp(argv[i],"-xz"))) ok = Printxz = 1;
	if(!(strcmp(argv[i],"-yz"))) ok = Printyz = 1;
	if(!(strcmp(argv[i],"-r"))) ok = PrintListR = 1;
	if(!(strcmp(argv[i],"-gas"))) ok = PrintGas = 1;
	if(!(strcmp(argv[i],"-mfs"))) ok = PrintMFS = 1;
	if(!(strcmp(argv[i],"-rho"))) ok = PrintRHO = 1;
	if(!(strcmp(argv[i],"-sfr"))) ok = PrintSFR = 1;
	if(!(strcmp(argv[i],"-gm"))) ok = PrintGMass = 1;
	if(!(strcmp(argv[i],"-h"))) ok = ShowHeader = 1;
	if(!(strcmp(argv[i],"-tm"))) ok = PrintTotalMass =1;
	if(!(strcmp(argv[i],"-tpot"))) ok = PrintTotalPot =1;
	if(!(strcmp(argv[i],"-metals"))) ok = PrintMetals = 1;
	if(!(strcmp(argv[i],"-ed"))) ok = PrintEnergyDetail = 1;

	if(!(strcmp(argv[i],"-m"))) PrintDefault = ShowMass = 1;
	if(!(strcmp(argv[i],"-id"))) PrintDefault = ShowId = 1;
	if(!(strcmp(argv[i],"-pot"))) PrintDefault = ShowPot = 1;
	if(!(strcmp(argv[i],"-reorder"))) PrintDefault = Reorder = 1;
	if(!(strcmp(argv[i],"-all"))) ShowId = ShowMass = PrintDefault = 1;

	if((ok==0) && (PrintDefault==0))
	{
		printf("Unknown parameter: %s\n",argv[i]);
		printf("Please use correct parameters.\n");
		exit(0); 
	}
  }


  load_snapshot(input_fname, files);

  if(Reorder) reordering();  /* call this routine only if your ID's are set properly */

  unit_conversion();  /* optional stuff */

  if(PrintDefault) write_std_to_file();
  if(ShowHeader) write_header();
  if(Printxy) print_xy();
  if(Printxz) print_xz();
  if(Printyz) print_yz();
  if(PrintListR) list_r();
  if(PrintGas) print_gas();
  if(PrintMFS) print_mfs();
  if(PrintRHO) print_rho();
  if(PrintSFR) print_sfr();
  if(PrintGMass) print_gmass();
  if(PrintTotalMass) print_totalmass();
  if(PrintTotalPot) print_totalpotential();
  if(PrintMetals) print_metals();
  if(PrintEnergyDetail) print_energydetail();


  return(0);
}





/* here the particle data is at your disposal 
 */
int write_std_to_file(void)
{
  FILE *fd;
  long int N_TOT=0;
  int i;
  double vsqr;
  double nstuff;

  if(!(fd=fopen("ic.txt", "w")))
    {
      printf("can't open file: snapshot.txt\n");
      exit(0);
    }

  fprintf(fd, "Header\n");
  fprintf(fd, "------\n");
  fprintf(fd, "NGas       = %ld\n", header1.npart[0]);
  fprintf(fd, "NHalo      = %ld\n", header1.npart[1]);
  fprintf(fd, "NDisk      = %ld\n", header1.npart[2]);
  fprintf(fd, "NBulge     = %ld\n", header1.npart[3]);
  fprintf(fd, "NStars     = %ld\n", header1.npart[4]);
  fprintf(fd, "N??        = %ld\n", header1.npart[5]);

  fprintf(fd, "Gas Mass   = %lg\n", header1.mass[0]);
  fprintf(fd, "Halo Mass  = %lg\n", header1.mass[1]);
  fprintf(fd, "Disk Mass  = %lg\n", header1.mass[2]);
  fprintf(fd, "Bulge Mass = %lg\n", header1.mass[3]);
  fprintf(fd, "Stars Mass = %lg\n", header1.mass[4]);
  /*fprintf(fd, "??? Mass = %lg\n", header1.mass[5]);*/

  fprintf(fd, "Time              = %lg\n", header1.time);
  fprintf(fd, "Redshift          = %lg\n", header1.redshift);
  fprintf(fd, "Flag SFR          = %ld\n", header1.flag_sfr);
  fprintf(fd, "Flag FeedbackTp   = %ld\n", header1.flag_feedbacktp);

  fprintf(fd, "Total NGas        = %ld\n", header1.npartTotal[0]);
  fprintf(fd, "Total NHalo       = %ld\n", header1.npartTotal[1]);
  fprintf(fd, "Total NDisk       = %ld\n", header1.npartTotal[2]);
  fprintf(fd, "Total NBulge      = %ld\n", header1.npartTotal[3]);
  fprintf(fd, "Total NStars      = %ld\n", header1.npartTotal[4]);
  /*fprintf(fd, "Total N??  = %ld\n", header1.npartTotal[5]);*/

  fprintf(fd, "Flag Cooling      = %ld\n", header1.flag_cooling);
  fprintf(fd, "Number of Files   = %ld\n", header1.num_files);
  fprintf(fd, "Box Size          = %lg\n", header1.BoxSize);
  fprintf(fd, "Omega             = %lg\n", header1.Omega0);
  fprintf(fd, "OLambda           = %lg\n", header1.OmegaLambda);
  fprintf(fd, "HubbleP           = %lg\n", header1.HubbleParam); 

#ifdef TJ_VERSION
  fprintf(fd, "Flag Multiphase   = %ld\n", header1.flag_multiphase);
#endif
  fprintf(fd, "Flag Stellarage   = %ld\n", header1.flag_stellarage);
#ifdef TJ_VERSION
  fprintf(fd, "Flag SFRHistogram = %ld\n", header1.flag_sfrhistogram);
  fprintf(fd, "Flag Stargens     = %ld\n", header1.flag_stargens);
  fprintf(fd, "Flag SnapHasPot   = %ld\n", header1.flag_snaphaspot);
#endif
  fprintf(fd, "Flag Metals       = %ld\n", header1.flag_metals);
#ifdef TJ_VERSION
  fprintf(fd, "Flag EnergyDetail = %ld\n", header1.flag_energydetail);
  fprintf(fd, "Flag ParentID     = %ld\n", header1.flag_parentid);
  fprintf(fd, "Flag StarOrigInfo = %ld\n", header1.flag_starorig);
#endif



  printf("Writing Data ..... ");
  fprintf(fd, "\nTy ");
  if(ShowMass) fprintf(fd, "Mass          ");
  if(ShowId) fprintf(fd, "ID      ");
  if(ShowPot) fprintf(fd, "Potential    ");
  fprintf(fd, "Position Data (x,y,z)                     ");
  fprintf(fd, "Velocity Data (vx,vy,vz)                   ");
  fprintf(fd, "Gas Information (U, Rho, Ne)\n");
  fprintf(fd, "-- ");
  if(ShowMass) fprintf(fd, "-------------- ");
  if(ShowId) fprintf(fd, "------- ");
  if(ShowPot) fprintf(fd, "------------ ");
  fprintf(fd, "---------------------                     ");
  fprintf(fd, "------------------------                   ");
  fprintf(fd, "----------------------------\n");

  for (i=0;i<6;i++) N_TOT+=header1.npart[i];

  /*fprintf(fd, "N Total = %ld\n", N_TOT);*/

  for (i=0;i<N_TOT;i++)
    {
      fprintf(fd, "%2d",P[i+1].Type);

      if(ShowMass) fprintf(fd, " %13.6le", P[i+1].Mass);
#ifdef TJ_VERSION
      if(ShowId) fprintf(fd, " %6d", Id[i+1]);
#else
      if(ShowId)
        if(P[i+1].Type==4)
                fprintf(fd, " %d", Id[i+1]);
        else
                fprintf(fd, " %6d", Id[i+1]);
#endif
      if(ShowPot) fprintf(fd, "  %12.5le", P[i+1].Potential);

      fprintf(fd, " %13.6le %13.6le %13.6le",P[i+1].Pos[0], P[i+1].Pos[1], P[i+1].Pos[2]);
      fprintf(fd, " %13.6le %13.6le %13.6le",P[i+1].Vel[0], P[i+1].Vel[1], P[i+1].Vel[2]);

      if ((header1.npart[0]>0) && (i<header1.npart[0]))
        {
          fprintf(fd, " %13.6le %13.6le %13.6le",P[i+1].U, P[i+1].Rho, P[i+1].Ne);
        }

      /* Stellar Info */
      nstuff= header1.npart[0]+header1.npart[1]+header1.npart[2]+header1.npart[3];
#ifdef TJ_VERSION
      if((header1.npart[4]>0) && (i>=nstuff) && (header1.flag_parentid>0))
	{
	  fprintf(fd, " %6d",P[i+1].ParentID);
	}
      if((header1.npart[4]>0) && (i>=nstuff) && (header1.flag_starorig>0))
        {
          fprintf(fd, " %13.6le %13.6le",P[i+1].OrigMass, P[i+1].OrigHsml);
        }
#endif
      if((header1.npart[4]>0) && (i>=nstuff) && (header1.flag_stellarage>0))
        {
          fprintf(fd, " %13.6le",P[i+1].meanz);
        }
      fprintf(fd, "\n");
    }
  printf("     done.\n");


  fclose(fd);
}


int write_header(void)
{
  FILE *fd;
  int i;

  if(SnapFile)
	printf("\nSnapshot Header\n");
  else
	printf("\nIC Header\n");
  printf("-------------------------------\n");
  printf("NGas   = %7ld  with mass = %15.8le\n", header1.npart[0], header1.mass[0]);
  printf("NHalo  = %7ld  with mass = %15.8le\n", header1.npart[1], header1.mass[1]);
  printf("NDisk  = %7ld  with mass = %15.8le\n", header1.npart[2], header1.mass[2]);
  printf("NBulge = %7ld  with mass = %15.8le\n", header1.npart[3], header1.mass[3]);
  printf("NStars = %7ld  with mass = %15.8le\n", header1.npart[4], header1.mass[4]);
  printf("N??    = %7ld  with mass = %15.8le\n", header1.npart[5], header1.mass[5]);

  printf("Time              = %lg\n", header1.time);
  printf("Redshift          = %lg\n", header1.redshift);
  printf("Flag SFR          = %ld\n", header1.flag_sfr);
  printf("Flag FeedbackTp   = %ld\n", header1.flag_feedbacktp);

  if(header1.npart[0]!=header1.npartTotal[0]) printf("Total NGas = %ld\n", header1.npartTotal[0]);
  if(header1.npart[1]!=header1.npartTotal[1]) printf("Total NHalo  = %ld\n", header1.npartTotal[1]);
  if(header1.npart[2]!=header1.npartTotal[2]) printf("Total NDisk  = %ld\n", header1.npartTotal[2]);
  if(header1.npart[3]!=header1.npartTotal[3]) printf("Total NBulge = %ld\n", header1.npartTotal[3]);
  if(header1.npart[4]!=header1.npartTotal[4]) printf("Total NStars = %ld\n", header1.npartTotal[4]);
  if(header1.npart[5]!=header1.npartTotal[5]) printf("Total N??  = %ld\n", header1.npartTotal[5]);

  printf("Flag Cooling      = %ld\n", header1.flag_cooling);
  printf("Number of Files   = %ld\n", header1.num_files);
  printf("Box Size          = %lg\n", header1.BoxSize);
  printf("Omega             = %lg\n", header1.Omega0);
  printf("OLambda           = %lg\n", header1.OmegaLambda);
  printf("HubbleP           = %lg\n", header1.HubbleParam);

#ifdef TJ_VERSION
  printf("Flag Multiphase   = %ld\n", header1.flag_multiphase);
#endif
  printf("Flag Stellarage   = %ld\n", header1.flag_stellarage);
#ifdef TJ_VERSION
  printf("Flag SFRHistogram = %ld\n", header1.flag_sfrhistogram);
  printf("Flag StarGens     = %ld\n", header1.flag_stargens);
  printf("Flag SnapHasPot   = %ld\n", header1.flag_snaphaspot);
#endif
  printf("Flag Metals       = %ld\n", header1.flag_metals);
#ifdef TJ_VERSION
  printf("Flag EnergyDetail = %ld\n", header1.flag_energydetail);
  printf("Flag ParentID     = %ld\n", header1.flag_parentid);
  printf("Flag StarOrigInfo = %ld\n", header1.flag_starorig);
#endif

  printf("\n\n");

  return(1);
}


int print_xy(void)
{
  FILE *fd;
  int i;
  int N_TOT=0;

  if(!(fd=fopen("xy.txt", "w")))
    {
      printf("can't open file: xy.txt\n");
      exit(0);
    }


  for (i=0;i<6;i++) N_TOT+=header1.npart[i];

  for (i=0;i<N_TOT;i++)
      fprintf(fd, " %15.8le %15.8le\n",P[i+1].Pos[0], P[i+1].Pos[1]);

  fclose(fd);

  return(1);
}


int print_xz(void)
{
  FILE *fd;
  int i;
  int N_TOT=0;

  if(!(fd=fopen("xz.txt", "w")))
    {
      printf("can't open file: xz.txt\n");
      exit(0);
    }

  for (i=0;i<6;i++) N_TOT+=header1.npart[i];

  for (i=0;i<N_TOT;i++)
      fprintf(fd, " %15.8le %15.8le\n",P[i+1].Pos[0], P[i+1].Pos[2]);

  fclose(fd);

  return(1);
}


int print_yz(void)
{
  FILE *fd;
  int i;
  int N_TOT=0;

  if(!(fd=fopen("yz.txt", "w")))
    {
      printf("can't open file: yz.txt\n");
      exit(0);
    }

  for (i=0;i<6;i++) N_TOT+=header1.npart[i];

  for (i=0;i<N_TOT;i++)
      fprintf(fd, " %15.8le %15.8le\n",P[i+1].Pos[1], P[i+1].Pos[2]);

  fclose(fd);

  return(1);
}


int list_r(void)
{
  FILE *fg,*fh,*fd;
  int i;
  int N_TOT=0;
  double r;

  if(!(fg=fopen("r_gas.txt", "w")))
    {
      printf("can't open file: r_gas.txt\n");
      exit(0);
    }

  if(!(fh=fopen("r_halo.txt", "w")))
    {
      printf("can't open file: r_halo.txt\n");
      exit(0);
    }

  if(!(fd=fopen("r_disk.txt", "w")))
    {
      printf("can't open file: r_disk.txt\n");
      exit(0);
    }

  for (i=0;i<header1.npart[0];i++)
  {
      r = sqrt(P[i+1].Pos[0]*P[i+1].Pos[0] + P[i+1].Pos[1]*P[i+1].Pos[1] + P[i+1].Pos[2]*P[i+1].Pos[2]);
      fprintf(fg, " %15.8le\n",r);
  }
  printf("done printing gas r's\n");  fflush(stdout);

  for (i=header1.npart[0];i<header1.npart[0]+header1.npart[1];i++)
  {
      r = sqrt(P[i+1].Pos[0]*P[i+1].Pos[0] + P[i+1].Pos[1]*P[i+1].Pos[1] + P[i+1].Pos[3]*P[i+1].Pos[3]);
      fprintf(fh, " %15.8le\n",r);
  }
  printf("done printing halo r's\n");  fflush(stdout);

  for (i=(header1.npart[0]+header1.npart[1]);i<(header1.npart[0]+header1.npart[1]+header1.npart[2]);i++)
  {
      r = sqrt(P[i+1].Pos[0]*P[i+1].Pos[0] + P[i+1].Pos[1]*P[i+1].Pos[1] + P[i+1].Pos[2]*P[i+1].Pos[2]);
      fprintf(fd, " %15.8le\n",r);
  }
  printf("done printing disk r's\n");  fflush(stdout);


  fclose(fg);
  fclose(fh);
  fclose(fd);

  return(1);
}


int print_mfs(void)
{
  FILE *fd;
  int i;
  int N_TOT=0;

  if(!(fd=fopen("mfs.txt", "w")))
    {
      printf("can't open file: mfs.txt\n");
      exit(0);
    }

  for (i=0;i<header1.npart[0];i++)
  {
      fprintf(fd, " %15.8le\n",P[i+1].Mfs);
  }

  fclose(fd);

  return(1);
}


int print_sfr(void)
{
  FILE *fd;
  int i;
  int N_TOT=0;

  if(!(fd=fopen("sfr.txt", "w")))
    {
      printf("can't open file: sfr.txt\n");
      exit(0);
    }

  for (i=0;i<header1.npart[0];i++)
  {
      fprintf(fd, " %15.8le\n",P[i+1].sfr);
  }

  fclose(fd);

  return(1);
}


int print_rho(void)
{
  FILE *fd;
  int i;
  int N_TOT=0;

  if(!(fd=fopen("rho.txt", "w")))
    {
      printf("can't open file: rho.txt\n");
      exit(0);
    }

  for (i=0;i<header1.npart[0];i++)
  {
      fprintf(fd, " %15.8le\n",P[i+1].Rho);
  }

  fclose(fd);

  return(1);
}



int print_gas(void)
{
  FILE *fd;
  int i;
  int N_TOT=0;

  if(!(fd=fopen("gas.txt", "w")))
    {
      printf("can't open file: gas.txt\n");
      exit(0);
    }

  fprintf(fd, "Mass         ");
  fprintf(fd, "ID       ");
  /*fprintf(fd, "Position Data (x,y,z)               ");*/
  /*fprintf(fd, "Velocity Data (vx,vy,vz)             ");*/
  fprintf(fd, "U           Rho          Ne          Nh         Mfs");
  fprintf(fd, "        Mclouds     hsml         sfr         meanz        TpU\n"); 
  fprintf(fd, "----------- ");
  fprintf(fd, "------ ");
  fprintf(fd, "----------- ----------- ----------- ----------- -----------");
  fprintf(fd, "----------- ----------- ----------- ----------- -----------\n");

  for (i=0;i<header1.npart[0];i++)
    {
      fprintf(fd, "%11.4le", P[i+1].Mass);
      fprintf(fd, " %6d", Id[i+1]);

      /*fprintf(fd, " %11.4le %11.4le %11.4le",P[i+1].Pos[0], P[i+1].Pos[1], P[i+1].Pos[2]);*/
      /*fprintf(fd, " %11.4le %11.4le %11.4le",P[i+1].Vel[0], P[i+1].Vel[1], P[i+1].Vel[2]);*/

      fprintf(fd, " %11.4le", P[i+1].U);
      fprintf(fd, " %11.4le", P[i+1].Rho);
      fprintf(fd, " %11.4le", P[i+1].Ne);
      fprintf(fd, " %11.4le", P[i+1].Nh);
      fprintf(fd, " %11.4le", P[i+1].Mfs);
      fprintf(fd, " %11.4le", P[i+1].Mclouds);
      fprintf(fd, " %11.4le", P[i+1].hsml);
      fprintf(fd, " %11.4le", P[i+1].sfr);
      fprintf(fd, " %11.4le", P[i+1].meanz);
      fprintf(fd, " %11.4le", P[i+1].TpU);

      fprintf(fd, "\n");
    }


  fclose(fd);

  return(1);
}


int print_gmass(void)
{
  FILE *fd;
  int i;
  double GMass=0;
  double FSMass=0;

  /*if(!(fd=fopen("gm.txt", "w")))
    {
      printf("can't open file: gm.txt\n");
      exit(0);
    }*/

  for (i=0;i<header1.npart[0];i++)
  {
	GMass += P[i+1].Mass;
	FSMass += P[i+1].Mfs;
  }

  printf("%5.3g  %15.8le %15.8le %3d\n",Time,GMass,FSMass,header1.npart[4]);

  /*fclose(fd);*/

  return(1);
}


int print_totalmass(void)
{
  int type,i,pc;
  double TotMass=0, GrandTotal=0;

  for (type=0,pc=1; type<6; type++)
  {
    for (i=0;i<header1.npart[type];i++)
    {
        TotMass += P[pc++].Mass;
    }
    if (TotMass!=0) 
    {
	printf("Type %d has total mass %15.5le\n",type,TotMass);
	GrandTotal += TotMass;
	TotMass=0;
    }
  }
    printf("                      --------------\n");
    printf("                       %15.5le\n",GrandTotal);


  return(1);
}



int print_totalpotential(void)
{
  int type,i,pc;
  double TotPot=0, GrandTotal=0;

  for (type=0,pc=1; type<6; type++)
  {
    for (i=0;i<header1.npart[type];i++)
    {
        TotPot += P[pc++].Potential;
    }
    if (TotPot!=0) 
    {
        printf("Type %d has total potential %15.5le\n",type,TotPot);
	GrandTotal += TotPot;
        TotPot=0;
    }
  }
  printf("                      --------------\n");
  printf("                       %15.5le\n",GrandTotal);

  return(1);
}




int print_metals(void)
{
  FILE *fd;
  int i,first_i;
  int N_TOT=0;

  if(!(fd=fopen("metals.txt", "w")))
    {
      printf("can't open file: metals.txt\n");
      exit(0);
    }

  for (i=0;i<header1.npart[0];i++)
  {
      fprintf(fd, "%6d %6d  %15.8le\n",P[i+1].Type,Id[i+1],P[i+1].Metallicity);
  }

  first_i= header1.npart[0]+header1.npart[1]+header1.npart[2]+header1.npart[3];
  for (i=first_i;i<first_i+header1.npart[4];i++)
  {
      fprintf(fd, "%6d %6d  %15.8le\n",P[i+1].Type,Id[i+1],P[i+1].Metallicity);
  }

  fclose(fd);

  return(1);
}




int print_energydetail(void)
{ 
  int type,i,pc;
  double TotRad=0, TotSh=0, TotFb= 0, GrandTotal=0;
  
  for (i=0;i<header1.npart[0];i++)
    {
        TotRad += P[i+1].totrad;
	TotSh += P[i+1].totshock;
	TotFb += P[i+1].totfb;
    }
  GrandTotal= -TotRad + TotSh + TotFb;
  printf("\nTotal Radiated     %15.5le\n",-TotRad);
  printf("Total Shock Heated %15.5le\n",TotSh);
  printf("Total Fb           %15.5le\n",TotFb);
  printf("                 ------------\n");
  printf("                   %15.5le\n",GrandTotal);

  
  return(1);
}







/* this template shows how one may convert from Gadget's units
 * to cgs units.
 * In this example, the temperate of the gas is computed.
 * (assuming that the electron density in units of the hydrogen density
 * was computed by the code. This is done if cooling is enabled.)
 */
int unit_conversion(void)
{
  double GRAVITY, BOLTZMANN, PROTONMASS;
  double UnitLength_in_cm, UnitMass_in_g, UnitVelocity_in_cm_per_s;
  double UnitTime_in_s, UnitDensity_in_cgs, UnitPressure_in_cgs, UnitEnergy_in_cgs;  
  double G, Xh, HubbleParam;

  int i;
  double MeanWeight, u, gamma;

  /* physical constants in cgs units */
  GRAVITY   = 6.672e-8;
  BOLTZMANN = 1.3806e-16;
  PROTONMASS = 1.6726e-24;

  /* internal unit system of the code */
  UnitLength_in_cm= 3.085678e21;   /*  code length unit in cm/h */
  UnitMass_in_g= 1.989e43;         /*  code mass unit in g/h */
  UnitVelocity_in_cm_per_s= 1.0e5;

  UnitTime_in_s= UnitLength_in_cm / UnitVelocity_in_cm_per_s;
  UnitDensity_in_cgs= UnitMass_in_g/ pow(UnitLength_in_cm,3);
  UnitPressure_in_cgs= UnitMass_in_g/ UnitLength_in_cm/ pow(UnitTime_in_s,2);
  UnitEnergy_in_cgs= UnitMass_in_g * pow(UnitLength_in_cm,2) / pow(UnitTime_in_s,2);

  G=GRAVITY/ pow(UnitLength_in_cm,3) * UnitMass_in_g * pow(UnitTime_in_s,2);


  Xh= 0.76;  /* mass fraction of hydrogen */
  HubbleParam= 0.65;


  for(i=1; i<=NumPart; i++)
    {
      if(P[i].Type==0)  /* gas particle */
	{
	  MeanWeight= 4.0/(3*Xh+1+4*Xh*P[i].Ne) * PROTONMASS;

	  /* convert internal energy to cgs units */

	  u  = P[i].U * UnitEnergy_in_cgs/ UnitMass_in_g;

	  gamma= 5.0/3;
	 
	  /* get temperature in Kelvin */

	  P[i].Temp= MeanWeight/BOLTZMANN * (gamma-1) * u;
	}
    }
}





