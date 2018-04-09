#include <stdio.h>
#include "nrutils/nrutil.h"
#include "globvars.h"

#define SWAP(a,b) itemp=(a);(a)=(b);(b)=itemp
#define M 7
#define NSTACK 50

/*
 * indextable.c
 * Algorithm for creating an index table for an array. From "Numerical Recipes
 * in C" 2nd Ed.
 */

void indexx( unsigned long n, int arr[], unsigned long indx[] ) {
  /* Indexes an array arr[1..n] */

  unsigned long i, indxt, ir=n, itemp, j, k, l=1;
  int jstack = 0, *istack;
  int a;
  istack = ivector(1,NSTACK);
  for( j=1; j<=n; j++ ) indx[j]=j;

  for(;;) {

    if( ir-l < M ) {

      for( j=l+1; j<=ir; j++ ) {
	indxt = indx[j];
	a = arr[indxt];
	for( i = j-1; i >= l; i-- ) {
	  if( arr[indx[i]] <= a ) break;
	  indx[i+1]=indx[i];
	}
	indx[i+1]=indxt;
      }
      if( jstack == 0 ) break;
      ir = istack[jstack--];
      l = istack[jstack--];

    } else {
      
      k = (l+ir) >> 1;
      SWAP( indx[k], indx[l+1] );
      if( arr[indx[l]] > arr[indx[ir]] ) {
	SWAP( indx[l], indx[ir] );
      }
      if( arr[indx[l+1]] > arr[indx[ir]] ) {
	SWAP( indx[l+1], indx[ir] );
      }
      if( arr[indx[l]] > arr[indx[l+1]] ) {
	SWAP( indx[l], indx[l+1] );
      }
      i = l+1;
      j = ir;
      indxt = indx[l+1];
      a = arr[indxt];
      for(;;) {
	do i++; while( arr[indx[i]] < a );
	do j--; while( arr[indx[j]] > a );
	if( j < i ) break;
	SWAP( indx[i], indx[j] );
      }
      indx[l+1] = indx[j];
      indx[j] = indxt;
      jstack += 2;
      if( jstack > NSTACK ) nrerror( "NSTACK too small in indexx." );
      if( ir-i+1 >= j-l ) {
	istack[jstack] = ir;
	istack[jstack-1] = i;
	ir = j-1;
      } else {
	istack[jstack] = j-1;
	istack[jstack-1] = l;
	l = i;
      }
    }
    
  }

  free_ivector( istack, 1, NSTACK );
  
}
