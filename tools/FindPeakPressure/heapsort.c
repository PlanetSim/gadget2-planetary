include "globvars.h"


/*
 * heapsort.c
 * algorithm for heap sort from "Numerical Recipes in C" 2nd Ed
 */

void hpsort( unsigned long n, float ra[] ) {
  /* Sorts an array into ascending numerical order using the Heapsort algorithm
   * n is input; ra is replaced on output by its sorted arrangement */
  
  unsigned long i,ir,j,l;
  float rra;

  if( n < 2 ) return;
  l = ( n >> 1 ) + 1; // half it and add one
  ir = n;
  /* The index l will be decremented from its initial value down to 1 during
   * the heap-creation phase. Once it reaches 1 the index ir will be
   * decremented from its initial value down to 1 during the heap selection
   * phase */
  for(;;) {
    if( l > 1 ) {    // still creating the heap
      rra = ra[--l];
    } else {         // heap selection phase
      rra = ra[ir];      //clear space at the end
      ra[ir] = ra[1];    //
      if( --ir == 1 ) { 
	ra[1] = rra;
	break;
      }
    }
    /* Set up to sift down element rra to its proper level (regardless of 
     * phase */
    i = l;
    j = l+l;
    while( j <= ir ) {
      if( j < ir && ra[j] < ra[j+1] ) j++; //compare to the greater underling
      if( rra < ra[j] ) { // demote rra
	ra[i] = ra[j];
	i=j;
	j <<= 1;
      } else break; //found the correct level, terminate sift
    }
    ra[i] = rra;
  }
}
