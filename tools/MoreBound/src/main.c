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
 *
 * May 2023: updated by Fergus 
 *  ++ multi-threading and modifying underlying data structures
 *  ++ made the program a little more verbose as a heartbeat
 */

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: ./MoreBound <snapshot file> <parameter file>\n\n");
    return 0;
  }
  readparam(argv[1], argv[2]);
  printf("Configuration file read. Max iterations %d\n", maxit);

  ParticleData pd;
  IOHeader header;
  
  int status = pd_read(&pd, &header);
  if (status < 0) {
    return -status;
  }
  printf("Read in %ld objects with total mass %g\n", pd.total_number,
         pd.total_mass);

  calculate_binding(&pd);
  printf("Writing output to '%s'\n", fout);

  status = save_output(&pd, fout);
  if (status < 0) {
    return -status;
  }

  // free allocated memory
  pd_free(&pd);
  return 0;
}
