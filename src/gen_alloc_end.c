/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include "gen_alloci.h"
#include "byte.h"
int 
gen_alloc_append(char **bptr, unsigned int bsize, unsigned int *len, 
	unsigned int *a, const char *add)
{
	if (!gen_alloc_ready(bptr,bsize,len,a, *len+1))
		return 0;
	byte_copy(*bptr+bsize*(*len),bsize,add); /* *len is now valid */
	(*len)++;
	return 1;
}
