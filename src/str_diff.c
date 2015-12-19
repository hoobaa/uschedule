/*
 * reimplementation of Daniel Bernstein's byte library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include "str.h"

int 
str_diff(const char *s,const char *t)
{
	unsigned int i=0;
	for (;;) {
		if (s[i]!=t[i]) break;
		if (!s[i]) break;
		i++;
	}
	return ((int)(unsigned int)(unsigned char) s[i])
		- ((int)(unsigned int)(unsigned char) t[i]);
}
