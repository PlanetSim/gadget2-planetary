#include "globalvars.h"


char snapbase[25];    // base name for the snapshot files
char fout[25];        // output file
int  opt;             // option that specifies output in energy field
                      // 0 for entropy, 1 for pressure, 2 for temperature

struct planet_data planet[1];
struct io_header header[1];

