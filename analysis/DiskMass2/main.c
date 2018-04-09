#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "globalvars.h"


int main(void)
{
  double fold;
  double fg, Ltot, Mm;
  int count=0;


  readparam();

  load_particles(&planet[0], &header[0]);

  
  findCM(&planet[0]);
  changeCoords(&planet[0]);  // now in COM coordinates


  fold = 0.0;
  f = 0.05;
  Mpe  = 0.8 * Mearth;

  bound(&planet[0]);


  while(fabs(fold-f) > tol && count < maxit)
  {
      fold = f;
      findDiskMass(&planet[0]);
  }


  Mm = 1.9*Ld/sqrt(G*Mearth*2.9*Rearth) - 1.195*Md;

  printf("\n\n\n\n");
  printf("N/10^3:  %d\n", planet[0].Ntot/1000);
  printf("gamma:   %.2f\n", gamma1);
  printf("b:       %.3f\n", b);
  printf("Me/M_L:  %.2f\n", Mesc/Mlun);
  printf("Md/M_L:  %.2f\n", Md/Mlun);
  printf("Ld/Lem:  %.2f\n", Ld/Lem);
  printf("M_Fe/Md: %.2f\n", Mfe/Md);
  printf("Mimp/Md: %.2f\n", Mimp/Md);
  printf("Lf/Lem:  %.2f\n", (Lpe+Ld)/Lem);
  printf("M_M/M_L: %.2f\n", Mm/Mlun);
  printf("\n\n\n\n");

  //save_snapshot(&planet[0], &header[0]);
  return 0;


}
