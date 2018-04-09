#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>

#include "globvars.h"

/**********************************************************
 Analyse
 Reads from snapshots in a directory specified in a parameter
 file and outputs temperature profiles, density profiles,
 and a table of mean speed and acceleration against snapshot
 number.
***********************************************************/

 

int main(int argc, char **argv) {  

  int i, j;
  char buf[200];

  if(argc != 2) {
    printf("\n>./Analyse <parameter file>\n\n");
    exit(0);
  }

  //read in parameters
  readparam( argv[1] );

  
  //set up EOS table
  struct eos_table *eostable[nMat];
  //load in EOS tables 
  for( i = 0; i < nMat; i++ ) {
    sprintf( buf, "%s%d.txt", eostablebase, i+1 );
    eostable[i] = eos_table_import( buf );
  }

  
  /* JMAA 7/2011 - was used for troubleshooting
  int I = 186; int J = 12;
  printf("Material 1 Table:\n");
  printf("For a density of: %f\n", eostable[0]->density[I]);
  printf("And an entropy of : %e\n", eostable[0]->entropy[J]);
  printf("You get a temperature of: %f\n", *((eostable[0]->thermo_quantities[EOS_TEMPERATURE])+(I + J*(eostable[0]->ND)) ) );
  printf("With eos_table_interp() you get T: %f\n", eos_table_interp( eostable[0]->density[I], eostable[0]->entropy[J], eostable[0], EOS_TEMPERATURE ) );
printf("With eos_table_interp() you get P: %e\n", eos_table_interp( eostable[0]->density[I], eostable[0]->entropy[J], eostable[0], EOS_PRESSURE ) );
printf("With eos_table_interp() you get U: %e\n", eos_table_interp( eostable[0]->density[I], eostable[0]->entropy[J], eostable[0], EOS_INT_ENERGY ) );
  //exit(0);
  */


  //use the dirent library to get a list of files in the input directory
  DIR *input;
  struct dirent *ent;
  input = opendir(inputdir);
  if( input == NULL ) {
    printf("Error opening directory %s.\n", inputdir);
    exit(0);
  }

  //get number of files in dir
  long int front = telldir( input );
  int nFiles = 0;
  while( (ent = readdir(input)) != NULL ) {
    char *snapbasebuf = strndup( ent->d_name, strlen(snapbase) );
    if( strcmp( snapbasebuf, snapbase ) == 0 ) {
      nFiles++;
    }
  }
  seekdir( input, front );


  //set up array to hold lines in the speed & acc vs number table
  char speedaccarr[nFiles][200];

  //loop over all of the files in the directory
  j = 0;
  while( (ent = readdir(input)) != NULL ) {
        
    //get the filename
    char filename[200];
    strcpy( filename, ent->d_name );
    //check for the correct sort of file, move on if not
    char *snapbasebuf = strndup( filename, strlen(snapbase) );
    if( strcmp( snapbasebuf, snapbase ) != 0 ) {
      continue;
    }

    //load this snapshot
    sprintf( buf, "%s/%s", inputdir, filename );
    load_snapshot( buf, _NOFILES );

    //Total number of particles by summing particle types
    long int N = 0;
    for( i = 0; i<6; i++ ) N += header1.npart[i];

    //get snapshot number
    char *snapnumber = strdup( filename+(strlen(filename)-3) );
 
    //set up procedures to analyse snapshots
    getspeedaccline( snapnumber, speedaccarr[j], N );
    printtprofile( snapnumber, N, eostable );
    printpprofile( snapnumber, N, eostable );
    printuprofile( snapnumber, N, eostable );
    printsprofile( snapnumber, N );
    printrhoprofile( snapnumber, N );
  
    j++;
    
  }
  
  //write speed & acc output
  FILE *fs;
  if( !(fs = fopen( saoutput, "w" )) ) {
    printf("Error opening file %s.\n", saoutput);
    exit(0);
  }
  for( j = 0; j < nFiles; j++ ) {
    fprintf( fs, "%s\n", speedaccarr[j] );
  }
  fclose(fs);
  
  //close directory
  closedir( input );
 
  return(0);

}



