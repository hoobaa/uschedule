/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include "str.h"
#include "stralloc.h"

int
stralloc_cats (stralloc * sa, const char *str)
{
	return stralloc_catb (sa, str, str_len (str));
}
