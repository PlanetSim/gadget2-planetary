#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>

#include "globvars.h"

/**********************************************************
 FindPeak
 Takes a directory of Gadget2 snapshot files and outputs
 a table of all particles with their peak pressures, entropy
 at peak pressure, final entropy and whether they're
 escaping.
***********************************************************/



int main( int argc, char **argv ) {

  char pfile[200];
  int i, count;
  char buf[200];
  char snapbasebuf[200];
  unsigned long * indextable;
  int * ids;
  double * peakpressures;
  double * entropyat;
  double * peakentropy;
  double * temperatureat;
  int * snapforpeakp;
  int * snapforpeaks;
  int * material;

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
    //char *snapbasebuf = strndup( ent->d_name, strlen(snapbase) );
    if( strcmp( snapbasebuf, snapbase ) == 0 ) {
      nFiles++;
    }
  }
  seekdir( input, front );


  // loop through files
  int first = 1;
  while( (ent = readdir(input)) != NULL ) {

    //get the filename
    char filename[200];
    strcpy( filename, ent->d_name );
    //check for the correct sort of file, move on if not
    strncpy( snapbasebuf, filename, strlen(snapbase) );
    //char *snapbasebuf = strndup( filename, strlen(snapbase) );
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
      indextable = malloc( npart * sizeof(unsigned long) );
      indextable -= 1;
      ids = malloc( npart * sizeof(int) );
      ids -= 1;
      peakpressures = malloc( npart * sizeof(double) );
      peakpressures -= 1;
      entropyat = malloc( npart * sizeof(double) );
      entropyat -= 1;
      peakentropy = malloc( npart * sizeof(double) );
      peakentropy -= 1;
      temperatureat = malloc( npart * sizeof(double) );
      temperatureat -= 1;
      snapforpeakp = malloc( npart * sizeof(int) );
      snapforpeakp -= 1;
      snapforpeaks = malloc( npart * sizeof(int) );
      snapforpeaks -= 1;
      material = malloc( npart * sizeof(int) );
      material -= 1;
      
      //set up initial values
      for( i = 1; i <= npart; i++ ) {
	int mat = calculate_material( P[i].Id );
	ids[i]           = P[i].Id;
	peakpressures[i] = eos_table_interp( P[i].Rho, P[i].S, eostable[mat-1], EOS_PRESSURE );
	entropyat[i]     = P[i].S;
	temperatureat[i] = eos_table_interp( P[i].Rho, P[i].S, eostable[mat-1], EOS_TEMPERATURE );
	peakentropy[i]   = P[i].S;
	snapforpeakp[i]  = snapnum;
	snapforpeaks[i]  = snapnum;
	material[i]      = mat;
      }

      indexx( (unsigned long)npart, ids, indextable );

      first = 0;

      count = 0;
      printf( "Of %d files:\n[%d", nFiles, count );

    } else {

      for( i = 1; i <= npart; i++ ) {
	
	//get the index table index
	int index = 1;
	while( ids[indextable[index]] < P[i].Id ) index++;

	if( P[i].Id != ids[indextable[index]] ){
	  printf( "Error: Particle id %d does not match the indexed id %d in snapshot %d.\n",
		  P[i].Id, indextable[index], snapnum );
	  printf( "\tindextable[%d] = %d\n", index, indextable[index] );
	  printf( "\tids[%d] = %d\n", indextable[index], ids[indextable[index]] );
	  printf( "\tP[%d].Id = %d\n", i, P[i].Id );
	  exit(0);
	}
	
	double pressure = eos_table_interp( P[i].Rho, P[i].S, eostable[material[indextable[index]]-1], EOS_PRESSURE );
	if( pressure > peakpressures[ indextable[index] ] ) {
	  peakpressures[ indextable[index] ] = pressure;
	  entropyat[ indextable[index] ] = P[i].S;
	  temperatureat[ indextable[index] ] = eos_table_interp( P[i].Rho, P[i].S, eostable[material[indextable[index]]-1], EOS_TEMPERATURE );
	  snapforpeakp[ indextable[index] ] = snapnum;
	}
	if( P[i].S > peakentropy[ indextable[index] ] ) {
	  peakentropy[ indextable[index] ] = P[i].S;
	  snapforpeaks[ indextable[index] ] = snapnum;
	}

      }

      count ++;
      printf( ",%d", count );

    }
    
  }

  printf( "]\n" );

  printf( "Saving output: %s\n", outputfile );
  save_output( ids, peakpressures, entropyat, temperatureat, peakentropy, snapforpeakp, snapforpeaks, material, indextable );

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



void save_output( int * ids, double * pps, double * ea, double * ta, double * pe, int * spp, int * sps, int * mat, unsigned long * it ) {

  FILE *fp;
  int i = 0;

  if( !(fp = fopen( outputfile, "w")) ) {
    printf( "Error opening output file: %s.\n\n", outputfile );
    exit(0);
  }

  // write column headers
  fprintf( fp, "#Id\tPeakPressure\tEntropyAtPeakPressure\tTemperatureAtPeakPressure\tPeakEntropy\tSnapshotAtPeakPressure\tSnapshotAtPeakEntropy\tMaterial\n" );
  
  // write lines
  for( i = 1; i <= npart; i ++ ) {
    int j = it[i];
    fprintf( fp, "%d\t%f\t%f\t%f\t%f\t%d\t%d\t%d\n",
	     ids[j], pps[j], ea[j], ta[j], pe[j], spp[j], sps[j], mat[j] );
  }

  fclose( fp );

}