void getspeedaccline( char * snapnumber, char * line, long int N ) {
  /* columns: snapnumber averagespeed averageacc */

  double speed = 0.0;  //mean speed
  double acc = 0.0;    //mean acceleration
  int i = 0;

  //sum the speeds
  for ( i = 1; i <= N; i++ ) {
    
    speed += sqrt(
		  P[i].Vel[0]*P[i].Vel[0] +
		  P[i].Vel[1]*P[i].Vel[1] +
		  P[i].Vel[2]*P[i].Vel[2] 
		  );

    if( _HASACC ) {
      acc += sqrt(
		  P[i].Acc[0]*P[i].Acc[0] +
		  P[i].Acc[1]*P[i].Acc[1] +
		  P[i].Acc[2]*P[i].Acc[2]
		  );
    }
    
  }

  //Take the average and print output
  speed /= N;
  
  if( _HASACC ) {
    acc /= N;
    sprintf( line, "%s\t%lf\t%lf", snapnumber, speed, acc );
  } else {
    sprintf( line, "%s\t%lf", snapnumber, speed );
  }  
  
}



void printtprofile( char * snapnumber, long int N, struct eos_table** eos ) {

  FILE *fh;
  struct eos_table *EOS;
  int i = 0; int j = 0; int k = 0;
  double r = 0; double Tfinal = 0;
  char filename[200];
  //variables for if we're averaging over multiple particles per bin
  int averagecount = 0; double cumT = 0; double cumr = 0;

  sprintf( filename, "%s/tprofiles/%s%s.txt", poutput, tpbase, snapnumber );
  if( !(fh = fopen(filename, "w")) ) {
    printf("Unable to open file: %s\n",filename);
    exit(0);
  }

  double binminvalues[nRadialbins];
  double binmaxvalues[nRadialbins];

  //Work out min & max radii for radial bins
  for( i = 0; i < nRadialbins; i++ ) {
    binmaxvalues[i] = (i+1)*(outerR/nRadialbins);
    binminvalues[i] = i*(outerR/nRadialbins);
  }

  //Get one particle per bin
  for( i = 0; i < nRadialbins; i++ ) {

    //reset cumulative variables
    if( binaverage ) {
      averagecount = 0;
      cumT = 0;
      cumr = 0;
    }

    for( j = 1; j <= N; j++ ) {

      r = sqrt(P[j].Pos[0]*P[j].Pos[0] +
	       P[j].Pos[1]*P[j].Pos[1] +
	       P[j].Pos[2]*P[j].Pos[2] );

      if( (r <= binmaxvalues[i]) && (r > binminvalues[i]) ) {

	//particle belongs in this bin
	
	//loop through to find the material based on idskip
	int k = 0;                
	do {
	  k++;
	  if( P[j].Id < idskip*k ) break;
	} while( k < nMat );
	int mat = k;

	EOS = eos[mat-1];

	//get the temperature
	Tfinal = eos_table_interp( P[j].Rho, P[j].S, EOS, EOS_TEMPERATURE );


	/********************************** JMAA 7/2011 - was used for troubleshooting 
	if( j == 10000 && (strcmp( snapnumber, "000" ) == 0) ) {
	  printf("-------------------------------\n");
	  printf("Particle Index: %d\tSnapshot: %s\nDensity: %f\tEntropy: %e\nMaterial: %d\tRadius: %e\n", j, snapnumber, P[j].Rho, P[j].S, mat, r );
	  printf( "Temperature: %f\tPressure: %e\tU: %e\n", 
		  Tfinal,
		  eos_table_interp( P[j].Rho, P[j].S, EOS, EOS_PRESSURE ),
		  P[j].U );
	}
	*****************************************/

	if( binaverage ) {
	  cumT += Tfinal;
	  cumr += r;
	  averagecount ++;
	  if( averagecount == nBinaverage ) {
	    Tfinal = cumT/averagecount;
	    r = cumr/averagecount;
	    fprintf( fh, "%f\t%f\n", r, Tfinal );
	    break;
	  }
	} else {
	  fprintf( fh, "%f\t%f\n", r, Tfinal );
	  break;
	}

	
      }

    }

    //if we're averaging over all
    if( binaverage && (nBinaverage == 0) ) {
      Tfinal = cumT/averagecount;
      r = cumr/averagecount;
      fprintf( fh, "%f\t%f\n", r, Tfinal );
    }

  } 

  fclose( fh );

}



/*
 * as with printtprofile, but prints the pressure profile
 */
