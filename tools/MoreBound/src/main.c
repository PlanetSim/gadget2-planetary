#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "globalvars.h"
#include "particledata.h"

/*
 * MoreBound
 * Derives from (and works the same as) BoundMass but then continues to
 * to find the 2nd largest etc. remnant down to a minimum number of
 * particles per remnant (added to the parameter file).
 * The "bound" column in the output file then refers to which remnant
 * (in order of size) that particle is bound to (zero being unbound).
 */

/*
 * BoundMass
 * Program originally by RAM to calculate the bound mass after a planetary
 * collision from a Gadget output. Outputs a table of particle ids & a 1 or
 * 0 based on whether they're considered bound or not. Commented and edited
 * by JMAA
 */

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: ./MoreBound <snapshot file> <parameter file>\n\n");
    exit(0);
  }
  readparam(argv[1], argv[2]);

  printf("Configuration file read. Max iterations %d\n", maxit);

  ParticleData pd;
  IOHeader header;

  pd_read(&pd, &header);
  printf("LOAD DONE\n");

  printf("Read in %d objects with total mass %g\n", pd.total_number,
         pd.total_mass);

  calculate_binding(&pd);
  pd_free(&pd);
  return 0;
}
