/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include "stralloc.h"

int
stralloc_copy (stralloc * to, const stralloc * from)
{
	return stralloc_copyb (to, from->s, from->len);
}
