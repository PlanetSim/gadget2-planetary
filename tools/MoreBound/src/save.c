#include "particledata.h"

#include <stdio.h>

int save_output(const ParticleData *pd, char *filename) {
  FILE *fp = fopen(filename, "w");

  if (!fp) {
      printf("Unable to open file '%s' for writing\n", fout);
      return -1;
  }

  fprintf(fp, "#Id\tBound\n");
  for (size_t i = 0; i < pd->total_number; ++i) {
    fprintf(fp, "%d\t%d\n", pd->id[i], pd->bnd[i]);
  }

  fclose(fp);
  return 0;
}
