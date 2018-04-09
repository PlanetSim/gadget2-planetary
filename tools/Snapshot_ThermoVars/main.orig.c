#include <stdio.h>
#include <stdlib.h>

#include "globalvars.h"



int main (void)
{
  double *pres, *sigma, *peak, *sigma_peak;
  int *num, i;

  readparam();
  load_particles(i, &planet[0], &header[0]);
  pressure(&planet[0]);
  save_pressure(&planet[0], &header[0]);

  return 0;


}
