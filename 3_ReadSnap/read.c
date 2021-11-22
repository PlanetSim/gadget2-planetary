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
int PrintTotalMass;
int PrintGasMass;
int PrintTotalPot;
int PrintMetals;
int PrintEnergyDetail;
int EOSAnalysis; /* PJC 14/12/15 */



int unit_conversion(void);
int write_std_to_file(char *fname);
int write_header(void);
int print_xy(void);
int print_xz(void);
int print_yz(void);
int list_r(void);
int print_gas(void);
int print_mfs(void);
int print_rho(void);
int print_sfr(void);
/*int print_gmass(void);*/
int print_gasmass(void);
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
  int  quiteon;
  char *p;


  if(argc<2)
    {
      printf("\n>ReadIC <IC filename> <[option]>\n\n");
      printf("options\n");
      printf("-------\n");
      printf(" -info   prints byte information for all fields present\n");
      printf(" -h      only the header information is printed to the screen\n");
      printf(" -m      prints the mass of a particle when displaying other information\n");
      printf(" -id     prints the ID for each particle.\n");
      printf(" -pot    prints the Potential for every particle.\n");
      printf(" -tm     prints the total mass of each type\n");
      printf(" -gm     prints the gas mass\n");
      printf(" -eos    prints the Pressure, Temperature and U of every particle.\n"); /* PJC 14/12/15 */
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
	quiteon= 0;
	if(!(strcmp(argv[i],"-info"))) ok = SnapInfo = 1;
	if(!(strcmp(argv[i],"-h"))) ok = ShowHeader = 1;
	if(!(strcmp(argv[i],"-tm"))) ok = PrintTotalMass =1;
	if(!(strcmp(argv[i],"-gm"))) ok = PrintGasMass = quiteon= 1;
	if(!(strcmp(argv[i],"-m"))) PrintDefault = ShowMass = 1;
	if(!(strcmp(argv[i],"-id"))) PrintDefault = ShowId = 1;
	if(!(strcmp(argv[i],"-pot"))) PrintDefault = ShowPot = 1;
	if(!(strcmp(argv[i],"-eos"))) PrintDefault = EOSAnalysis = 1; /* PJC 14/12/15 */

	if((ok==0) && (PrintDefault==0))
	{
		printf("Unknown parameter: %s\n",argv[i]);
		printf("Please use correct parameters.\n");
		exit(0); 
	}
  }


  load_snapshot(input_fname, files, quiteon);

  if(EOSAnalysis) pressure(&P); /* PJC 14/12/15 */

  if(Reorder) reordering();  /* call this routine only if your ID's are set properly */

  unit_conversion();  /* optional stuff */

  if(PrintDefault) write_std_to_file(input_fname);
  if(ShowHeader) write_header();
  if(PrintTotalMass) print_totalmass();
  if(PrintGasMass) print_gasmass();

  return(0);
}





/* --------------------------------------------
    here the particle data is at your disposal 
   -------------------------------------------- */
int write_std_to_file(char *fname)
{
  FILE *fd;
  long int N_TOT=0;
  int i;
  double vsqr;
  double nstuff;
 
  strcat(fname,".txt");

/*  if(!(fd=fopen("ic.txt", "w")))*/
  if(!(fd=fopen(fname, "w")))
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
  fprintf(fd, "Entropy in ICs    = %1d\n", header1.flag_entr_ics);
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
  fprintf(fd, "Gas Information (S, Rho, hsml");
  if(EOSAnalysis) fprintf(fd, ", P, T, U"); /* PJC 14/12/15 */
  fprintf(fd, ")\n-- ");
  if(ShowMass) fprintf(fd, "-------------- ");
  if(ShowId) fprintf(fd, "------- ");
  if(ShowPot) fprintf(fd, "------------ ");
  fprintf(fd, "---------------------                     ");
  fprintf(fd, "------------------------                   ");
  if(EOSAnalysis) fprintf(fd, "---------");
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
      //RAM edited to show entropy/U field instead of potential
      //if(ShowPot) fprintf(fd, "  %12.5le", P[i+1].Potential);
      if(ShowPot) fprintf(fd, "  %12.5le", P[i+1].U);

      fprintf(fd, " %13.6le %13.6le %13.6le",P[i+1].Pos[0], P[i+1].Pos[1], P[i+1].Pos[2]);
      fprintf(fd, " %13.6le %13.6le %13.6le",P[i+1].Vel[0], P[i+1].Vel[1], P[i+1].Vel[2]);

      if ((header1.npart[0]>0) && (i<header1.npart[0]))
        {
          fprintf(fd, " %13.6le %13.6le %13.6le",P[i+1].U, P[i+1].Rho, P[i+1].hsml);
	  if(EOSAnalysis) fprintf(fd, " %13.6le %13.6le %13.6le",P[i+1].P, P[i+1].T, P[i+1].IntE); /* PJC 14/12/15 */
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





/* --------------------------------------------
   -------------------------------------------- */
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



/* --------------------------------------------
   -------------------------------------------- */
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



/* --------------------------------------------
   -------------------------------------------- */
int print_gasmass(void)
{
  int type,i,pc;
  double GasMass=0;

  pc=1;
  for (i=0;i<header1.npart[0];i++)
    {
        GasMass += P[pc++].Mass;
    }

  printf(" %15.5le\n",GasMass);

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





