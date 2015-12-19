/*
 * reimplementation of Daniel Bernstein's byte library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include "byte.h"

int 
byte_diff(const char *s,unsigned int n,const char *t)
{
	for (;;) {
		if (!n) return 0; 
		if (*s != *t) break; 
		++s; ++t; --n;

		if (!n) return 0; 
		if (*s != *t) break; 
		++s; ++t; --n;

		if (!n) return 0; 
		if (*s != *t) break; 
		++s; ++t; --n;

		if (!n) return 0; 
		if (*s != *t) break; 
		++s; ++t; --n;
	}
	return ((int)(unsigned int)(unsigned char) *s)
	   - ((int)(unsigned int)(unsigned char) *t);
}
