#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "globvars.h"

/**********************************************************
 SnapToTable
 Converts a Gadget2 snapshot file into an ASCII table suitable
 for plotting with, e.g., topcat
***********************************************************/


int CalculateMaterial( long int );


int main(int argc, char **argv)
{
  char input_fname[200];
  char output_fname[200];
  char buf[200];

  int  i, j, k;
  int id = 0;
  int material = 0;
  int position = 0;
  int radius = 0;
  int velocity = 0;
  int mass = 0;
  int internalenergy = 0;
  int density = 0;
  int hsml = 0;
  int potential = 0;
  int acceleration = 0;
  int entropy = 0;
  int timestep = 0;
  int temperature = 0;
  int pressure = 0;
  int soundspeed = 0;

  if( argc != 4 ) {
      
    printf("\n>./SnapToTable <snapshot filename> <output filename> <options>\n\n");
    printf("Options are to INCLUDE variables in the table.\n");
    printf("(N.B. Potential, acceleration, internal energy and timestep\n");
    printf("cannot be included if they're not in the snapshots and\n");
    printf("globvars.h should have flags that reflect this at compile.)\n");
    printf("-i particle ids\n");
    printf("-M material\n");
    printf("-x position\n");
    printf("-r radius\n");
    printf("-v velocity\n");
    printf("-m mass\n");
    printf("-U internal energy\n");
    printf("-R density\n");
    printf("-h hsml smoothing length\n");
    printf("-p potential\n");
    printf("-a acceleration\n");
    printf("-S entropy\n");
    printf("-t timestep\n");
    printf("-T temperature\n");
    printf("-P pressure\n");
    printf("-c sound speed\n\n");
    printf("-A all of the above\n\n");

    exit(0);

  }

  strcpy(input_fname, argv[1]);
  strcpy(output_fname, argv[2]);
  
      
  char options[14];
  strcpy( options, argv[3] );

  for( i=0; i < (strlen(options)-1); i++ ) {
    
    switch( options[i+1] ) {
    case 'i':
      id = 1;
      break;
    case 'M':
      material = 1;
      break;
    case 'x':
      position = 1;
      break;
    case 'r':
      radius = 1;
      break;
    case 'v':
      velocity = 1;
      break;
    case 'm':
      mass = 1;
      break;
    case 'U':
      if( _HASU )
	internalenergy = 1;
      break;
    case 'R':
      density = 1;
      break;
    case 'h':
      hsml = 1;
      break;
    case 'p':
      if( _HASPOT ) 
	potential = 1;
      break;
    case 'a':
      if( _HASACC )
	acceleration = 1;
      break;
    case 'S':
      entropy = 1;
      break;
    case 't':
      if( _HASSTEP )
	timestep = 1;
      break;
    case 'T':
      temperature = 1;
      break;
    case 'P':
      pressure = 1;
      break;
    case 'c':
      soundspeed = 1;
      break;
    case 'A':
      id = material = position = radius = velocity = mass = internalenergy = density = hsml = potential = acceleration = entropy = timestep = temperature = pressure = soundspeed = 1;
      break;
    }

  }

  load_snapshot( input_fname, _NOFILES );

  //Total number of particles by summing particle types
  long int N = 0;
  for( i = 0; i<6; i++ ) N += header1.npart[i];


  //set up EOS table
  int nMat = _NOMATERIALS;
  struct eos_table *eostable[nMat];
  //load in EOS tables 
  for( i = 0; i < nMat; i++ ) {
    sprintf( buf, "%s%d.txt", eostablebase, i+1 );
    eostable[i] = eos_table_import( buf );
  }

  
  
  //Find the number of columns
  int cols = id + material + 3*position + radius + 3*velocity + mass + internalenergy + density + hsml + potential + 3*acceleration + entropy + timestep + temperature + pressure + soundspeed;


  //check that /any/ columns are asked for
  if( cols == 0 ) {
    printf("Please specify some columns to include via the options.\n");
    exit(0);
  }

  //open the file for saving results
  FILE *fh;
  if( !(fh = fopen(output_fname, "w")) ) {
	printf("Error opening file %s\n", output_fname);
	exit(0);
  }


  //write headings
  fprintf( fh, "#" );
  if( id )  fprintf( fh, "id\t" );
  if( material )  fprintf( fh, "material\t" );
  if( position ) {
    fprintf( fh, "x\t" );
    fprintf( fh, "y\t" );
    fprintf( fh, "z\t" );
  }
  if( radius ) fprintf( fh, "radius\t" );
  if( velocity ) {
    fprintf( fh, "Vx\t" );
    fprintf( fh, "Vy\t" );
    fprintf( fh, "Vz\t" );
  }
  if( mass )  fprintf( fh, "mass\t" );
  if( internalenergy )  fprintf( fh, "U\t" );
  if( density )  fprintf( fh, "Rho\t" );
  if( hsml )  fprintf( fh, "HSML\t" );
  if( potential )  fprintf( fh, "potential\t" );
  if( acceleration ) {
    fprintf( fh, "Ax\t" );
    fprintf( fh, "Ay\t" );
    fprintf( fh, "Az\t" );
  }
  if( entropy )  fprintf( fh, "S\t" );
  if( timestep )  fprintf( fh, "Timestep\t" );
  if( temperature )  fprintf( fh, "T\t" );
  if( pressure )  fprintf( fh, "P\t" );
  if( soundspeed ) fprintf( fh, "Soundspeed\t" );
  fprintf( fh, "\n" );


  //loop over all of the particles, getting and printing the data to the output file
  for( i = 1; i <= N; i++ ) {

    k = 0;
    char line[cols][100]; //array of strings for a line
    int Pmat;
    double PT, PP, PU, PC;  //particles material, temperature, pressure, internal energy & sound speed

    if( id ) {
      sprintf(line[k],"%d",P[i].Id);
      k++;
    }
    Pmat = CalculateMaterial( P[i].Id );
    if( material ) {
      sprintf(line[k],"%d", Pmat );
      k++;
    }
    if( position ) {
      sprintf(line[k],"%f",P[i].Pos[0]);
      k++;
      sprintf(line[k],"%f",P[i].Pos[1]);
      k++;
      sprintf(line[k],"%f",P[i].Pos[2]);
      k++;
    }
    if( radius ) {
      sprintf(line[k],"%f",sqrt( P[i].Pos[0]*P[i].Pos[0] +
				 P[i].Pos[1]*P[i].Pos[1] +
				 P[i].Pos[2]*P[i].Pos[2] ) );
      k++;
    }
    if( velocity ) {
      sprintf(line[k],"%f",P[i].Vel[0]);
      k++;
      sprintf(line[k],"%f",P[i].Vel[1]);
      k++;
      sprintf(line[k],"%f",P[i].Vel[2]);
      k++;
    }
    if( mass ) {
      sprintf(line[k],"%f",P[i].Mass);
      k++;
    }
    
    if( internalenergy ) {
      PU = eos_table_interp( P[i].Rho, P[i].S, eostable[Pmat-1], EOS_INT_ENERGY );
      sprintf(line[k],"%f", PU);
      k++;
    }
    if( density ) {
      sprintf(line[k],"%f",P[i].Rho);
      k++;
    }
    if( hsml ) {
      sprintf(line[k], "%f", P[i].hsml);
      k++;
    }
    if( potential ) {
      sprintf(line[k],"%f",P[i].Potential);
      k++;
    }
    if( acceleration ) {
      sprintf(line[k],"%f",P[i].Acc[0]);
      k++;
      sprintf(line[k],"%f",P[i].Acc[1]);
      k++;
      sprintf(line[k],"%f",P[i].Acc[2]);
      k++;
    }
    if( entropy ) {
      sprintf(line[k],"%f",P[i].S);
      k++;
    }
    if( timestep ) {
      sprintf(line[k],"%f",P[i].TimeStep);
      k++;
    }
    
    if( temperature ) {
      PT = eos_table_interp( P[i].Rho, P[i].S, eostable[Pmat-1], EOS_TEMPERATURE );
      sprintf( line[k], "%f", PT );
      k++;
    }
    
    if( pressure ) {
      PP = eos_table_interp( P[i].Rho, P[i].S, eostable[Pmat-1], EOS_PRESSURE );
      sprintf( line[k], "%f", PP );
      k++;
    }

    if( soundspeed ) {
      PC = eos_table_interp( P[i].Rho, P[i].S, eostable[Pmat-1], EOS_SOUND_SPEED );
      sprintf( line[k], "%f", PC );
      k++;
    }

    //collate the line and print it to file
    fprintf( fh, "%s", line[0] );
    for( j = 1; j < cols; j++ ) {
      fprintf( fh, "\t%s", line[j] );
    }
    fprintf( fh, "\n" );
       
  }

  fclose( fh );

  return(0);
}



int CalculateMaterial( long int id ) {

  int i;
  for( i = 1; i <= _NOMATERIALS; i++ ) {
    if( id < i*_IDSKIP )
      break;
  }
  return i;

}
