#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>

#include "globvars.h"
#include "nrutils/nrutil.h"

/**********************************************************
 EvolutionTables
 Takes a directory of Gadget2 snapshots and outputs a dir
 of ascii tables, one for each particle ID, describing the
 time evolution of each property of that particle.
***********************************************************/



int main( int argc, char **argv ) {

  char pfile[200];
  int i, j, count;
  char buf[200];
  char snapbasebuf[200];
  int * idsmap;
  unsigned long * timesindex;
  double * times;
  int * ids;
  short * mats;
  double ** pxs;
  double ** pys;
  double ** pzs;
  double ** vxs;
  double ** vys;
  double ** vzs;
  double ** rhos;
  double ** Us;
  double ** nes;
  double ** hs;
  double ** pots;
  double ** axs;
  double ** ays;
  double ** azs;
  double ** Ss;
  double ** Ts;
  double ** Ps;
  double ** Cs;
  double ** tss;
  if( argc != 2 ) {      
    printf("\n>./FindPeak <parameter file>\n\n");
    exit(0);
  }

  strcpy(pfile, argv[1]);

  printf( "Reading parameters from: %s\n", pfile );
  read_param( pfile );

  //set up EOS table
  struct eos_table *eostable[nMat];
  //load in EOS tables 
  for( i = 0; i < nMat; i++ ) {
    sprintf( buf, "%s%d.txt", eostablebase, i+1 );
    printf( "Reading EOS table: %s\n", buf );
    eostable[i] = eos_table_import( buf );
  }

  //use the dirent library to get a list of files in the input directory
  DIR *input;
  struct dirent *ent;
  printf( "Opening input directory: %s\n", inputdir );
  input = opendir(inputdir);
  if( input == NULL ) {
    printf("Error opening directory %s.\n", inputdir);
    exit(0);
  }

  //get number of files in dir
  long int front = telldir( input );
  int nFiles = 0;
  while( (ent = readdir(input)) != NULL ) {
    strncpy( snapbasebuf, ent->d_name, strlen(snapbase) );
    if( strcmp( snapbasebuf, snapbase ) == 0 ) {
      nFiles++;
    }
  }
  seekdir( input, front );

  printf( "Using %d files.\n", nFiles );
  times = malloc( nFiles * sizeof(double) );

  // loop through files
  int first = 1;
  while( (ent = readdir(input)) != NULL ) {

    //get the filename
    char filename[200];
    strcpy( filename, ent->d_name );
    //check for the correct sort of file, move on if not
    strncpy( snapbasebuf, filename, strlen(snapbase) );
    if( strcmp( snapbasebuf, snapbase ) != 0 ) {
      continue;
    }
    
    //load this snapshot
    sprintf( buf, "%s/%s", inputdir, filename );
    load_snapshot( buf, _NOFILES );
    
    int snapnum = atoi( strdup( filename+(strlen(filename)-3) ) );

    //if it's the first snapshot then allocate memory
    if( first == 1 ) {

      //Total number of particles by summing particle types
      npart = 0;
      for( i = 0; i<6; i++ ) npart += header1.npart[i];

      //allocate memory 
      printf( "Allocating memory for %d particles.\n", npart );
      ids = malloc( npart * sizeof(int) );
      ids -= 1;
      mats = malloc( npart * sizeof(short) );
      mats -= 1;
      pxs = dmatrix( 1, npart, 1, nFiles );
      pys = dmatrix( 1, npart, 1, nFiles );
      pzs = dmatrix( 1, npart, 1, nFiles );
      vxs = dmatrix( 1, npart, 1, nFiles );
      vys = dmatrix( 1, npart, 1, nFiles );
      vzs = dmatrix( 1, npart, 1, nFiles );
      rhos = dmatrix( 1, npart, 1, nFiles );
      Us = dmatrix( 1, npart, 1, nFiles );
      nes = dmatrix( 1, npart, 1, nFiles );
      hs = dmatrix( 1, npart, 1, nFiles );
      pots = dmatrix( 1, npart, 1, nFiles );
      axs = dmatrix( 1, npart, 1, nFiles );
      ays = dmatrix( 1, npart, 1, nFiles );
      azs = dmatrix( 1, npart, 1, nFiles );
      Ss = dmatrix( 1, npart, 1, nFiles );
      Ps = dmatrix( 1, npart, 1, nFiles );
      Ts = dmatrix( 1, npart, 1, nFiles );
      Cs = dmatrix( 1, npart, 1, nFiles );
      tss = dmatrix( 1, npart, 1, nFiles );
      
      //set up initial values
      int maxid = 0;
      for( i = 1; i <= npart; i++ ) {
	mats[i] = calculate_material( P[i].Id );
	ids[i] = P[i].Id;
	if( P[i].Id > maxid ) {
	  maxid = P[i].Id;
	}
      }
      idsmap = malloc( maxid * sizeof(int) );
      idsmap -= 1;
      for( i = 1; i <= npart; i++ ) {
	idsmap[P[i].Id] = i;
      }
      first = 0;
      count = 1;
      printf( "Of %d files:\n[%d", nFiles, count );

    }
    
    times[count] = header1.time;
    // run through all particles in the snapshot
    for( i = 1; i <= npart; i++ ) {

      int id = P[i].Id;

      pxs[idsmap[id]][count] = P[i].Pos[0];
      pys[idsmap[id]][count] = P[i].Pos[1];
      pzs[idsmap[id]][count] = P[i].Pos[2];
      vxs[idsmap[id]][count] = P[i].Vel[0];
      vys[idsmap[id]][count] = P[i].Vel[1];
      vzs[idsmap[id]][count] = P[i].Vel[2];
      rhos[idsmap[id]][count] = P[i].Rho;
      Us[idsmap[id]][count] = eos_table_interp( P[i].Rho, P[i].S, eostable[mats[idsmap[id]]-1], EOS_INT_ENERGY );
      nes[idsmap[id]][count] = P[i].Ne;
      hs[idsmap[id]][count] = P[i].hsml;
      pots[idsmap[id]][count] = P[i].Potential;
      axs[idsmap[id]][count] = P[i].Acc[0];
      ays[idsmap[id]][count] = P[i].Acc[1];
      azs[idsmap[id]][count] = P[i].Acc[2];
      Ss[idsmap[id]][count] = P[i].S;
      Ts[idsmap[id]][count] = eos_table_interp( P[i].Rho, P[i].S, eostable[mats[idsmap[id]]-1], EOS_TEMPERATURE );
      Ps[idsmap[id]][count] = eos_table_interp( P[i].Rho, P[i].S, eostable[mats[idsmap[id]]-1], EOS_PRESSURE );
      Cs[idsmap[id]][count] = eos_table_interp( P[i].Rho, P[i].S, eostable[mats[idsmap[id]]-1], EOS_SOUND_SPEED );
      tss[idsmap[id]][count] = P[i].TimeStep;

    }

    count ++;
    printf( ",%d", count );
    
  }

  printf( "]\n" );

  indexx( nFiles, times, timesindex );

  printf( "Saving output files to: %s\n", outputdir );
  for( i = 1; i <= npart; i ++ ) {

    char buf[200];
    sprintf( buf, "%s/%d.txt", outputdir, ids[i] );
    
    FILE * fp;
    if( !(fp = fopen(buf, "w")) ) {
      printf( "Could not open file for writing: %s", buf );
      exit(0);
    }
    fprintf( fp, "#Time\tx\ty\tz\tVx\tVy\tVz\tRho\tU\tNe\tHSML\tPotential\tAx\tAy\tAz\tS\tT\tP\tSoundSpeed\tTimeStep\n" );
    for( j = 1; j <= nFiles; j++ ) {

      fprintf( fp, "%f\tf\tf\tf\tf\tf\tf\tf\tf\tf\tf\tf\tf\tf\tf\tf\tf\tf\tf\tf\n",
	       times[timesindex[j]],
	       pxs[i][timesindex[j]], pys[i][timesindex[j]], pzs[i][timesindex[j]], 
	       vxs[i][timesindex[j]], vys[i][timesindex[j]], vzs[i][timesindex[j]], 
	       rhos[i][timesindex[j]], Us[i][timesindex[j]],
	       nes[i][timesindex[j]], hs[i][timesindex[j]], pots[i][timesindex[j]],
	       axs[i][timesindex[j]], ays[i][timesindex[j]], azs[i][timesindex[j]], 
	       Ss[i][timesindex[j]], Ts[i][timesindex[j]], Ps[i][timesindex[j]], 
	       Cs[i][timesindex[j]], tss[i][timesindex[j]] );

    }
    fclose(fp);

  }

  return(0);

}



int calculate_material( long int id ) {

  int i;
  for( i = 1; i <= nMat; i++ ) {
    if( id < i*idskip )
      break;
  }
  return i;

}

