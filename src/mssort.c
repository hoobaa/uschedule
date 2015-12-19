#include "mssort.h"
#include "alloc.h"
#include "byte.h"
#include <stdlib.h> /* qsort: fallback */

void mssort (char *base, uint32 n, uint32 elesize,
                  int (*cmp) (const void *, const void *))
{
  char *v;
  char buf[64];
  const uint32 dists[]={
    1, 5, 19, 41, 109, 209, 505, 929, 
    2161, 3905, 8929, 16001, 36289, 64769, 146305, 260609, 
    587521, 1045505, 2354689, 4188161, 9427969, 16764929, 37730305, 67084289,
    0
  };
  int distp;
  if (n==1) return;
  if (elesize<=sizeof(buf))
    v=buf;
  else {
    v=alloc(elesize);
    if (!v)
      return qsort(base,n,elesize,cmp);
  }
  for (distp=1;dists[distp] && dists[distp]<n;distp++)
    /* nothing */;
  distp--;
  while(1) {
    uint32 h=dists[distp];
    uint32 i,j;
    for (i=h;i<n;i++) {
      j=i;
      byte_copy(v,elesize,base+i*elesize);
      while (j>=h && cmp(base+(j-h)*elesize,v)>0) {
	byte_copy(base+j*elesize,elesize,base+(j-h)*elesize);
	j-=h;
      }
      byte_copy(base+j*elesize,elesize,v);
    }
    if (!distp)
      break;
    distp--;
  }
  if (v!=buf)
    alloc_free(v);
}

