#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globalvars.h"



int main (int argc, char *argv[])
{
  double *pres, *sigma, *peak, *sigma_peak;
  int *num, i;

  FILE *fp1;

  /*  readparam();
  */

  strcpy(snapbase,argv[1]);
  strcpy(fout,strcat(argv[1],"_long"));
  
  printf("fin is %s \n",snapbase);
  printf("fout is %s \n",fout);
  printf("arg is %s \n",argv[1]); 
  load_particles(i, &planet[0], &header[0]);
  pressure(&planet[0]);
  save_pressure(&planet[0], &header[0]);

  return 0;


}
