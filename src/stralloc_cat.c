/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */

#include "stralloc.h"

int stralloc_cat(stralloc *to,const stralloc *from)
{
	return stralloc_catb(to,from->s,from->len);
}
