#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "globalvars.h"


int main(int argc, char *argv[])
{
  double fold;
  double fg, Ltot, Mm;
  int count=0;


  if(argc==2)
    {
      sscanf(argv[1],"%s",snapbase);
    }else
    {
      readparam();
    }

  load_particles(&planet[0], &header[0]);

  
  bound(&planet[0]);

  //save_snapshot(&planet[0], &header[0]);
  return 0;


}
