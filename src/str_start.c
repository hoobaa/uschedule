/*
 * reimplementation of Daniel Bernstein's byte library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include "str.h"

int 
str_start(const char *s,const char *t)
{
	unsigned int i=0;
	for (;;) {
		if (!t[i]) return 1;
		if (s[i]!=t[i]) return 0;
		i++;

		if (!t[i]) return 1;
		if (s[i]!=t[i]) return 0;
		i++;

		if (!t[i]) return 1;
		if (s[i]!=t[i]) return 0;
		i++;

		if (!t[i]) return 1;
		if (s[i]!=t[i]) return 0;
		i++;
	}
}