void printpprofile( char * snapnumber, long int N, struct eos_table **eos ) {

  FILE *fh;
  struct eos_table *EOS;
  int i = 0; int j = 0; int k = 0;
  double r = 0; double Pfinal = 0;
  char filename[200];
  //variables for if we're averaging over multiple particles per bin
  int averagecount = 0; double cumP = 0; double cumr = 0;

  sprintf( filename, "%s/pprofiles/%s%s.txt", poutput, prespbase, snapnumber );
  if( !(fh = fopen(filename, "w")) ) {
    printf("Unable to open file: %s\n",filename);
    exit(0);
  }

  double binminvalues[nRadialbins];
  double binmaxvalues[nRadialbins];

  //Work out min & max radii for radial bins
  for( i = 0; i < nRadialbins; i++ ) {
    binmaxvalues[i] = (i+1)*(outerR/nRadialbins);
    binminvalues[i] = i*(outerR/nRadialbins);
  }

  //Get one particle per bin
  for( i = 0; i < nRadialbins; i++ ) {

    //reset cumulative variables
    if( binaverage ) {
      averagecount = 0;
      cumP = 0;
      cumr = 0;
    }

    for( j = 1; j <= N; j++ ) {

      r = sqrt(P[j].Pos[0]*P[j].Pos[0] +
	       P[j].Pos[1]*P[j].Pos[1] +
	       P[j].Pos[2]*P[j].Pos[2] );

      if( (r <= binmaxvalues[i]) && (r > binminvalues[i]) ) {

	//particle belongs in this bin
	
	//loop through to find the material based on idskip
	int k = 0;                
	do {
	  k++;
	  if( P[j].Id < idskip*k ) break;
	} while( k < nMat );
	int mat = k;

	//get eos table
	EOS = eos[mat-1];

	//get the pressure
	Pfinal = eos_table_interp( P[j].Rho, P[j].S, EOS, EOS_PRESSURE );

	if( binaverage ) {
	  cumP += Pfinal;
	  cumr += r;
	  averagecount ++;
	  if( averagecount == nBinaverage ) {
	    Pfinal = cumP/averagecount;
	    r = cumr/averagecount;
	    fprintf( fh, "%f\t%f\n", r, Pfinal );
	    break;
	  }
	} else {
	  fprintf( fh, "%f\t%f\n", r, Pfinal );
	  break;
	}

	
      }

    }

    //if we're averaging over all
    if( binaverage && (nBinaverage == 0) ) {
      Pfinal = cumP/averagecount;
      r = cumr/averagecount;
      fprintf( fh, "%f\t%f\n", r, Pfinal );
    }

  } 

  fclose( fh );

}



/*
 * as with printtprofile, but prints the internal energy profile
 */
void printuprofile( char * snapnumber, long int N, struct eos_table **eos ) {

  FILE *fh;
  struct eos_table *EOS;
  int i = 0; int j = 0; int k = 0;
  double r = 0; double Ufinal = 0;
  char filename[200];
  //variables for if we're averaging over multiple particles per bin
  int averagecount = 0; double cumU = 0; double cumr = 0;

  sprintf( filename, "%s/uprofiles/%s%s.txt", poutput, upbase, snapnumber );
  if( !(fh = fopen(filename, "w")) ) {
    printf("Unable to open file: %s\n",filename);
    exit(0);
  }

  double binminvalues[nRadialbins];
  double binmaxvalues[nRadialbins];

  //Work out min & max radii for radial bins
  for( i = 0; i < nRadialbins; i++ ) {
    binmaxvalues[i] = (i+1)*(outerR/nRadialbins);
    binminvalues[i] = i*(outerR/nRadialbins);
  }

  //Get one particle per bin
  for( i = 0; i < nRadialbins; i++ ) {

    //reset cumulative variables
    if( binaverage ) {
      averagecount = 0;
      cumU = 0;
      cumr = 0;
    }

    for( j = 1; j <= N; j++ ) {

      r = sqrt(P[j].Pos[0]*P[j].Pos[0] +
	       P[j].Pos[1]*P[j].Pos[1] +
	       P[j].Pos[2]*P[j].Pos[2] );

      if( (r <= binmaxvalues[i]) && (r > binminvalues[i]) ) {

	//particle belongs in this bin
	
	//loop through to find the material based on idskip
	int k = 0;                
	do {
	  k++;
	  if( P[j].Id < idskip*k ) break;
	} while( k < nMat );
	int mat = k;

	//get eos table
	EOS = eos[mat-1];

	//get the internal energy
	Ufinal = eos_table_interp( P[j].Rho, P[j].S, EOS, EOS_INT_ENERGY );

	if( binaverage ) {
	  cumU += Ufinal;
	  cumr += r;
	  averagecount ++;
	  if( averagecount == nBinaverage ) {
	    Ufinal = cumU/averagecount;
	    r = cumr/averagecount;
	    fprintf( fh, "%f\t%f\n", r, Ufinal );
	    break;
	  }
	} else {
	  fprintf( fh, "%f\t%f\n", r, Ufinal );
	  break;
	}
	
      }

    }

    //if we're averaging over all
    if( binaverage && (nBinaverage == 0) ) {
      Ufinal = cumU/averagecount;
      r = cumr/averagecount;
      fprintf( fh, "%f\t%f\n", r, Ufinal );
    }

  } 

  fclose( fh );

}





