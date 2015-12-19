/*
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include "stralloc.h"
#include "alloc.h"
#include "str.h"
#include "api_dir.h"
#include "mssort.h"

int
api_dir_sort(stralloc *sa, int (*fn)(const void *, const void *))
{
  char **a;
  unsigned int n;
  unsigned int i;
  for (i=0,n=0;i<sa->len;) {
    n++;
    i+=str_len(sa->s+i)+1;
  }
  a=(char **)alloc(sizeof (char*) * n);
  if (!a) return -1;
  for (i=0,n=0;i<sa->len;) {
    a[n]=sa->s+i;
    n++;
    i+=str_len(sa->s+i)+1;
  }
  mssort((char *)a,n,sizeof(char *),fn);
  alloc_free((char *)a);
  return 0;
}