void printrhoprofile( char * snapnumber, long int N ) {
  /*
    Converts the number of bins and the radius into actual bins (ranges)
    then scans through the particles until we have one per bin retrieving Rho
    for each of these particles */

  FILE *fh;
  int i = 0; int j = 0;
  double r = 0; double rho = 0;
  char filename[200];
  //variables for if we're averaging over muliple particles each bin
  int averagecount = 0; double cumRho = 0; double cumr = 0;

  //open file
  sprintf( filename, "%s/rhoprofiles/%s%s.txt", poutput, rhopbase, snapnumber );
  if( !(fh = fopen(filename, "w")) ) {
    printf("Unable to open file: %s\n",filename);
    exit(0);
  }

  double binminvalues[nRadialbins];
  double binmaxvalues[nRadialbins];

  //Work out min & max radii for radial bins
  for( i = 0; i < nRadialbins; i++ ) {
    binmaxvalues[i] = (i+1)*(outerR/nRadialbins);
    binminvalues[i] = i*(outerR/nRadialbins);
  }

  //Get one particle per bin
  for( i = 0; i < nRadialbins; i++ ) {

    if( binaverage ) {
      averagecount = 0;
      cumRho = 0;
      cumr = 0;
    }

    for( j = 1; j <= N; j++ ) {

      r = sqrt(P[j].Pos[0]*P[j].Pos[0] +
	       P[j].Pos[1]*P[j].Pos[1] +
	       P[j].Pos[2]*P[j].Pos[2] );

      if( (r <= binmaxvalues[i]) && (r > binminvalues[i]) ) {
	//particle belongs in this bin

	//get & print the radius & density

	if( binaverage ) {
	  cumRho += P[j].Rho;
	  cumr += r;
	  averagecount++;
	  if( averagecount == nBinaverage ) {
	    rho = cumRho/averagecount;
	    r = cumr/averagecount;
	    fprintf( fh, "%f\t%f\n", r, rho );
	    break;
	  }
	} else {
	  fprintf( fh, "%f\t%f\n", r, P[j].Rho ); //print the output
	  break;  //move onto the next bin
	}

      }

    }

    // if we're averaging over all of them
    if( binaverage && (nBinaverage == 0) ) {
      rho = cumRho/averagecount;
      r = cumr/averagecount;
      fprintf( fh, "%f\t%f\n", r, rho );
    }

  } 

  fclose( fh );

}




/*
 * As with printrhoprofile, but printing the entropy profile
 */
void printsprofile( char * snapnumber, long int N ) {

  FILE *fh;
  int i = 0; int j = 0;
  double r = 0; double s = 0;
  char filename[200];
  //variables for if we're averaging over muliple particles each bin
  int averagecount = 0; double cumS = 0; double cumr = 0;

  //open file
  sprintf( filename, "%s/sprofiles/%s%s.txt", poutput, spbase, snapnumber );
  if( !(fh = fopen(filename, "w")) ) {
    printf("Unable to open file: %s\n",filename);
    exit(0);
  }

  double binminvalues[nRadialbins];
  double binmaxvalues[nRadialbins];

  //Work out min & max radii for radial bins
  for( i = 0; i < nRadialbins; i++ ) {
    binmaxvalues[i] = (i+1)*(outerR/nRadialbins);
    binminvalues[i] = i*(outerR/nRadialbins);
  }

  //Get one particle per bin
  for( i = 0; i < nRadialbins; i++ ) {

    if( binaverage ) {
      averagecount = 0;
      cumS = 0;
      cumr = 0;
    }

    for( j = 1; j <= N; j++ ) {

      r = sqrt(P[j].Pos[0]*P[j].Pos[0] +
	       P[j].Pos[1]*P[j].Pos[1] +
	       P[j].Pos[2]*P[j].Pos[2] );

      if( (r <= binmaxvalues[i]) && (r > binminvalues[i]) ) {
	//particle belongs in this bin

	//get & print the radius & internal energy

	if( binaverage ) {
	  cumS += P[j].S;
	  cumr += r;
	  averagecount++;
	  if( averagecount == nBinaverage ) {
	    s = cumS/averagecount;
	    r = cumr/averagecount;
	    fprintf( fh, "%f\t%f\n", r, s );
	    break;
	  }
	} else {
	  fprintf( fh, "%f\t%f\n", r, P[j].S ); //print the output
	  break;  //move onto the next bin
	}

      }

    }

    // if we're averaging over all of them
    if( binaverage && (nBinaverage == 0) ) {
       s = cumS/averagecount;
      r = cumr/averagecount;
      fprintf( fh, "%f\t%f\n", r, s );
    }

  } 

  fclose( fh );

}
